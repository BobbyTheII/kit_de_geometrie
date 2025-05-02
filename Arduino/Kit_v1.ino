#include <SPI.h>
#include <MFRC522.h>
#include <MPR121.h>
#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>
#include <SFEMP3Shield.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

byte nuidPICC[4];

int trackList[2];

uint8_t result;

SFEMP3Shield MP3player;
SdFat sd;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),Play1 , LOW);
  attachInterrupt(digitalPinToInterrupt(3), Play2, LOW);

  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
  }

  MPR121.setInterruptPin(4);

  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US); 
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(1000);
  MPR121.autoSetElectrodes(); 
  digitalWrite(LED_BUILTIN, LOW);

  result = MP3player.begin();
  MP3player.setVolume(10, 10);
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
      
    if(rfid.uid.uidByte[1]==234){
      Serial.println("rectangle");
      trackList[0] = 2;
      trackList[1] = 3;
    }
    else if(rfid.uid.uidByte[1]==190){
      Serial.println("cube");
      trackList[0] = 0;
      trackList[1] = 1;
    }
    else if(rfid.uid.uidByte[1]==197){
      Serial.println("pyramide");
      trackList[0] = 4;
      trackList[1] = 5;
    }
  }
  Serial.println(trackList[0]);
  Serial.println(trackList[1]);
}

void Play1(){
  MPR121.updateAll();
  if (MPR121.isNewTouch(trackList[0])) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (MP3player.isPlaying()) {
      MP3player.stopTrack();
      MP3player.playTrack(trackList[0]);
    }
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void Play2(){
  MPR121.updateAll();
  if (MPR121.isNewTouch(trackList[1])) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (MP3player.isPlaying()) {
      MP3player.stopTrack();
      MP3player.playTrack(trackList[1]);
    }
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}
