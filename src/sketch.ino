/*
-------------How to wire Arduino to Smart meter using RJ11 cable-------------
Note: make sure to place a small resistor between between 5V & ground. Current from an Arduino output pin is 40mA, while 30mA is allowed on the P1 port.
Yellow to pin 9 (TX on meter, RX on arduino)
Red to ground	(Yeah... illogical)
Black to pin 4 or 5V (5V, also counter-intuitive)

-------------Example P1 telegram----------------------------
/ISk5\2ME382-1003

0-0:96.1.1(4B414C37303035303739393336333132)
1-0:1.8.1(00053.950*kWh)	-> Used Low Tariff
1-0:1.8.2(00081.586*kWh)	-> Used High Tariff
1-0:2.8.1(00003.303*kWh)	-> Supplied Low Tariff
1-0:2.8.2(00009.299*kWh)	-> Supplied High Tariff
0-0:96.14.0(0002)		-> Current Tariff
1-0:1.7.0(0000.03*kW)		-> Current Usage
1-0:2.7.0(0000.00*kW)
0-0:17.0.0(0999.00*kW)		-> Limit / Useless
0-0:96.3.10(1)			-> Vendor specific
0-0:96.13.1()			-> Vendor specific
0-0:96.13.0()			-> Vendor specific
!
*/

#include <SoftwareSerial.h>
#include <SPI.h>

const int RTSpin =  4;
int incomingByte = 0;
char buffer[512];	// Overkill
int counter=0;
int timer=0;

SoftwareSerial mySerial(9, 8, true); // RX, TX, inverted

void setup () {
	Serial.begin(115200);
	mySerial.begin(9600);
	delay(1000);

	pinMode(RTSpin, OUTPUT);
	digitalWrite(RTSpin, HIGH);
	Serial.print("Arduino Smart-Meter Relay/Translator v0.1\r\n\n" );

	buffer[counter] = '\0';
	timer=millis();
}

void loop ()
{
	// If we haven't seen any new characters for 2 seconds, we must be done,
	// so print the buffer, as the next one will arrive in another 8 seconds
	int timer2 = millis();
	if ( (  timer2 > (timer+2000) ) || ( timer2 < timer) )
	{
		buffer[counter++] = '\0';
		Serial.print(buffer);
		counter = 0;
		buffer[counter] = '\0';
		timer=millis();
	}

	while (mySerial.available() > 0)
	{
		// Pull 8th bit to zero
		incomingByte = (mySerial.read() & B01111111);
		buffer[counter++] = incomingByte;
	}
}
