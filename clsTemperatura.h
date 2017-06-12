static const int DS18S20_Pin = 8; //DS18S20 Signal pin on digital 8

class clsTemperatura {
  float fTemperatura;
  
  float readSensorTemp();
public:

clsTemperatura();
float readTemperatura();
float getTemperatura();
};
