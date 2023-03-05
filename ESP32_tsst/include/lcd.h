#include <Arduino.h>

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

#define delay_ms delayMicroseconds

#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif

/*==================lcd引脚定义==================

    SCL -- Pin25
    SDA -- Pin26
    RES -- Pin27
    DC  -- Pin14
    CS  -- Pin12
    BL  -- Pin13

===============================================*/

#define BL 13
#define CS 12
#define DC 14
#define RE 27
#define SD 26
#define SC 25

#define LCD_SCLK_Clr() digitalWrite(SC, 0x00)//SCL=SCLK
#define LCD_SCLK_Set() digitalWrite(SC, 0x01)

#define LCD_MOSI_Clr() digitalWrite(SD, 0x00)//SDA=MOSI
#define LCD_MOSI_Set() digitalWrite(SD, 0x01)

#define LCD_RES_Clr()  digitalWrite(RE, 0x00)//RES
#define LCD_RES_Set()  digitalWrite(RE, 0x01)

#define LCD_DC_Clr()   digitalWrite(DC, 0x00)//DC
#define LCD_DC_Set()   digitalWrite(DC, 0x01)
 		     
#define LCD_CS_Clr()   digitalWrite(CS, 0x00)//CS
#define LCD_CS_Set()   digitalWrite(CS, 0x01)

#define LCD_BLK_Clr()  digitalWrite(BL, 0x00)//BLK
#define LCD_BLK_Set()  digitalWrite(BL, 0x01)

void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
