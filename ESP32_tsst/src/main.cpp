#include <Arduino.h>
#include <TFT_eSPI.h> 
#include "fonts.h"
#include "SoftwareSerial.h"
#include <stdio.h>

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

/*====================引脚定义===================

    SCL -- Pin25
    SDA -- Pin26
    RES -- Pin27
    DC  -- Pin14
    CS  -- Pin12
    BL  -- Pin13

    切换键 -- Pin33
    确认键 -- Pin32

    rs485串口RX -- Pin4
    rs485串口TX -- Pin15

===============================================*/

TFT_eSPI tft = TFT_eSPI();
SoftwareSerial rs485; //RX TX

u8 mode = 0; /*0-主界面 1-雨量计 2-液位*/
u8 sle = 0;  /*选择指针*/

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

uint16_t high = 0;    //液位高度
float high_cm = 0.0f; //液位单位换算

void set_zero()
{//雨量计数据清零
  uint8_t a=0x00;
  rs485.write(0x01);
  rs485.write(0x06);
  rs485.write(a);
  rs485.write(0x37);
  rs485.write(a);
  rs485.write(0x03);
  rs485.write(0x78);
  rs485.write(0x05);
}

void mea1()
{//雨量计读数
  uint8_t a=0x00;
  uint8_t n=0x00;
  uint8_t temp[30];
  rs485.write(0x01);
  rs485.write(0x03);
  rs485.write(a);
  rs485.write(a);
  rs485.write(a);
  rs485.write(0x0a);
  rs485.write(0xc5);
  rs485.write(0xcd);
  delay(300);
  while(rs485.available())
  {
    temp[n] = rs485.read();
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

void mea2()
{//液位读数
  static uint16_t l[3]={};
  high = analogRead(33);
  for(uint8_t i=0; i<3; i++) high+=l[i];
  high=high / 4;
  for(uint8_t i=2; i>0; i--) l[i]=l[i-1];
  l[0]=high;
}

void mov(int begin_y, int end_y, int time)
{
  float step = (float)(end_y-begin_y) / (float)(time-1);
  for(int i=0;i<time;i++)
  {
      delay(8);
      tft.pushImage(0,0,128,160,p);
      tft.drawRoundRect(2,begin_y + (int)(step * i), 126, 26, 4, TFT_YELLOW);
  }
}

void display()
{
  char text[10];
  if(mode == 1)
  {
    static uint8_t n=0;
    n++;
    if(n<50) return;
    n=0;
    tft.pushImage(0,0,128,160,p1);
    sprintf(text,"%.1fmm", today / 10.0f);
    tft.drawString(text, 84, 17);
    sprintf(text,"%.1fmm", now / 10.0f);
    tft.drawString(text, 84, 36);
    sprintf(text,"%.1fmm", yestoday / 10.0f);
    tft.drawString(text, 84, 55);
    sprintf(text,"%.1fmm", total / 10.0f);
    tft.drawString(text, 84, 75);
    sprintf(text,"%.1fmm", hour / 10.0f);
    tft.drawString(text, 84, 95);
    sprintf(text,"%.1fmm", lasthour / 10.0f);
    tft.drawString(text, 84, 115);
    sprintf(text,"%.1fmm", hour24max / 10.0f);
    tft.drawString(text, 84, 136);
  }
  else if(mode == 2)
  {
    static float l0 = 0.0f, l1 = 0.0f, l2 = 0.0f;
    static uint8_t n=0;
    n++;
    if(n<5) return;
    n=0;
    high_cm = (high - 302) * 27.0f / 55.0f;
    high_cm = (high_cm + l0 + l1 + l2) / 4.0f;
    l2 = l1;
    l1 = l0;
    l0 = high_cm;
    if(high_cm > 2)
    {
      sprintf(text, "%.1f cm", high_cm);
      tft.pushImage(0,0,128,160,p2);
      tft.drawString(text,40,100);
    }
    else
    {
      sprintf(text, "%.1f cm", 0.0f);
      tft.pushImage(0,0,128,160,p2);
      tft.drawString(text,40,100);
    }
  }
}

void key()
{//按键检测
  static u8 sw_last = 1, cf_last = 1;
  u8 sw, cf;
  sw = digitalRead(23);
  cf = digitalRead(22);

  if(sw_last && (!sw))
  {
    if(sle == 0 && mode == 0)
    {
      mov(9,38,5);
      sle=1;
    }
    else if(sle == 1 && mode == 0)
    {
      mov(38,9,5);
      sle=0;    
    }
  }

  if(cf_last && (!cf))
  {
    if(mode == 0)
    {
      if(sle == 0)
      {
        mode = 1;
        tft.pushImage(0,0,128,160,p1);
      }
      else if(sle == 1)
      {
        mode = 2;
        tft.pushImage(0,0,128,160,p2);
      }
    }
    else if(mode == 1)
    {
      tft.pushImage(0,0,128,160,p);
      tft.drawRoundRect(2,9,126,26,4,TFT_YELLOW);
      mode = 0;
    }
    else if(mode == 2)
    {
      tft.pushImage(0,0,128,160,p);
      tft.drawRoundRect(2,38,126,26,4,TFT_YELLOW);
      mode = 0;
    }
  }
  sw_last = sw;
  cf_last = cf;
}

void setup() 
{
  pinMode(23,INPUT_PULLUP);
  pinMode(22,INPUT_PULLUP);

  rs485.begin(4800, SWSERIAL_8N1, 4, 15, false, 256);
  Serial.begin(9600);

  tft.init();
  tft.pushImage(0,0,128,160,p);
  tft.drawRoundRect(2,9,126,26,4,TFT_YELLOW);
  //tft.drawRoundRect(2,38,126,26,4,TFT_YELLOW);
}

void loop() 
{
  static uint8_t n1=0;
  key();
  display();
  delay(10);
  if(mode == 1)
  {
    if(n1==100) 
    {
      mea1();
      n1=0;
    }
    n1++;
  }
  else if(mode == 2)
  {
    mea2();
  }
}