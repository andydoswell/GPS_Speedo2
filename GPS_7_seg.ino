/*
Fast GPS LED Speedo
(c) 17 Aug 2023 A.G.Doswell

Released to all and sundry under GNU License.

The sketch uses a U-Blox 6 GPS satellite module connected to the hardware serial interface,


*/
#include <Arduino.h>
#include "TM1637.h"
#include <TinyGPS++.h>

TM1637 TM;


#define CLK 2
#define DIO 3
int SatVal;       // Number of satellites locked
int Speed;        // Speed in MPH
char SpeedA[6];   // Speed as char array
TinyGPSPlus gps;  // Feed gps data to TinySGPSPlus

void setup() {
  Serial.begin(9600);// start the comms with the GPS Rx
  // set 57,600 baud on u-blox
  Serial.write(0xB5);
  Serial.write(0x62);
  Serial.write(0x06);
  Serial.write(0x00);
  Serial.write(0x14);
  Serial.write(0x00);
  Serial.write(0x01);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0xD0);
  Serial.write(0x08);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0xE1);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x07);
  Serial.write(0x00);
  Serial.write(0x02);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0xDD);
  Serial.write(0xC3);
  Serial.write(0xB5);
  Serial.write(0x62);
  Serial.write(0x06);
  Serial.write(0x00);
  Serial.write(0x01);
  Serial.write(0x00);
  Serial.write(0x01);
  Serial.write(0x08);
  Serial.write(0x22);
  delay (100);
  Serial.end();// stop Serial coms at 9,600 baud
  delay (100);
  Serial.begin (57600); // restart GPS coms at 57,600 baud
  // send Serial to update u-blox rate to 200mS
  Serial.write(0xB5);
  Serial.write(0x62);
  Serial.write(0x06);
  Serial.write(0x08);
  Serial.write(0x06);
  Serial.write(0x00);
  Serial.write(0xC8);
  Serial.write(0x00);
  Serial.write(0x01);
  Serial.write(0x00);
  Serial.write(0x01);
  Serial.write(0x00);
  Serial.write(0xDE);
  Serial.write(0x6A);
  Serial.write(0xB5);
  Serial.write(0x62);
  Serial.write(0x06);
  Serial.write(0x08);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x0E);
  Serial.write(0x30);
  delay (100);
  TM.begin(CLK, DIO);
  TM.displayClear();
  TM.setBrightness (7); //brightness to max.
}

void ascii_to_7segment(char *buff, uint8_t *data) {
  for (int8_t i = 0, j = 5; j > -1 && i < 12 && buff[i]; i++) {
    if (isalpha(buff[i])) { buff[i] = tolower(buff[i]); }
    if (buff[i] >= '0' && buff[i] <= '9') {
      data[j--] = buff[i] - '0';
    } else if (buff[i] >= 'a' && buff[i] <= 'f') {
      data[j--] = buff[i] - 'a' + 10;
    } else if (buff[i] >= 'g' && buff[i] <= 'z') {
      data[j--] = buff[i] - 'g' + 18;
    } else if (buff[i] == '.') {
      data[j + 1] |= 0x80;  // decimal point on previous digit
    } else {
      data[j--] = 0x10;  // blank
    }
  }
}

void displayInfo() {  // Display the data
  SatVal = int(gps.satellites.value());
  if (SatVal < 4) {
    char buff[20];
    uint8_t data[10];
    strcpy(buff, "no loc");
    ascii_to_7segment(buff, data);
    TM.displayRaw(data, -1);
  }

  else {
    Speed = 10 * float(gps.speed.mph());
    if (Speed < 20) { Speed = 0; }
    char buff[20];
    uint8_t data[10];
    strcpy(buff, "  ");
    if (Speed < 100) {
      strcat(buff, "0");
    }
    itoa((Speed / 10), SpeedA, 10);
    strcat(buff, SpeedA);
    strcat(buff, ".");
    itoa((Speed % 10), SpeedA, 10);
    strcat(buff, SpeedA);
    //Serial.println(strlen(buff)); Serial.print (" ");
    //Serial.print(buff);
    strcat(buff, "  ");
    ascii_to_7segment(buff, data);
    TM.displayRaw(data, -1);
  }
}

void loop() {
  while (Serial.available() > 0) {  // While GPS message received
    if (gps.encode(Serial.read())) {
      displayInfo();
    }
  }
}
