#include "Arduino.h"
#include <OneWire.h>
#include "clsReadWriteSetting.h"
#include "clsTemperatura.h"

//Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 8

clsTemperatura::clsTemperatura() {
  fTemperatura=-1;
}

float clsTemperatura::readSensorTemp() {
  //returns the temperature from one DS18S20 in DEG Celsius
  //In caso di esito negativo ritorna i seguenti codici di errore:
  //   -1   :no more sensors on chain, reset search
  //   -2   :CRC is not valid
  //   -3   :Device is not recognized
  
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      //Serial.println("CRC is not valid!");
      return -2;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      //Serial.print("Device is not recognized");
      return -3;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
}

float clsTemperatura::readTemperatura() {
  float valTemp=0;
  static int intNumeroLettureTemp=0;
  static float fTemperaturaNew=0;
  
  valTemp=readSensorTemp();
  
  if (valTemp>0) {
    if (abs(fTemperatura-valTemp)<0.05 || fTemperatura<0) {
      intNumeroLettureTemp=0;
      fTemperatura=valTemp;
    }
    else
    {
      if (abs(fTemperatura-valTemp)<abs(fTemperatura-fTemperaturaNew) || intNumeroLettureTemp==0) {
        fTemperaturaNew=valTemp;
      }
      
      intNumeroLettureTemp++;
      if (intNumeroLettureTemp>100) {
        intNumeroLettureTemp=0;
        
        fTemperatura=fTemperaturaNew;
      }
    }
  }
  
  return fTemperatura;
}

float clsTemperatura::getTemperatura() {
  if (fTemperatura==-1) {
    fTemperatura=readTemperatura();
  }
  return fTemperatura;
}


