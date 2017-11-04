/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "atmel_start.h"
#include "usb_start.h"

#define cdcBufferSize 64

#if CONF_USBD_HS_SP
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_LS_FS};
static uint8_t single_desc_bytes_hs[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_HS_DESCES_HS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ_HS
#else
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    CDCD_ACM_DESCES_LS_FS};
#define CDCD_ECHO_BUF_SIZ CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ
#endif

static struct usbd_descriptors single_desc[]
    = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
       ,
       {single_desc_bytes_hs, single_desc_bytes_hs + sizeof(single_desc_bytes_hs)}
#endif
};

// Tx and Rx CDC buffers
uint8_t cdcTxBuffer[cdcBufferSize];
uint8_t cdcTxIndex = 0;
uint8_t cdcRxBuffer[1];

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

volatile uint8_t writeCompleteFlag = 0;
volatile uint8_t readCompleteFlag = 0;
volatile uint8_t readCompleteFlagNonBlocking = 2;


static bool readComplete(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	readCompleteFlag = 1;
	if(readCompleteFlagNonBlocking == 0)readCompleteFlagNonBlocking = 1;
	return false;
}

static bool writeComplete(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	writeCompleteFlag = 1;
	/* No error. */
	return false;
}

/**
 * \brief Callback invoked when Line State Change
 */
static bool usb_device_cb_state_c(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR) {
		/* Callbacks must be registered after endpoint allocation */
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)readComplete);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)writeComplete);
	}

	/* No error. */
	return false;
}

/**
 * \brief CDC ACM Init
 */
void cdc_device_acm_init(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	cdcdf_acm_init();

	usbdc_start(single_desc);
	usbdc_attach();
}

//setup the ACM device and callbacks
void acm_setup(void)
{
	while (!cdcdf_acm_is_enabled());//wait for CDC to be enabled
	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_c); // register callbacks
}

void usb_init(void)
{
	cdc_device_acm_init();
}

//flush whatever is in the Tx buffer out the USB CDC
void cdcFlush(void)
{
	if(cdcTxIndex > 0)
	{
		writeCompleteFlag = 0;
		cdcdf_acm_write(cdcTxBuffer, cdcTxIndex);
		while(writeCompleteFlag == 0); // block until data sent
		cdcTxIndex = 0;
	}
}

void putCharUSB(uint8_t outChar)
{
	cdcTxBuffer[cdcTxIndex++] = outChar;
	if(cdcTxIndex == cdcBufferSize)cdcFlush();

}
//returns null if no char ready
uint8_t getCharUSB(void)
{
	uint8_t inChar = '\0';
	readCompleteFlag = 0;
	cdcdf_acm_read(&inChar, 1);
	while(readCompleteFlag == 0); // block until data read
	return inChar;
}

//returns null if no character received
uint8_t getCharUSBnonBlocking(void)
{
	uint8_t rValue = '\0';
	if(readCompleteFlagNonBlocking == 1)
	{
		rValue = cdcRxBuffer[0];
		readCompleteFlagNonBlocking = 0;
		cdcdf_acm_read(cdcRxBuffer, 1);
	}
	//happens only the first time this is called
	else if(readCompleteFlagNonBlocking == 2)
	{
		readCompleteFlagNonBlocking = 0;
		cdcdf_acm_read(cdcRxBuffer, 1);
	}
	return rValue;
}




