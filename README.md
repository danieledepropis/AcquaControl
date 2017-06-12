# AcquaControl
Arduino software AcquaControl 

Autore: Daniele De Propis

Note per la compilazione:
   Nella cartella C:\Program Files (x86)\Arduino\libraries cancellare la cartella RobotIRremote
   Dal menu Sketch->#includi libreria->Gestione Librerie installare le seuenti Librerie:
     - DS1307RTC
     - OneWire
     - Time
     - IRremote
     
     copiare il file time.h dalla cartella Documenti\Arduino\libraries\Time nella cartella Documenti\Arduino\libraries\DS1307RTC
 
 Mappatura dei pin digitali:
    Comunicazione seriale con Raspberry
      D0:  RX
      D1:  TX
      D2:  EN - Display (clockpin)
      D3:  BL - Display           
      D4:  Sensore IR per telecomando
      D5:  Relè - Reattore Kalkwasser
      D6:  Relè - Corrente generale  
      D7:  RS - Display (latchpin)   
      D8:  DS18S20 - Sonda della temperatura
      D9:  Relè - Pompa di rabbocco         
      D10: RW - Display (datapin)           
      D11: Relè - Luci acquario             
      D12: Relè - Presa Riscaldatore        
      D13: Rele - Ventole raffreddamento    
 Mappatura dei pin analogici:
      A0: Galleggiante 1 - Livello minimo acqua in sump ---------(ARANCIO)
      A1: Galleggiante 2 - Livello acqua in sump (rabbocco) -----(BLU)
      A2: Galleggiante 3 - Livello minimo vasca di rabocco ------(MARRONE)
      A3: Sonda PH
      A4/SDA: RTC orologio
      A5/SCL: RTC orologio
 
  - Galleggiante 1: Allarme acqua sump
  - Galleggiante 2: Avvio pompa di rabocco
  - Galleggiante 3: Allarme acqua rabocco esaurita
 
 Comandi inviabili ad Arduino:
    Messaggio per sincronizzare l'orologio di Arduino (21 caratteri)
      Tdd/mm/yyyy|hh:nn:ss:#
      C(int)#    Calibratura sonda PH
      A(float)#  Temperatura acquario
      V(float)#  Temperatura ventole raffreddamento
      G#         Richiesta Info
      S#         Richiesta Info startup
      KI(int)    0 <= (int) <= 24 Intervallo di attivazione miscelazione kalkwassere
      KD(int)    0 <= (int) <= 60 Durata in minuti miscelazione Kalkwasser
      LG(int)    0 <= (int) <= 2359 Ora di inizio Giorno (int=h*100+m)
      LN(int)    0 <= (int) <= 2359 Ora di inizio Notte  (int=h*100+m)
 
 Messaggi inviati da Arduino e comandi tramite telecomando:
 -Tasto On/off : Display On/off
 -Tasto Mute: Reset Arduino
 -Tasto Zoom: View parametri On/off
 -Tasto 1 telecomando: AcquaControlRdd;mm;yyyy;hh;mm;ss;#     Data e Ora Inserita acqua di rabbocco
 -Tasto 2 telecomando: AcquaControlPdd;mm;yyyy;hh;mm;ss;#     
 -Tasto 3 telecomando: AcquaControlOdd;mm;yyyy;hh;mm;ss;#     Data e Ora Prodotta acqua osmosi
 -Tasto 4 telecomando: AcquaControlHdd;mm;yyyy;hh;mm;ss;#     
 -Tasto 5 telecomando: AcquaControlRFdd;mm;yyyy;hh;mm;ss;#     
 -Tasto 6 telecomando: AcquaControlCAdd;mm;yyyy;hh;mm;ss;#     
 -Tasto 7 telecomando: AcquaControlRSdd;mm;yyyy;hh;mm;ss;#     
 -Tasto 8 telecomando: AcquaControlCCSdd;mm;yyyy;hh;mm;ss;#     
 -Tasto 9 telecomando: AcquaControlMSdd;mm;yyyy;hh;mm;ss;#     
 
 AcquaControlOndd;mm;yyyy;hh;mm;ss;#    Data e Ora di avvio di Arduino in risposta alla richiesta S#
  
 AcquaControlInfo:(Versione);dd;mm;yyyy;hh;mm;ss;(temperatura);(Ph: Valore rilevato dalla sonda);(Ph con compensazione della temperatura);StatoRelèCorrente;StatoRelèRabocco;StatoRelèKalkwasser;StatoRelèLuci;StatoRelèRiscaldatore;StatoRelèVentole;StatoGalleggiante1;StatoGalleggiante3;offsetCalibratura;temperaturaAcquario;temperaturaVentole;intervalloKalkwasser;durataKalkwasser;oraInizioGiorno;oraInizioNotte;#
 
 
