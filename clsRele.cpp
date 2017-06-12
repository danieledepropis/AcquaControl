#include "Arduino.h"
#include <Time.h>
#include "clsRele.h"
#include "clsReadWriteSetting.h"

clsRele::clsRele(int relePin) {
  intRelePin=relePin; 
  ultimaAttivazione=0;
  intStatoRele=LOW;
}

void clsRele::setPinOutputMode() {
  pinMode(intRelePin, OUTPUT);  
}

void clsRele::setReleOn(int intReleOn) {
  if (intStatoRele!=intReleOn) {
    digitalWrite(intRelePin,intReleOn);
    ultimaAttivazione=millis();
    intStatoRele=intReleOn;
  }
}

void clsRele::ReleKalkwasser() {
  unsigned long lngIntervallo=0;  //Intervallo in ore
  unsigned long lngDurata=0;      //Durata in minuti
  clsReadWriteSetting readWriteSetting=clsReadWriteSetting();
  
  lngIntervallo=readWriteSetting.readIntervalloKalkwasser();
  lngDurata=readWriteSetting.readDurataKalkwasser();
  //lngDurata=lngDurata*60000;
  
  /*if (intStatoRele == HIGH) {
    if (abs(millis()-ultimaAttivazione)>lngDurata) {
      setReleOn(LOW);
    }
  } else {
    if (abs(millis()-ultimaAttivazione)>lngIntervallo) {
      setReleOn(HIGH);
    }
  }
  */
  if ((hour() % lngIntervallo)==0 && minute()<lngDurata) {
    setReleOn(HIGH);
  } else {
    setReleOn(LOW);
  }
}

void clsRele::ReleLuci() {
  clsReadWriteSetting readWriteSetting=clsReadWriteSetting();
  
  if (hour() * 100 + minute()>=readWriteSetting.readOraInizioGiorno() && hour() * 100 + minute()<=readWriteSetting.readOraFineGiorno()) {
    setReleOn(HIGH);
  } else {
    setReleOn(LOW);
  }
}

void clsRele::ReleRabbocco(boolean bRabbocco) {
  if (intStatoRele == HIGH) {
    if (abs(millis()-ultimaAttivazione)>300000 || bRabbocco==false) {
      setReleOn(LOW);
    } 
  } else {
    if (abs(millis()-ultimaAttivazione)>300000 && bRabbocco==true) {
      setReleOn(HIGH);
    }
  }
}

int clsRele::getStatoRele() {
  return intStatoRele;
}
