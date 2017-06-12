#include "Arduino.h"
#include "clsReadWriteSetting.h"
#include "clsPH.h"

clsPH::clsPH() {
  intNumeroLetturePH=0;
  fPH=-1;
  fTemp=-1;
}

void clsPH::setTemperatura(float fTemperatura) {
  fTemp=fTemperatura;
}

float clsPH::readPH() {
  float valPH=0;
  static float fPHNew=0;
  
  valPH=analogRead(analogPin_PH);
  
  if (valPH>0) {
    if (abs(fPH-valPH)<2 || fPH<0) {
      intNumeroLetturePH=0;
      fPH=valPH;
    }
    else
    {
      if (abs(fPH-valPH)<abs(fPH-fPHNew) || intNumeroLetturePH==0) {
        fPHNew=valPH;
      }
      
      intNumeroLetturePH++;
      if (intNumeroLetturePH>5000) {
        intNumeroLetturePH=0;
        fPH=fPHNew;
      }
    }
  }
  
  return fPH;
}

float clsPH::getPH() {
  //Calcolo del ph con compensazione della temperatura
  //pH = 7 - (2.5 - SensorValue / 200) / (0.257179 + 0.000941468 * Temperature)
  clsReadWriteSetting readWriteSetting=clsReadWriteSetting();
  float valPH=0;
    
  //valPH=getPHMedio(intNumeroLetturePH)-offsetCalibraturaPH;
  //valTemperatura=getTemperaturaMedia(intNumeroLettureTemp);
  valPH=fPH-readWriteSetting.readOffsetCalibraturaPH();
     
  return 7 - (2.5 - valPH / 200) / (0.257179 + 0.000941468 * fTemp);
}

int clsPH::getNumeroLetturePH() {
  return intNumeroLetturePH;
}
