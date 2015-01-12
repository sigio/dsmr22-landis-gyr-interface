/*
-------------How to wire Arduino to Smart meter using RJ11 cable-------------
Note: make sure to place a small resistor between between 5V & ground. Current from an Arduino output pin is 40mA, while 30mA is allowed on the P1 port.
Yellow to pin 9
Red to ground
Black to pin  4

-------------Example P1 telegram----------------------------
/ISk5\2ME382-1003

0-0:96.1.1(4B414C37303035303739393336333132)
1-0:1.8.1(00053.950*kWh)
1-0:1.8.2(00081.586*kWh)
1-0:2.8.1(00003.303*kWh)
1-0:2.8.2(00009.299*kWh)
0-0:96.14.0(0002)
1-0:1.7.0(0000.03*kW)
1-0:2.7.0(0000.00*kW)
0-0:17.0.0(0999.00*kW)
0-0:96.3.10(1)
0-0:96.13.1()
0-0:96.13.0()
!
*/

#include <SoftwareSerial.h>
#include <SPI.h>

const int RTSpin =  4;
int incomingByte = 0;

SoftwareSerial mySerial(9, 8, true); // RX, TX, inverted

void setup () {
	Serial.begin(115200);
	mySerial.begin(9600);
	delay(1000);

	pinMode(RTSpin, OUTPUT);
	digitalWrite(RTSpin, HIGH);
}

void loop () {
	while (mySerial.available() > 0) {
		incomingByte = mySerial.read();
		incomingByte &= ~(1 << 7);  // forces 0th bit to be 0. all other bits left alone.
	  
		Serial.write(incomingByte);
	}
}
