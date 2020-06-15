
#include <ESP8266WiFi.h>
#include <TimeLib.h>         // for Date/Time operations
#include <WiFiUdp.h>         // for UDP NTP

#include <SPI.h>             // Serial Peripheral Interface


char ssid[] = "ssid";       //  your network SSID (name)
char pass[] = "password";       // your network password

// NTP Servers:
IPAddress timeServer(195, 186, 4, 101); // 195.186.4.101 (bwntp2.bluewin.ch)
const char* ntpServerName = "ch.pool.ntp.org";
const int timeZone = 3;     // Central European Time (summer time)

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

void setup(void) {


  Serial.begin(115200);

  Serial.print("Connecting to wifi ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

  Serial.print("Using NTP Server ");
  Serial.println(ntpServerName);
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServer);
  Serial.print("NTP Server IP ");
  Serial.println(timeServer);

  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(60);

  /*
    time_t t = getNtpTime();
    setTime(t);
  */

  /*
    time_t t = getNtpTime();
    setTime(t);
  */



}



/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void loop() {
  digitalClockDisplay();
 
}
void digitalClockDisplay()  {

Serial.print(year());
Serial.print("-");
Serial.print(month());
Serial.print("-");
Serial.print(day());
Serial.print(" ");
Serial.print(hour());
Serial.print(":");
Serial.print(minute());
Serial.print(":");
Serial.print(second());
Serial.println(" ");


delay(1000);


}






/*-------- Serial Debug Code ----------*/