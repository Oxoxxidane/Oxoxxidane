#include <Arduino.h>

/*==================================================================

|   外接设备   |      协议      |              接线                  |
--------------------------------------------------------------------
|    雨量计    | Modbus RTU 485 | Serial 2  RX - D16 TX - D17  4800 |
|    液位计    | analog in      |     ADC2 CH4~7 D13 D12 D14 D27    |
|     屏幕     | UART           |       Default Serial 0 USB        |
|   空气阀*N   | digital out    |                                   | 

===================================================================*/

/*=======雨量计======*/

uint16_t today = 0x0000;   //当天降雨量
uint16_t now = 0x0000;     //瞬时降雨量
uint16_t yestoday = 0x000; //昨日降雨量
uint16_t total = 0x00;     //总共降雨量
uint16_t hour = 0x00;      //小时降雨量
uint16_t lasthour = 0x00;  //上小时降雨量
uint16_t hour24max = 0x00; //24小时最大降雨量
uint16_t hour24min = 0x00; //24小时最小降雨量

/*========液位=======*/

uint16_t ch4;
uint16_t ch5;
uint16_t ch6;
uint16_t ch7;

uint16_t high = 0;    //液位高度
float high_cm = 0.0f; //液位单位换算

/*======雨量计驱动=====*/

void set_zero()
{//雨量计数据清零
  uint8_t a=0x00;
  Serial2.write(0x01);
  Serial2.write(0x06);
  Serial2.write(a);
  Serial2.write(0x37);
  Serial2.write(a);
  Serial2.write(0x03);
  Serial2.write(0x78);
  Serial2.write(0x05);
}

void mea1()
{//雨量计读数
  uint8_t a=0x00;
  uint8_t n=0x00;
  uint8_t temp[30];
  Serial2.write(0x01);
  Serial2.write(0x03);
  Serial2.write(a);
  Serial2.write(a);
  Serial2.write(a);
  Serial2.write(0x0a);
  Serial2.write(0xc5);
  Serial2.write(0xcd);
  delay(300);
  while(Serial2.available())
  {
    temp[n] = Serial2.read();
    Serial.write(temp[n]);
    n++;
  }
  if(n==25)
  {
    today = temp[3] << 8 | temp[4];
    now = temp[5] << 8 | temp[6];
    yestoday = temp[7] << 8 | temp[8];
    total = temp[9] << 8 | temp[10];
    hour = temp[11] << 8 | temp[12];
    lasthour = temp[13] << 8 | temp[14];
    hour24max = temp[15] << 8 | temp[16];
    hour24min = temp[19] << 8 | temp[20];
  }
}

/*======液位计驱动=======*/

void mea2()
{
  static uint16_t l[3]={};

  ch4 = analogRead(13);
  ch5 = analogRead(12);
  ch6 = analogRead(14);
  ch7 = analogRead(27);
  //取平均
  high = (ch4 + ch5 + ch6 + ch7) >> 2;
  //均值滤波
  for(uint8_t i=0; i<3; i++) high+=l[i];
  high=high / 4;
  for(uint8_t i=2; i>0; i--) l[i]=l[i-1];
  l[0]=high;

  //换算
  
}

void setup() 
{
  Serial.begin(9600);
  Serial2.begin(4800);
}

void loop() 
{
  
}