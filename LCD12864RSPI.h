//LCD12864 spi
//www.dfrobot.com

#ifndef LCD12864RSPI_h
  #define LCD12864RSPI_h
  //#include <avr/pgmspace.h>
  //#include <inttypes.h>

  class LCD12864RSPI {
    typedef unsigned char uchar;

  public:

  LCD12864RSPI();

  void Initialise(void);
  void delayns(void);

  void WriteByte(int dat);
  void WriteCommand(int CMD);
  void WriteData(int CMD);

  void Clear(void);
  void StandBy();
  void DisplayString(int X,int Y,uchar *ptr,int dat);

  static const int clockPin = 2;  //SCK(EN)
  static const int latchPin = 7;  //CS(RS)
  static const int dataPin = 10;   //SID(RW)
  };
  extern LCD12864RSPI LCDA;    
#endif
