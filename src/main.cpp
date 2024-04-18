/**
 * Program      CYD_Sprites.cpp
 * Author       2024-04-18 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Shows how to create and handle sprites
 * 
 * Board        ESP32-2432S028
 *
 * Wiring       None
 *
 * Remarks      You may take a screenshot if you uncomment a line in the main loop
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

SPIClass sdcardSPI(VSPI);

Wait waitSmiley(750);
Wait waitAngry(1200);


class UiPanelSmiley : public UiPanel
{
    public:
        UiPanelSmiley(LGFX &lcd, int x, int y, int w, int h, int bgColor, bool hidden=true) : 
            UiPanel(lcd, x, y, w, h, bgColor, hidden)
        {
            if (! _hidden) { show(); }
        }

        void show()
        {
            UiPanel::show();
        }

        void addSprite(LGFX_Sprite *sprite)
        {
          _sprite = sprite;
        }
        
        void moveSprite()
        {
            static int x = _x+10;
            static int y = _y+10;
            lcd.fillRect(x, y, _sprite->width(), _sprite->height(), _bgColor);
            x = random(_x, _x + _w - _sprite->width());
            y = random(_y, _y + _h - _sprite->height());
            _sprite->pushSprite(x, y); 
        }
                
    private:
        LGFX_Sprite *_sprite = nullptr;

};


class UiPanelAngry : public UiPanel
{
    public:
        UiPanelAngry(LGFX &lcd, int x, int y, int w, int h, int bgColor, bool hidden=true) : 
            UiPanel(lcd, x, y, w, h, bgColor, hidden)
        {
            if (! _hidden) { show(); }
        }

        void show()
        {
            UiPanel::show();
        };

        void addSprite(LGFX_Sprite *sprite)
        {
          _sprite = sprite;
        }
        
        void moveSprite()
        {
            static int x = _x+10;
            static int y = _y+10;
            lcd.fillRect(x, y, _sprite->width(), _sprite->height(), _bgColor);
            x = random(_x, _x + _w - _sprite->width());
            y = random(_y, _y + _h - _sprite->height());
            _sprite->pushSprite(x, y); 
        }

    private:
        LGFX_Sprite *_sprite = nullptr;
};

// Declare pointers to the panels and initialize them with nullptr
UiPanelSmiley  *panelSmiley = nullptr; 
UiPanelAngry   *panelAngry  = nullptr; 

// Declare the static class variable again in main
std::vector<UiPanel *> UiPanel::panels;


void createSmiley(int w, int h)
{
  smiley.createSprite(w, h);
  smiley.fillSprite(TFT_OLIVE); // same color as panel background
  smiley.fillRoundRect(0, 0, w, h, 50, TFT_ORANGE);
  smiley.fillCircle(30, 35, 10, TFT_BLACK);
  smiley.fillCircle(70, 35, 10, TFT_BLACK);
  smiley.fillArc(50, 10, 50, 54, 70.0, 110.0, TFT_BLACK);
  smiley.setFont(&myFont);
  smiley.setTextColor(TFT_RED, TFT_ORANGE);
  smiley.drawString("Smile!", 20, 70);
}

void createAngry(int w, int h)
{
  angry.createSprite(w, h);
  angry.fillRect(0, 0, w, h, TFT_DARKGREEN);
  angry.fillCircle(30, 30, 10, TFT_BLACK);
  angry.fillCircle(70, 30, 10, TFT_BLACK);
  angry.fillArc(50, 100, 50, 54, 250.0, 300.0, TFT_BLACK);
  angry.setFont(&myFont);
  angry.setTextColor(TFT_YELLOW, TFT_DARKGREEN);
  angry.drawString("Arrrg!", 30, 75);
}

/**
 * 👉 An empty white image is created when SD card 
 * and touch are both active. Why are the pixels
 * not output to the file? 
*/
void takeScreenshot()
{   
    static int count= 0;
    char buf[64];
    snprintf(buf, sizeof(buf), "/SCREENSHOTS/smiley%03d.bmp", count++);
    saveBmpToSD_16bit(lcd, buf);
    log_i("Screenshot saved: %s\n", buf);
}

void setup() 
{
  Serial.begin(115200);

  initDisplay(lcd, Rotation::LANDSCAPE, &myFont, lcdInfo);
  initSDCard(sdcardSPI);
  printSDCardInfo();

  // Create the panels and showm them ( argument hidden is set to false)
  panelSmiley = new UiPanelSmiley(lcd, 0,              0, lcd.width()/2, lcd.height(), TFT_OLIVE,  false);
  panelAngry  = new UiPanelAngry (lcd, lcd.width()/2,  0, lcd.width()/2, lcd.height(), TFT_GREEN,  false);

  createSmiley(100, 100);
  panelSmiley->addSprite(&smiley);
  waitSmiley.begin();

  createAngry(100, 100);
  panelAngry->addSprite(&angry);
  waitAngry.begin(); 
}


void loop() 
{
  if (waitSmiley.isOver()) { panelSmiley->moveSprite(); }
  if (waitAngry.isOver())  { panelAngry->moveSprite(); } 
  //delay(1000); takeScreenshot(); delay(1000);  // uncomment this line to take a screenhot
}
