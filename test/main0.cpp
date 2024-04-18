/**
 * Program      CYD_Sprites.cpp
 * Author       2024-03-29 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Shows how to create and handle sprites
 * 
 * Board        ESP32-2432S028
 *
 * Wiring       None
 *
 * Remarks      A screenshot of the sprites is stored to the SD card
 *
 * References   https://wiki.elecrow.com/index.php?title=Lesson02_Simple_graphics_drawing
 */

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "lgfx_ESP32_2432S028.h"
#include <SD.h>
#include "UiComponents.h"
#include "Wait.h"

using Action = void(&)(LGFX &lcd);
enum Rotation {PORTRAIT, LANDSCAPE};
GFXfont myFont = fonts::DejaVu18;

extern GFXfont defaultFont;
extern void nop(LGFX &lcd);
extern void initDisplay(LGFX &lcd, uint8_t rotation=0, lgfx::v1::GFXfont *theFont=&myFont, Action greet=nop);
extern void initSDCard(SPIClass &spi);
extern void lcdInfo(LGFX &lcd);
extern void nop(LGFX &lcd);
extern bool saveBmpToSD_16bit(LGFX &lcd, const char *filename);
extern void printSDCardInfo();


LGFX lcd;
LGFX_Sprite smiley(&lcd);
LGFX_Sprite angry(&lcd);
LGFX_Sprite btnOn(&lcd);
LGFX_Sprite btnOff(&lcd);

SPIClass sdcardSPI(VSPI);


Wait waitSmiley(750);
Wait waitAngry(2000);


void createSmiley()
{
  smiley.createSprite(100, 100);
  smiley.fillSprite(TFT_MAROON);
  smiley.fillRoundRect(0, 0, 100, 100, 50, TFT_ORANGE);
  smiley.fillCircle(30, 35, 10, TFT_BLACK);
  smiley.fillCircle(70, 35, 10, TFT_BLACK);
  smiley.fillArc(50, 10, 50, 54, 70.0, 110.0, TFT_BLACK);
  smiley.setFont(&myFont);
  smiley.setTextColor(TFT_RED, TFT_ORANGE);
  smiley.drawString("Smile!", 20, 70);
}

void createAngry()
{
  angry.createSprite(100, 100);
  angry.fillRect(0, 0, 100, 100, TFT_DARKGREEN);
  angry.fillCircle(30, 30, 10, TFT_BLACK);
  angry.fillCircle(70, 30, 10, TFT_BLACK);
  angry.fillArc(50, 100, 50, 54, 250.0, 300.0, TFT_BLACK);
  angry.setFont(&myFont);
  angry.setTextColor(TFT_YELLOW, TFT_DARKGREEN);
  angry.drawString("Arrrg!", 30, 75);
}


void moveSmiley()
{
  static int x = 10;
  static int y = 10;
  lcd.fillRect(x, y, 100, 100,TFT_MAROON);
  x = random(10, 60);
  y = random(10, 130);
  smiley.pushSprite(x, y);
}


void moveAngry()
{
  static int x = 10;
  static int y = 10;
  lcd.fillRect(x, y, 100, 100,TFT_MAROON);
  x = random(160, 210);
  y = random(10, 130);
  angry.pushSprite(x, y); 
}


void setup() 
{
  Serial.begin(115200);

  initDisplay(lcd, Rotation::LANDSCAPE, &myFont, lcdInfo);
  lcd.setRotation(1);
  lcdInfo(lcd);
  lcd.setBaseColor(TFT_MAROON);
  lcd.clear();
  lcd.drawRect(5, 5, 310, 230, TFT_DARKGREEN);
  initSDCard(sdcardSPI);
  printSDCardInfo();
  createSmiley();
  waitSmiley.begin();

  createAngry();
  waitAngry.begin();
}


void loop() 
{
  static bool saved = false;
  if (waitSmiley.isOver()) { moveSmiley(); }
  if (waitAngry.isOver())  
    { 
      moveAngry(); 
      if (! saved) 
      {
        saveBmpToSD_16bit(lcd, "/smileys.bmp"); 
        saved = true; 
      }
    }
}
