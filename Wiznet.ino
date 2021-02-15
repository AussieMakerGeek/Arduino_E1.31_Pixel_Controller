// E1.31 Receiver and pixel controller
// This code may be freely distributed and used as you see fit for non-profit
// purposes and as long as the original author is credited and it remains open
// source
//
// Please configure your Lighting product to use Unicast to the IP the device is given from your DHCP server
// Multicast is not currently supported due to bandwidth/processor limitations

// You will need the Ethercard and FastLed Libraries from:
// https://github.com/FastLED/FastLED/releases
//
// The Atmega328 only has 2k of SRAM.  This is a severe limitation to being able to control large
// numbers of smart pixels due to the pixel data needing to be stored in an array as well as
// a reserved buffer for receiving Ethernet packets.  This code will allow you to use a maximum of 240 pixels
// as that just about maxes out the SRAM on the Atmega328.

// There is deliberately no serial based reporting from the code to conserve SRAM.  There is a **little**
// bit available if you need to add some in for debugging but keep it to an absolute minimum for debugging
// only.

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "FastLED.h"

//*********************************************************************************

// enter desired universe and subnet  (sACN first universe is 1)
#define DMX_SUBNET 0
#define DMX_UNIVERSE 11 //**Start** universe

// Set a different MAC address for each...
byte mac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x11 };

// Uncomment if you want to use static IP
//*******************************************************
// ethernet interface ip address
IPAddress ip(10, 0, 0, 11);  //IP address of ethernet shield
//*******************************************************

EthernetUDP Udp;

// By sacrificing some of the Ethernet receive buffer, we can allocate more to the LED array
// but this is **technically** slower because 2 packets must be processed for all 240 pixels.

/// DONT CHANGE unless you know the consequences...
 #define ETHERNET_BUFFER 540 
 #define CHANNEL_COUNT 360 //because it divides by 3 nicely
 #define NUM_LEDS 240 // can not go higher than this - Runs out of SRAM
 #define UNIVERSE_COUNT 2
 #define LEDS_PER_UNIVERSE 120

// The pin the data line is connected to for WS2812b
#define DATA_PIN 7

//********************************************************************************

// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned char packetBuffer[ETHERNET_BUFFER];

void setup() {
  // Using different LEDs or colour order? Change here...
  // ********************************************************
     FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  
  // ********************************************************

 
  // ********************************************************  
  Ethernet.begin(mac,ip);
  Udp.begin(5568);
  // ******************************************************** 

  //Once the Ethernet is initialised, run a test on the LEDs
  initTest();
}

void loop() {
   //Process packets
   int packetSize = Udp.parsePacket(); //Read UDP packet count
   if(packetSize){
    Udp.read(packetBuffer,ETHERNET_BUFFER); //read UDP packet
    int count = checkACNHeaders(packetBuffer, packetSize);
    if (count) {
      sacnDMXReceived(packetBuffer, count); //process data function
    }
  }
}


void sacnDMXReceived(unsigned char* pbuff, int count) {
  if (count > CHANNEL_COUNT) count = CHANNEL_COUNT;
  byte b = pbuff[113]; //DMX Subnet
  if ( b == DMX_SUBNET) {
    b = pbuff[114];  //DMX Universe
    if ( b >= DMX_UNIVERSE && b <= DMX_UNIVERSE + UNIVERSE_COUNT ) {  
      if ( pbuff[125] == 0 ) {  //start code must be 0
      int ledNumber = (b - DMX_UNIVERSE) * LEDS_PER_UNIVERSE;
       // sACN packets come in seperate RGB but we have to set each led's RGB value together
       // this 'reads ahead' for all 3 colours before moving to the next led.
       //Serial.println("*");
       for (int i = 126;i < 126+count;i = i + 3){
          byte charValueR = pbuff[i];
          byte charValueG = pbuff[i+1];
          byte charValueB = pbuff[i+2];
          leds[ledNumber].setRGB(charValueR,charValueG,charValueB);
          ledNumber++;
        }
      FastLED.show();  //Do it!
      }
    }
  }
}

int checkACNHeaders(unsigned char* messagein, int messagelength) {
  //Do some VERY basic checks to see if it's an E1.31 packet.
  //Bytes 4 to 12 of an E1.31 Packet contain "ACN-E1.17"
  //Only checking for the A and the 7 in the right places as well as 0x10 as the header.
  //Technically this is outside of spec and could cause problems but its enough checks for us
  //to determine if the packet should be tossed or used.
  //This improves the speed of packet processing as well as reducing the memory overhead.
  //On an Isolated network this should never be a problem....
  if ( messagein[1] == 0x10 && messagein[4] == 0x41 && messagein[12] == 0x37) {	
      int addresscount = (byte) messagein[123] * 256 + (byte) messagein[124]; // number of values plus start code
      return addresscount -1; //Return how many values are in the packet.
    }
  return 0;
}

void initTest() //runs at board boot to make sure pixels are working
{
  LEDS.showColor(CRGB(255, 0, 0)); //turn all pixels on red
   delay(1000);
   LEDS.showColor(CRGB(0, 255, 0)); //turn all pixels on green
   delay(1000);
   LEDS.showColor(CRGB(0, 0, 255)); //turn all pixels on blue
   delay(1000);
   LEDS.showColor(CRGB(0, 0, 0)); //turn all pixels off
}