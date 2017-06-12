/* Note per la compilazione:
 *  Nella cartella C:\Program Files (x86)\Arduino\libraries cancellare la cartella RobotIRremote
 *  Dal menu Sketch->#includi libreria->Gestione Librerie installare le seuenti Librerie:
 *    - DS1307RTC
 *    - OneWire
 *    - Time
 *    - IRremote
 *    
 *    copiare il file time.h dalla cartella Documenti\Arduino\libraries\Time nella cartella Documenti\Arduino\libraries\DS1307RTC
 *    
 ***********************************************************************************    
 *************************** Mappatura dei pin digitali ****************************
 ***********************************************************************************
 **** Comunicazione seriale con Raspberry                                       ****
 ****   D0:  RX                                                                 ****
 ****   D1:  TX                                                                 ****
 ***********************************************************************************
 ****   D2:  EN - Display (clockpin)                                            ****
 ****   D3:  BL - Display                                                       ****
 ****   D4:  Sensore IR per telecomando                                         ****
 ****   D5:  Relè - Reattore Kalkwasser                                         ****
 ****   D6:  Relè - Corrente generale                                           ****
 ****   D7:  RS - Display (latchpin)                                            ****
 ****   D8:  DS18S20 - Sonda della temperatura                                  ****
 ****   D9:  Relè - Pompa di rabbocco                                           ****
 ****   D10: RW - Display (datapin)                                             ****
 ****   D11: Relè - Luci acquario                                               ****
 ****   D12: Relè - Presa Riscaldatore                                          ****
 ****   D13: Rele - Ventole raffreddamento                                      ****
 ****                                                                           ****
 ****   A0: Galleggiante 1 - Livello minimo acqua in sump ---------(ARANCIO)    ****
 ****   A1: Galleggiante 2 - Livello acqua in sump (rabbocco) -----(BLU)        ****
 ****   A2: Galleggiante 3 - Livello minimo vasca di rabocco ------(MARRONE)    ****
 ****   A3: Sonda PH                                                            ****
 ****   A4/SDA: RTC orologio                                                    ****
 ****   A5/SCL: RTC orologio                                                    ****
 ***********************************************************************************
 *
 *  Galleggiante 1: Allarme acqua sump
 *  Galleggiante 2: Avvio pompa di rabocco
 *  Galleggiante 3: Allarme acqua rabocco esaurita
 *
 * Comandi inviabili ad Arduino:
 * Mseeggio per sincronizzare orologio di Arduino (21 caratteri)
 * Tdd/mm/yyyy|hh:nn:ss:#
 * C(int)#    Calibratura sonda PH
 * A(float)#  Temperatura acquario
 * V(float)#  Temperatura ventole raffreddamento
 * G#         Richiesta Info
 * S#         Richiesta Info startup
 * KI(int)    0 <= (int) <= 24 Intervallo di attivazione miscelazione kalkwassere
 * KD(int)    0 <= (int) <= 60 Durata in minuti miscelazione Kalkwasser
 * LG(int)    0 <= (int) <= 2359 Ora di inizio Giorno (int=h*100+m)
 * LN(int)    0 <= (int) <= 2359 Ora di inizio Notte  (int=h*100+m)
 * 
 * Messaggi inviati da Arduino:
 * Tasto On/off : Display On/off
 * Tasto Mute: Reset Arduino
 * Tasto Zoom: View parametri On/off
 * Tasto 1 telecomando: AcquaControlRdd;mm;yyyy;hh;mm;ss;#     Data e Ora Inserita acqua di rabbocco
 * Tasto 2 telecomando: AcquaControlPdd;mm;yyyy;hh;mm;ss;#     
 * Tasto 3 telecomando: AcquaControlOdd;mm;yyyy;hh;mm;ss;#     Data e Ora Prodotta acqua osmosi
 * Tasto 4 telecomando: AcquaControlHdd;mm;yyyy;hh;mm;ss;#     
 * Tasto 5 telecomando: AcquaControlRFdd;mm;yyyy;hh;mm;ss;#     
 * Tasto 6 telecomando: AcquaControlCAdd;mm;yyyy;hh;mm;ss;#     
 * Tasto 7 telecomando: AcquaControlRSdd;mm;yyyy;hh;mm;ss;#     
 * Tasto 8 telecomando: AcquaControlCCSdd;mm;yyyy;hh;mm;ss;#     
 * Tasto 9 telecomando: AcquaControlMSdd;mm;yyyy;hh;mm;ss;#     
 * 
 * AcquaControlOndd;mm;yyyy;hh;mm;ss;#    Data e Ora di avvio di Arduino in risposta alla richiesta S#
 * 
 * AcquaControlInfo:(Versione);dd;mm;yyyy;hh;mm;ss;(temperatura);(Ph: Valore rilevato dalla sonda);(Ph con compensazione della temperatura);
 * StatoRelèCorrente;StatoRelèRabocco;StatoRelèKalkwasser;StatoRelèLuci;StatoRelèRiscaldatore;StatoRelèVentole;StatoGalleggiante1;StatoGalleggiante3;
 * offsetCalibratura;temperaturaAcquario;temperaturaVentole;intervalloKalkwasser;durataKalkwasser;oraInizioGiorno;oraInizioNotte;#
 * 
*/

#include "LCD12864RSPI.h"
//#include <Time.h>  
//#include <Wire.h>  
#include <DS1307RTC.h>
#include <IRremote.h>
#include <OneWire.h>
//#include <TimeAlarms.h>
#include <EEPROM.h>

#include "clsReadWriteSetting.h"
#include "clsDebounce.h"
#include "clsRele.h"
#include "clsTemperatura.h"
#include "clsPH.h"

/*  code to process time sync messages from the serial port   */
#define TIME_MSG_LEN  21   // time sync to PC is HEADER followed by unix time_t as ten ascii digits

clsReadWriteSetting readWriteSetting=clsReadWriteSetting();

String inputString = "";         // a string to hold incoming data

static const int blPin = 3;       //Display (BL)
static const int RECV_PIN = 4;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

clsDebounce clsGalleggiante1=clsDebounce(A0);  //A0 usato come pin digitale - Livello minimo acqua in sump
clsDebounce clsGalleggiante2=clsDebounce(A1);  //A1 usato come pin digitale - Livello acqua in sump (rabbocco)
clsDebounce clsGalleggiante3=clsDebounce(A2);  //A2 usato come pin digitale - Livello minimo vasca di rabocco

clsRele clsReleKalkwasser=clsRele(5);
clsRele clsReleCorrente=clsRele(6);
clsRele clsReleRabbocco=clsRele(9);
clsRele clsReleLuci=clsRele(11);
clsRele clsReleRiscaldatore=clsRele(12);
clsRele clsReleVentole=clsRele(13);

clsTemperatura clsTemp=clsTemperatura();
clsPH objPH=clsPH();

IRrecv irrecv(RECV_PIN);
decode_results results;

boolean bDisplayOnOff=false;
unsigned long lngArduinoStandBy=0;
unsigned long lngDisplayRefresh=0;
unsigned long lngDisplayInfo=0;

int intFunzioneMenu=0;

String strRigaDisplay2 = "";
String strRigaDisplay3 = "";

time_t tStart;

/*
// freRam occupa uno spazio di memori di 332 byte
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
*/

void ArduinoStandBy()
{
  if ((millis()-lngArduinoStandBy) > 300000)
  {  //trascorsi 5 minuti dall'ultima attività metto in stand by il display
    DisplayOff();
  }
}

void DisplayOn() {
  digitalWrite(blPin, HIGH);
  lngArduinoStandBy=millis();
  bDisplayOnOff=true;
}

void DisplayOff() {
  digitalWrite(blPin, LOW);
  bDisplayOnOff=false;
  intFunzioneMenu=0;
}

void DisplayOnOff() {
  if (bDisplayOnOff) {
    DisplayOff();

  } else {
    DisplayOn();
    //digitalWrite(blPin, HIGH);
    //lngArduinoStandBy=millis();
  }  
  //bDisplayOnOff=!bDisplayOnOff;
}

time_t processSyncMessage() {
  // return the time if a valid sync message is received on the serial port.
  // formato
  // Tdd/mm/yyyy|hh:nn:ss:
  tmElements_t tm;
  
  if (inputString.length() >=  TIME_MSG_LEN )  // time message consists of a header and ten ascii digits
  {
    tm.Day=inputString.substring(1,3).toInt();
    tm.Month=inputString.substring(4,6).toInt();
    tm.Year=inputString.substring(7,11).toInt()-1970;
    tm.Hour=inputString.substring(12,14).toInt();
    tm.Minute=inputString.substring(15,17).toInt();
    tm.Second=inputString.substring(18,20).toInt();
    
    return makeTime(tm); 
  }
  return 0;
}

void inviaDatiConsole(){
  Serial.print("AcquaControlInfo:");
  Serial.print(VERSIONE);
  Serial.print(";");
  Serial.print(day());
  Serial.print(";");
  Serial.print(month());
  Serial.print(";");
  Serial.print(year());
  Serial.print(";");
  Serial.print(hour());
  Serial.print(";");
  Serial.print(minute());
  Serial.print(";");
  Serial.print(second());
  Serial.print(";");
  Serial.print(clsTemp.getTemperatura());
  Serial.print(";");
  Serial.print(objPH.getPH());
  Serial.print(";");
  Serial.print(objPH.readPH());
  Serial.print(";");
  Serial.print(clsReleCorrente.getStatoRele());
  Serial.print(";");
  Serial.print(clsReleRabbocco.getStatoRele());
  Serial.print(";");
  Serial.print(clsReleKalkwasser.getStatoRele());
  Serial.print(";");
  Serial.print(clsReleLuci.getStatoRele());
  Serial.print(";");
  Serial.print(clsReleRiscaldatore.getStatoRele());
  Serial.print(";");
  Serial.print(clsReleVentole.getStatoRele());
  Serial.print(";");
  Serial.print(clsGalleggiante1.getButtonState());
  Serial.print(";");
  Serial.print(clsGalleggiante3.getButtonState());
  Serial.print(";");
  Serial.print(readWriteSetting.readOffsetCalibraturaPH());
  Serial.print(";");
  Serial.print(readWriteSetting.readTemperaturaAcquario());
  Serial.print(";");
  Serial.print(readWriteSetting.readTemperaturaVentole());
  Serial.print(";");
  Serial.print(readWriteSetting.readIntervalloKalkwasser());
  Serial.print(";");
  Serial.print(readWriteSetting.readDurataKalkwasser());
  Serial.print(";");
  Serial.print(readWriteSetting.readOraInizioGiorno());
  Serial.print(";");
  Serial.print(readWriteSetting.readOraFineGiorno());
  /*Serial.print(";");
  Serial.print("freeRam:");
  Serial.println(freeRam());*/
  Serial.print(";#");
}

void inviaArduinoInfoTime(String strMsg){
  time_t tTime;
  if (strMsg=="On:") {
    tTime=tStart;
  } else {
    tTime=now();
    DisplayOk();
  }
  Serial.print("AcquaControl");
  Serial.print(strMsg);
  Serial.print(day(tTime));
  Serial.print(";");
  Serial.print(month(tTime));
  Serial.print(";");
  Serial.print(year(tTime));
  Serial.print(";");
  Serial.print(hour(tTime));
  Serial.print(";");
  Serial.print(minute(tTime));
  Serial.print(";");
  Serial.print(second(tTime));
  Serial.print(";#");
}

void digitalClockDisplay(){
  // digital clock display of the time
  char buf[]="                 ";
  unsigned char bufferdisplay[16];
  int result = 0;
   
  result = snprintf(buf, 17, "%02d/%02d/%04d %02d:%02d", day(), month(), year(), hour(), minute());
  memcpy(bufferdisplay,buf,16);
  LCDA.DisplayString(0,0,bufferdisplay,16);
}

void digitalTemperaturaDisplay(){
  // digital clock display of the time
  char buf[10];
  unsigned char bufferdisplay[]="xx.xx^C";
  
  //bufferdisplay[5]=0x09;
  dtostrf(clsTemp.getTemperatura(), 2, 2, buf);
  memcpy(bufferdisplay,buf,5);
  LCDA.DisplayString(3,0,bufferdisplay,7);
}

void digitalPHDisplay(){
  // digital clock display of the time
  char buf[10];
  unsigned char bufferdisplay[]="  PH    ";
  
  dtostrf(objPH.getPH(), 1, 2, buf);
  LCDA.DisplayString(3,4,bufferdisplay,8);
  memcpy(bufferdisplay,buf,4);
  LCDA.DisplayString(3,6,bufferdisplay,4);
}

void digitalRigaDisplay(){
  unsigned char bufferdisplay[]="                ";
  
  for (int i=0;i<16 && i<strRigaDisplay2.length();i++) {
    bufferdisplay[i]=strRigaDisplay2.charAt(i);
  }
  LCDA.DisplayString(1,0,bufferdisplay,16);
  for (int i=0;i<16;i++) {
    if (i<strRigaDisplay3.length()) {
      bufferdisplay[i]=strRigaDisplay3.charAt(i);
    } else {
      bufferdisplay[i]=' ';
    }
  }
  LCDA.DisplayString(2,0,bufferdisplay,16);
}

void DisplayRefresh(int intMillisecondi) {
  if ((millis()-lngDisplayRefresh) > intMillisecondi) {
    lngDisplayRefresh=millis();
    digitalClockDisplay(); 
    digitalTemperaturaDisplay();  
    digitalPHDisplay();
    digitalRigaDisplay();
  }
}

void DisplayOk(){
  unsigned char bufferdisplay[]="*";
  
  DisplayOn();
  lngDisplayRefresh=millis();
  LCDA.DisplayString(3,4,bufferdisplay,1);
}

void DisplayInfo() {
  static int intNumInfo=0;
  char versione[]=VERSIONE;
  char buf[10];
  int result = 0;
  
  if (lngDisplayInfo==0) lngDisplayInfo=millis();
  if (millis()-lngDisplayInfo>3000) {
    lngDisplayInfo=millis();
    intNumInfo++;
  }
  
  switch (intNumInfo) {
    case 0:
      strRigaDisplay2="Versione";
      strRigaDisplay3="        ";
      for (int i=0;i<7;i++) {
        strRigaDisplay3.setCharAt(i,versione[i]);
      }
      break;
    case 1:
      strRigaDisplay2="Offset Calibr.";
      strRigaDisplay3="        ";
      dtostrf(readWriteSetting.readOffsetCalibraturaPH(), 1, 2, buf);
      for (int i=0;i<5;i++) {
        strRigaDisplay3.setCharAt(i,buf[i]);
      }
      break;
    case 2:
      strRigaDisplay2="Temp. acquario";
      strRigaDisplay3="       ^C";
      dtostrf(readWriteSetting.readTemperaturaAcquario(), 2, 2, buf);
      for (int i=0;i<5;i++) {
        strRigaDisplay3.setCharAt(i,buf[i]);
      }
      break;
    case 3:        
      strRigaDisplay2="Temp. ventole";
      strRigaDisplay3="      ^C";
      dtostrf(readWriteSetting.readTemperaturaVentole(), 2, 2, buf);
      for (int i=0;i<5;i++) {
        strRigaDisplay3.setCharAt(i,buf[i]);
      }
      break;
    case 4:    
      strRigaDisplay2="Intervallo Kalk";
      strRigaDisplay3="   ore";
      result = snprintf(buf, 9, "%02d", readWriteSetting.readIntervalloKalkwasser());
      strRigaDisplay3.setCharAt(0,buf[0]);
      strRigaDisplay3.setCharAt(1,buf[1]);
      break;
    case 5:
      strRigaDisplay2="Durata Kalk";
      strRigaDisplay3="   minuti";
      result = snprintf(buf, 9, "%02d", readWriteSetting.readDurataKalkwasser());
      strRigaDisplay3.setCharAt(0,buf[0]);
      strRigaDisplay3.setCharAt(1,buf[1]);
      break;
    case 6:
    case 7:
      strRigaDisplay2=(intNumInfo==6)?"Inizio giorno":"Fine giorno";
      strRigaDisplay3="ore   :   ";
      dtostrf((intNumInfo==6)?(float)readWriteSetting.readOraInizioGiorno()/100:(float)readWriteSetting.readOraFineGiorno()/100, 2, 2, buf);
      for (int i=0;i<5;i++) {
        strRigaDisplay3.setCharAt(i+4,(buf[i]=='.'?':':buf[i]));
      }
      break;
    case 8:
      intFunzioneMenu=0;
      intNumInfo=0;
      lngDisplayInfo=0;
      break;
  }
  DisplayRefresh(0);
}

void setup()  {
  Serial.begin(9600);
  
  //setup pin
  pinMode(blPin, OUTPUT);
  
  clsGalleggiante1.setPinMode(INPUT);
  clsGalleggiante2.setPinMode(INPUT);
  clsGalleggiante3.setPinMode(INPUT);
  
  clsReleKalkwasser.setPinOutputMode();
  clsReleCorrente.setPinOutputMode();
  clsReleRabbocco.setPinOutputMode();
  clsReleLuci.setPinOutputMode();
  clsReleRiscaldatore.setPinOutputMode();
  clsReleVentole.setPinOutputMode();

  /*
  // cambiare la versione del software e decommentare questo blocco di codice per resettare i valori nell'EEPROM
  if (!readWriteSetting.checkVersione()) {
    readWriteSetting.writeVersione();
    readWriteSetting.writeOffsetCalibraturaPH(0);
    readWriteSetting.writeTemperaturaAcquario(25.00);
    readWriteSetting.writeTemperaturaVentole(26.00);
    readWriteSetting.writeIntervalloKalkwasser(8);
    readWriteSetting.writeDurataKalkwasser(15);
    readWriteSetting.writeOraInizioGiorno(1000);
    readWriteSetting.writeOraFineGiorno(2200);
  }*/
  
  // reserve 40 bytes for the inputString:
  inputString.reserve(40);
  
  setSyncProvider(RTC.get);   //the function to get the time from the RTC
  
  DisplayOnOff();
  
  LCDA.Initialise();
  delay(100);
  
  irrecv.enableIRIn(); // Start the receiver
  
  unsigned char bufferdisplay[]="AcquaControl    ";
  LCDA.DisplayString(0,1,bufferdisplay,12);
  memcpy(bufferdisplay," by",3);
  LCDA.DisplayString(1,3,bufferdisplay,3);
  memcpy(bufferdisplay,"Daniele DePropis",16);
  LCDA.DisplayString(2,0,bufferdisplay,16);
  memcpy(bufferdisplay,VERSIONE,strlen(VERSIONE));
  LCDA.DisplayString(3,4,bufferdisplay,strlen(VERSIONE));
  
  delay(3000);
  LCDA.Clear();
  
  if(timeStatus()!= timeSet) 
  {
    memcpy(bufferdisplay,"Set time NOK",12);    
    LCDA.DisplayString(0,1,bufferdisplay,12);
  }
  /*else
  {
    memcpy(bufferdisplay,"Set time OK",11);    
    LCDA.DisplayString(0,1,bufferdisplay,11);
  }*/
  
  delay(3000);
  LCDA.Clear();
  
  //Repeats();
  // create the alarms 
  //Serial.println(Alarm.alarmRepeat(8,30,0, MorningAlarm));  // 8:30am every day
  //Serial.println(Alarm.alarmRepeat(17,45,0,EveningAlarm));  // 5:45pm every day 
  //Serial.println(Alarm.alarmRepeat(dowSaturday,8,30,30,WeeklyAlarm));  // 8:30:30 every Saturday 
     
  //Serial.println(Alarm.timerRepeat(15, Repeats));            // timer for every 15 seconds    
  //Serial.println(Alarm.timerOnce(10, OnceOnly));             // called once after 10 seconds  
  
  //clsReleKalkwasser.setReleOn(HIGH);
  
  tStart=now();
  inviaArduinoInfoTime("On:");
}

void loop()
{
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0x1FE48B7:   //Tasto On/Off
        DisplayOnOff();
        break;
      case 0x1FE807F:   //Tasto MUTE
        resetFunc();  //call reset
        break;
      case 0x1FE20DF:    //Serial.println("Tasto 1");
        inviaArduinoInfoTime("R:");     
        break;
      case 0x1FEA05F:    //Serial.println("Tasto 2");
        inviaArduinoInfoTime("P:");
        break;
      case 0x1FE609F:    //Serial.println("Tasto 3");
        inviaArduinoInfoTime("O:");
        break;
      case 0x1FEE01F:    //Serial.println("Tasto 4");
        inviaArduinoInfoTime("H:");
        break;
      case 0x1FE10EF:    //Serial.println("Tasto 5");
        inviaArduinoInfoTime("RF:");
        break;
      case 0x1FE906F:    //Serial.println("Tasto 6");
        inviaArduinoInfoTime("CA:");
        break;
      case 0x1FE50AF:    //Serial.println("Tasto 7");
        inviaArduinoInfoTime("RS:");
        break;  
      case 0x1FED827:    //Serial.println("Tasto 8");
        inviaArduinoInfoTime("CCS:");
        break;
      case 0x1FEF807:    //Serial.println("Tasto 9");
        inviaArduinoInfoTime("MS:");
        break;
      case 0x1FE30CF:    //Tasto Zoom
        //View parametri on off
        if (bDisplayOnOff) {
          if (intFunzioneMenu==0) {
            intFunzioneMenu=2;
            lngDisplayInfo=millis();
          } /*else {
            if (intFunzioneMenu==2) {
              intFunzioneMenu=0;
            }
          }*/
        }
        lngArduinoStandBy=millis();
        break;
/*      case 0x1FE58A7:
        Serial.println("Vol +");
        break;
      case 0x1FE40BF:
        Serial.println("Vol -");
        break;
      case 0x1FEB04F:
        Serial.println("Tasto 0");
        break;
      case 0x1FE7887:
        Serial.println("CH +");
        break;
      case 0x1FEC03F:
        Serial.println("CH -");
        break;     
      case 0x1FE708F:
        Serial.println("Jump");
        break;     */
    }
    irrecv.resume(); // Receive the next value
  } 
  
  objPH.setTemperatura(clsTemp.readTemperatura());
  objPH.readPH();
  
  strRigaDisplay2="G:-- R:-- K:--";
  strRigaDisplay3="L:-- ^:-- *:--";
  
  //if (clsTemp.getTemperatura()>50) resetFunc();  //call reset
  
  if (clsTemp.getTemperatura()<readWriteSetting.readTemperaturaAcquario()) {
    clsReleRiscaldatore.setReleOn(HIGH);
  } else {
    clsReleRiscaldatore.setReleOn(LOW);
  }
  
  if (clsTemp.getTemperatura()>readWriteSetting.readTemperaturaVentole()) {
    clsReleVentole.setReleOn(HIGH);
  } else {
    clsReleVentole.setReleOn(LOW);
  }
  
  switch (intFunzioneMenu) {
/*    case 1: 
      strRigaDisplay2="Menu";
      strRigaDisplay3="";
      DisplayRefresh(0);
      break;*/
    case 2:
      DisplayInfo();
      break;
/*    case 3:
      DisplayOk();
      break;*/
  }
  
  if (clsGalleggiante2.getButtonState() == HIGH && clsGalleggiante3.getButtonState() == LOW) {
    //Avviare pompa di rabbocco
    clsReleRabbocco.ReleRabbocco(true);
  } else {
    clsReleRabbocco.ReleRabbocco(false);    
  }
  
  if (clsGalleggiante3.getButtonState() == HIGH) {
    //Allarme-Acqua di rabbocco esaurita
    //Serial.println("Allarme-Acqua di rabbocco esaurita!");
    strRigaDisplay2="Allarme:Acqua di"; 
    strRigaDisplay3="rabbocco insuff.";
    DisplayOn();
  }
  
  if (clsGalleggiante1.getButtonState() == HIGH) 
  {
    //Allarme acqua in sump insufficiente
    //Serial.println("Allarme acqua in sump insufficiente");
    strRigaDisplay2="Allarme:Livello"; 
    strRigaDisplay3="acqua in sump!";
    DisplayOn(); 
    
    clsReleCorrente.setReleOn(HIGH);
  } else {
    clsReleCorrente.setReleOn(LOW);
  }
  
  clsReleKalkwasser.ReleKalkwasser();
  clsReleLuci.ReleLuci();
  
  if (clsReleCorrente.getStatoRele()==LOW) strRigaDisplay2.replace("G:--","G:on");
  if (clsReleRabbocco.getStatoRele()==HIGH) strRigaDisplay2.replace("R:--","R:on");
  if (clsReleKalkwasser.getStatoRele()==HIGH) strRigaDisplay2.replace("K:--","K:on");
  if (clsReleLuci.getStatoRele()==HIGH) strRigaDisplay3.replace("L:--","L:on");
  if (clsReleRiscaldatore.getStatoRele()==HIGH) strRigaDisplay3.replace("^:--","^:on");
  if (clsReleVentole.getStatoRele()==HIGH) strRigaDisplay3.replace("*:--","*:on");
  
  DisplayRefresh(10000); 
  ArduinoStandBy();
  //Alarm.delay(0); // wait one second between clock display
  
  //inviaDatiConsole();
}

void serialEvent() {
  time_t t = 0;
  //Serial.println("serialEvent Start");
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    //Serial.print(inChar);
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '#') {
      //Serial.println("serialEvent Interpreto");
      switch (inputString.charAt(0)) {
        case 'T':    //Data e Ora   Tdd/mm/yyyy|hh:nn:ss:
          //Serial.println("Ricevuto T command");
          t = processSyncMessage();
          if (t>0)
          {
            RTC.set(t);   // set the RTC and the system time to the received value
            //setTime(t);
            
            resetFunc();  //call reset   
          }
          break;
        case 'C':    //Calibratura
          readWriteSetting.writeOffsetCalibraturaPH(inputString.substring(1).toInt());
          break;
        case 'A':    //Temperatura acquario    
          readWriteSetting.writeTemperaturaAcquario(inputString.substring(1).toFloat());
          break;
        case 'V':    //Temperatura ventole di raffreddamento
          readWriteSetting.writeTemperaturaVentole(inputString.substring(1).toFloat());
          break;
        case 'G':    //Richiesta info
          inviaDatiConsole();
          break;
        case 'S':    //Richiesta info startup
          inviaArduinoInfoTime("On:");
          break;
        case 'K':
          if (inputString.charAt(1)=='I') {
            if (inputString.substring(2).toInt()>0 && inputString.substring(2).toInt()<24) {
              readWriteSetting.writeIntervalloKalkwasser(inputString.substring(2).toInt());
            }
          }
          if (inputString.charAt(1)=='D') {
            if (inputString.substring(2).toInt()>0 && inputString.substring(2).toInt()<60) {
              readWriteSetting.writeDurataKalkwasser(inputString.substring(2).toInt());
            }
          }
          break;
        case 'L':
          if (inputString.charAt(1)=='G') {
            if (inputString.substring(2).toInt()>0 && inputString.substring(2).toInt()<2359) {
              readWriteSetting.writeOraInizioGiorno(inputString.substring(2).toInt());
            }
          }
          if (inputString.charAt(1)=='N') {
            if (inputString.substring(2).toInt()>0 && inputString.substring(2).toInt()<2359) {
              readWriteSetting.writeOraFineGiorno(inputString.substring(2).toInt());
            }
          }
          break;
      }
      
      inputString = "";
    }
  }
  //Serial.println("serialEvent End");
}
