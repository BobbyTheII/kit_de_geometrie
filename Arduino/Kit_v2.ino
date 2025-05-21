#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <SdFat.h>
#include "SerialMP3Player.h"

#define SS_PIN 10
#define RST_PIN 9
#define TX 6
#define RX 5

SerialMP3Player mp3(RX,TX);

MFRC522 rfid(SS_PIN, RST_PIN);

byte nuidPICC[4];

int trackList[2]={8,8};

unsigned long lastInterruptTimePlay1 = 0;
unsigned long lastInterruptTimePlay2 = 0;
const unsigned long debounceDelay = 200;


void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),Play1 , LOW);
  attachInterrupt(digitalPinToInterrupt(3), Play2, LOW);

  mp3.begin(9600);       
  delay(500); 

  mp3.sendCommand(CMD_SEL_DEV, 0, 2);
  delay(500);
  mp3.playF(01);
  mp3.setVol(20);
  mp3.qVol();
}

void loop() {
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  if (rfid.uid.uidByte[0] != nuidPICC[0] ||  rfid.uid.uidByte[1] != nuidPICC[1] ||  rfid.uid.uidByte[2] != nuidPICC[2] ||  rfid.uid.uidByte[3] != nuidPICC[3] ) {
      for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = rfid.uid.uidByte[i];
      }
    if(rfid.uid.uidByte[1]==21){
      Serial.println("rectangle");
      trackList[0] = 04;
      trackList[1] = 05;
    }
    else if(rfid.uid.uidByte[1]==190){
      Serial.println("cube");
      trackList[0] = 01;
      trackList[1] = 03;
    }
    else if(rfid.uid.uidByte[1]==197){
      Serial.println("pyramide");
      trackList[0] = 06;
      trackList[1] = 07;
    }
    else if(rfid.uid.uidByte[1]==71){
      Serial.println("Cylindre");
      trackList[0]=9;
      trackList[1]=10;
    }
    else if(rfid.uid.uidByte[1]==47){
      Serial.println("Cone");
      trackList[0]=11;
      trackList[1]=02;
    }
  }
}

void Play1(){
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimePlay2 > debounceDelay) {
    lastInterruptTimePlay2 = currentTime;
  if (mp3.available()){
    Serial.println(mp3.decodeMP3Answer()); // print decoded answers from mp3
  }
    mp3.play(trackList[0]);
  }
}

void Play2(){
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimePlay2 > debounceDelay) {
    lastInterruptTimePlay2 = currentTime;
  if (mp3.available()){
    Serial.println(mp3.decodeMP3Answer()); // print decoded answers from mp3
  }
    mp3.play(trackList[1]);
  }
}
