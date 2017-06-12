#include "Arduino.h"
#include "clsReadWriteSetting.h"
#include <EEPROM.h>

clsReadWriteSetting::clsReadWriteSetting() {
  
}

boolean clsReadWriteSetting::checkVersione() {
  char versione[]=VERSIONE;
  boolean bVersioneOk;
  
  bVersioneOk=true;
  for (int i=0;i<7;i++) {
    bVersioneOk=bVersioneOk && EEPROM.read(i)==versione[i];
  }
  return bVersioneOk;
}

void clsReadWriteSetting::writeVersione() {
  char versione[]=VERSIONE;
  
  for (int i=0;i<7;i++) {
    EEPROM.write(i, versione[i]);  
  }
}

float clsReadWriteSetting::readOffsetCalibraturaPH() {
  float fHighValue;
  
  return float(EEPROM.read(7)*256+EEPROM.read(8))/10;
}

void clsReadWriteSetting::writeOffsetCalibraturaPH(int offsetCalibraturaPH) {
  int intHighValue;
  int intLowValue;
  
  intHighValue=offsetCalibraturaPH / 256;  
  intLowValue=offsetCalibraturaPH - intHighValue * 256;
  
  EEPROM.write(7, intHighValue);  
  EEPROM.write(8, intLowValue);
}

float clsReadWriteSetting::readTemperaturaAcquario() {
  char buf[5];
  
  for (int i=0;i<5;i++) {
    buf[i]=EEPROM.read(9+i);  
  }
  
  return atof(buf);
}

void clsReadWriteSetting::writeTemperaturaAcquario(float fTemperaturaAcquario) {
  char buf[10];
  
  dtostrf(fTemperaturaAcquario, 2, 2, buf);  
  
  for (int i=0;i<5;i++) {
    EEPROM.write(9+i, buf[i]);  
  }
}

float clsReadWriteSetting::readTemperaturaVentole() {
  char buf[5];
  
  for (int i=0;i<5;i++) {
    buf[i]=EEPROM.read(14+i);  
  }
  
  return atof(buf);
}

void clsReadWriteSetting::writeTemperaturaVentole(float fTemperaturaVentole) {
  char buf[10];
  
  dtostrf(fTemperaturaVentole, 2, 2, buf);  
  
  for (int i=0;i<5;i++) {
    EEPROM.write(14+i, buf[i]);  
  }
}

int clsReadWriteSetting::readIntervalloKalkwasser() {
  return EEPROM.read(19);  
}

void clsReadWriteSetting::writeIntervalloKalkwasser(int intIntervalloKalkwasser) {
  EEPROM.write(19, intIntervalloKalkwasser);
}

int clsReadWriteSetting::readDurataKalkwasser() {
  return EEPROM.read(20);
}

void clsReadWriteSetting::writeDurataKalkwasser(int intDurataKalkwasser) {
  EEPROM.write(20, intDurataKalkwasser);
}

int clsReadWriteSetting::readOraInizioGiorno() {
  return EEPROM.read(21) * 100 + EEPROM.read(22);  
}  

void clsReadWriteSetting::writeOraInizioGiorno(int intOraInizioGiorno) {
  int intHH;
  int intMM;
  
  intHH=intOraInizioGiorno/100;
  intMM=intOraInizioGiorno - intHH * 100;
  
  EEPROM.write(21, intHH);
  EEPROM.write(22, intMM);
}

int clsReadWriteSetting::readOraFineGiorno() {
  return EEPROM.read(23) * 100 + EEPROM.read(24);  
}

void clsReadWriteSetting::writeOraFineGiorno(int intOraFineGiorno) {
  int intHH;
  int intMM;
  
  intHH=intOraFineGiorno/100;
  intMM=intOraFineGiorno - intHH * 100;
  
  EEPROM.write(23, intHH);
  EEPROM.write(24, intMM);
}

