//#if ARDUINO >= 100
  #include <Arduino.h> 
//#else
//  #include <WProgram.h> 
//#endif

#include "LCD12864RSPI.h"
#include "DFrobot_font8x8.h"

extern "C" 
{ 
  //#include <inttypes.h>
  //#include <stdio.h>  //not needed yet
  //#include <string.h> //needed for strlen()
  //#include <avr/pgmspace.h>
  //S#include <wiring.h>
}

LCD12864RSPI::LCD12864RSPI() { } 

void LCD12864RSPI::delayns(void)
{   
  delayMicroseconds(80);
}

void LCD12864RSPI::WriteByte(int dat)
{
  digitalWrite(latchPin, HIGH);
  delayns();
  shiftOut(dataPin, clockPin, MSBFIRST, dat);
  digitalWrite(latchPin, LOW);
}

void LCD12864RSPI::WriteCommand(int CMD)
{
  int H_data,L_data;
  H_data = CMD;
  H_data &= 0xf0;           
  L_data = CMD;             
  L_data &= 0x0f;          
  L_data <<= 4;            
  WriteByte(0xf8);         
  WriteByte(H_data);
  WriteByte(L_data);
}

void LCD12864RSPI::WriteData(int CMD)
{
  int H_data,L_data;
  H_data = CMD;
  H_data &= 0xf0;           
  L_data = CMD;            
  L_data &= 0x0f;           
  L_data <<= 4;             
  WriteByte(0xfa);          
  WriteByte(H_data);
  WriteByte(L_data);
}

void LCD12864RSPI::Initialise()
{
  pinMode(latchPin, OUTPUT);     
  pinMode(clockPin, OUTPUT);    
  pinMode(dataPin, OUTPUT);
  digitalWrite(latchPin, LOW);
  delayns();

  WriteCommand(0x30);   //Function Set: 110000 - DL=1 8-bit interface; RE=0 basic istruction    
  WriteCommand(0x0c);   //Display control: 1100 - D=1 display on; c=0 cursor off; b=0 character blink off
  WriteCommand(0x01);   //Display clear
  WriteCommand(0x06);   //Entry mode set: 110
}

void LCD12864RSPI::Clear(void)
{  
  WriteCommand(0x30);
  WriteCommand(0x01);
  delayns();
}

void LCD12864RSPI::StandBy()
{
  WriteCommand(0x34);		//Function Set: 110100 - DL=1 8-bit interface; RE=1 extended istruction 
  WriteCommand(0x01);		//Enter standby mode, any other istruction can terminate
  WriteCommand(0x30);		//Function Set: 110000 - DL=1 8-bit interface; RE=0 basic istruction
}

void LCD12864RSPI::DisplayString(int X,int Y,uchar *ptr,int dat)
{
  int i;

  switch(X)
  {
    case 0:  
      Y|=0x80;break;
    case 1:  
      Y|=0x90;break;
    case 2:  
      Y|=0x88;break;
    case 3:  
      Y|=0x98;break;
    default: 
      break;
  }

  WriteCommand(Y); 

  for(i=0;i<dat;i++)
  { 
    WriteData(ptr[i]);
  }
}

LCD12864RSPI LCDA = LCD12864RSPI();
