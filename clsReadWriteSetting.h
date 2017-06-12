#define VERSIONE "VER 1.0"

class clsReadWriteSetting {
  
public:
  clsReadWriteSetting();
  boolean checkVersione();
  void writeVersione();
  float readOffsetCalibraturaPH();
  void writeOffsetCalibraturaPH(int offsetCalibraturaPH);
  float readTemperaturaAcquario();
  void writeTemperaturaAcquario(float fTemperaturaAcquario);
  float readTemperaturaVentole();
  void writeTemperaturaVentole(float fTemperaturaVentole);
  int readIntervalloKalkwasser();
  void writeIntervalloKalkwasser(int intIntervalloKalkwasser);
  int readDurataKalkwasser();
  void writeDurataKalkwasser(int intDurataKalkwasser);
  int readOraInizioGiorno();
  void writeOraInizioGiorno(int intOraInizioGiorno);
  int readOraFineGiorno();
  void writeOraFineGiorno(int intOraFineGiorno);
};
