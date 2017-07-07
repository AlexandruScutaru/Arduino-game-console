#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "colors.h"
#include "tileset.h"
#include "level.h"
#include "input_manager.h"
#include "enemy.h"
#include "player.h"
#include "bomb.h"

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
Tiles tiles;
Level level(&tiles, 5);
InputManager inputManager;
Player player(16, 16, &level);

Balloom* ballooms[5];
Balloom* balloom;
Bomb bomb;

byte updateTime = 150;
long lastUpdate = 0;
long milliseconds = 0;

void dead(char* msg);
void spawnBallooms();

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  if (!tiles.init()) {
    while (1);
  }
  tft.setRotation(1);
  level.initLevel();
  level.drawLevel(tft);
  spawnBallooms();
}

void loop() {
  byte buttonPressed;
  milliseconds = millis();
  if (milliseconds - lastUpdate > updateTime) {
    buttonPressed = inputManager.getButton();
    player.updateEntity(tft, buttonPressed);
    player.draw(tft);
    
    for(byte i = 0; i < 5; i++){
      if(ballooms[i]){
        ballooms[i]->updateEntity(tft, buttonPressed);
        ballooms[i]->draw(tft);
        if (player.collideWithEntity(ballooms[i])) {
          char* msg = "Died!";
          dead(msg);
        }
        if (bomb.insideFlames(ballooms[i]->getX(), ballooms[i]->getY(), level)) {
          if (ballooms[i]->getX() >= level.getCameraX() * 16 && ballooms[i]->getY() >= level.getCameraY() * 16)
            tft.fillRect(ballooms[i]->getX() - level.getCameraX() * 16, 
                         ballooms[i]->getY() - level.getCameraY() * 16, 
                         16, 16, DARK_GREEN);
          delete ballooms[i];
          ballooms[i] = NULL;
          level.decBallooms();
        }
      }
    }
    
    if (bomb.insideFlames(player.getX(), player.getY(), level)) {
      char* msg = "Died!";
      dead(msg);
    }
    if (buttonPressed == 5 && !bomb.isAlive()) {
      bomb.init(player.getX(), player.getY(), level);
    }
    if (bomb.isAlive()) {
      bomb.draw(level, tft);
    }

    if (level.isDone()) {
      char* msg = "Done!";
      dead(msg);
    }
    lastUpdate = milliseconds;
  }
}

void spawnBallooms(){
  byte count = 0;
  while (count < 5) {
    randomSeed(analogRead(A1));
    byte row = random(1, level.getLvlHeight()-1);
    byte col = random(1, level.getLvlWidth()-1);
    if ((row % 2 == 1 && col % 2 == 0) || (row % 2 == 0 && col % 2 == 1)) {
      for(byte i = 0; i < count ; i++)
        if(row == ballooms[i]->getY()/16 && col == ballooms[i]->getX()/16)
          continue;
      if (level.getTileAt(row, col) == ' ') {
        balloom = new Balloom(col*16, row*16, &level);
        ballooms[count] = balloom;
        count++;
      }
    }
  }
}

void dead(char* msg) {
  player.explode(tft);
  tft.setCursor(16, 120);
  tft.setTextColor(WHITE, BLACK);
  tft.print(msg);
  while (1);
}

