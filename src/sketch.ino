/*
 * DSRM Telegram reader version 0.3
 * 2018/07/27 -- Mark Janssen -- Sig-I/O Automatisering
 *
 * Public Domain
 */

/*
 * Wiring arduino to RJ11 / P1 Port for the DSMR

-------------How to wire Arduino to Smart meter using RJ11 cable-------------
Note: make sure to place a small resistor between between 5V & ground. Current from an Arduino output pin is 40mA, while 30mA is allowed on the P1 port.
Yellow to pin 10 (TX on meter, RX on arduino)
Green: NC (RX on meter, unused)
Red to ground   (Yeah... illogical)
Black to pin 4 or 5V (5V, also counter-intuitive)

-------------Example P1 telegram----------------------------
/ISk5\2ME382-1003           -> Beginning of report

0-0:96.1.1(4B414C37303035303739393336333132)
1-0:1.8.1(00053.950*kWh)    -> Used Low Tariff
1-0:1.8.2(00081.586*kWh)    -> Used High Tariff
1-0:2.8.1(00003.303*kWh)    -> Supplied Low Tariff
1-0:2.8.2(00009.299*kWh)    -> Supplied High Tariff
0-0:96.14.0(0002)           -> Current Tariff
1-0:1.7.0(0000.03*kW)       -> Current Usage
1-0:2.7.0(0000.00*kW)       
0-0:17.0.0(0999.00*kW)      -> Limit
0-0:96.3.10(1)              -> Vendor specific
0-0:96.13.1()               -> Vendor specific
0-0:96.13.0()               -> Vendor specific
!                           -> End of report
*/

#include <SoftwareSerial.h>
#include <SPI.h>
#define BUFSIZE 400
#define VERSION "v0.3"

const int   RTSpin = 4;             // To tell DSMR meter to send reports
char        buffer[BUFSIZE];        // Buffer to read received bytes into
char        received;               // Current received byte
int         counter = 0;            // Number of Bytes in buffer
int         lasttime = 0;           // millis() at last printed report
int         currenttime = 0;        // millis() currently
int         interval=10000;         // Every X-millis() to print a report

// Use SoftwareSerial to talk to DSMR, with RX on port 10, TX is un-used by this sketch
SoftwareSerial DSMR(10,11, true);   // RX (pin 10), TX(pin 11, unused, inverted signals)

void setup () {
    // Ask DSMR to remain silent
    pinMode(RTSpin, OUTPUT);
    digitalWrite(RTSpin, LOW);

    // Initialize Serial Ports
    Serial.begin(115200);           // Port towards computer
    DSMR.begin(9600);               // Port towards DSMR

    delay(50);
    Serial.print("Arduino Smart-Meter Relay/Translator VERSION\r\n\n" );

    buffer[counter] = '\0';
    lasttime=millis();
    currenttime=millis();

    // Ask DSMR to begin reports
    digitalWrite(RTSpin, HIGH);
}

void loop ()
{
    currenttime = millis();

    if ( currenttime < lasttime )
    {
       // we looped, just reset last loop time to 0, we might print with a slight delay then
       // loops only happen every 49+ days
       lasttime = 0;
    }
 
    if ( currenttime > (lasttime+interval-500) )
    {
        // We are almost at report-time (1/2 second before)
        // stop asking for data, so we can handle the last incoming bytes
        // without getting a new report
        digitalWrite(RTSpin, LOW);
    }
          
    if ( (  currenttime > (lasttime+interval) ) )
    {
        // It's time for a new printed report
        // Write null byte to end the string
        buffer[counter++] = '\0';
        // Print buffer to PC
        Serial.print(buffer);

        // Reset counters and buffers to clean state
        counter = 0;
        buffer[0] = '\0';
        lasttime=millis();

        // Ask DSMR to send new reports / data
        digitalWrite(RTSpin, HIGH);
    }

    // Is there serial data to be read from the DSMR
    while (DSMR.available() > 0)
    {
        // Pull 8th bit to zero
        received = (DSMR.read() & 0x7F);
        buffer[counter++] = received;
    }
}
