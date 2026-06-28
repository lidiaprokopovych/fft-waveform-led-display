
========================
UART REGISTER BREAKDOWN
========================
UCSR0A - status register, contains flags for successful receives or errors that you read from
UCSR0B - control register, contains flags to turn on transmitter/receiver, enable interrupts 
UCSR0C - format register, used to configure character size, stop bits (1 or 2), parity mode


========================
UART INITIALIZATION
========================
For the ATmega328P chip that I am using, the following must be done to init UART protocol:
1. Set UCSR0C format
		UMSEL0 for asynch or sync UART (0 = asynch, 1 = sync)
		UPMn1/UPMn0 for parity 
		USBS0 for stop bits
2. 	
