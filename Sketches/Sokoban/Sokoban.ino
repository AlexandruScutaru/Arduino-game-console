#include "input_manager.h"
#include "player.h"
#include "colors.h"
#include "tileset.h"
#include "level.h"
#include "menu.h"
#include "sounds.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8 
#define TFT_DC     9

#define PLAY       0
#define MENU       1
 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

InputManager inputManager;
Tiles tiles;
Level level(&tiles);
Menu menu;
Player player(&level);
SoundEffects sounds;

byte gamestate = MENU;
byte buttonPressed;

byte updateTime = 150;
long lastUpdate = 0; 
long milliseconds = 0;

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  if(!tiles.init()){
    while(1);
  }
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  menu.init();
  menu.draw(tft);
}

void loop(){
  milliseconds = millis();
  if(milliseconds - lastUpdate > updateTime){
    buttonPressed = inputManager.getButton();
    if(buttonPressed == 34){
      if(gamestate == PLAY){
        gamestate = MENU;
        menu.init();
        tft.fillScreen(BLACK);
        menu.draw(tft);
      }
    }
    if(gamestate == PLAY){
      player.updatePlayer(tft, buttonPressed, sounds);
      if(level.isDone()){
        tft.fillScreen(BLACK);
        tft.setTextSize(1);
        tft.setCursor(64, 57);
        tft.print("Done!");
        sounds.playClearMelody();
        level.drawLevel(tft);
      }
    }else{
      if(menu.updateMenu(tft, buttonPressed)){
        gamestate = PLAY;
        level.init();
        level.drawLevel(tft);
      }
    }
    lastUpdate = milliseconds;
  }
}

