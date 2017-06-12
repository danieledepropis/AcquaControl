static const int analogPin_PH = 3;

class clsPH {
  int intNumeroLetturePH;
  float fPH;
  float fTemp; 
public:

clsPH();
void setTemperatura(float fTemperatura);
float readPH();
float getPH();
int getNumeroLetturePH();
};
