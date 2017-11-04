# powerMeter

This is an example base program that reads an ADC value (that happens to be proportional to power) and prints it over a USB CDC virtual serial port.

This is an Eclipse/gcc-eabi-none project based on ASF v4 

The general structure is of a non-blocking control loop that processes commands allowing some background process to run

Current target is ATSAMD11D14 but will change to ATSAMD11C 

## Known issues:

* Continuous print statements will cause the flags to break 
