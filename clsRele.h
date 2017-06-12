class clsRele {
  int intRelePin;
  unsigned long ultimaAttivazione;
  int intStatoRele;
public:

clsRele(int relePin);
void setPinOutputMode();
void setReleOn(int intReleOn);
void ReleKalkwasser();
void ReleLuci();
void ReleRabbocco(boolean bRabbocco);
int getStatoRele();
};
