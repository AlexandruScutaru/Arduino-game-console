#ifndef BOMB_H
#define BOMB_H

#include "level.h"

class Bomb {
  public:
    Bomb() {
      m_ttl = 0;
      m_fireLength = 2;
    }
    void draw(Level& level, Adafruit_ST7735& tft);
    bool isAlive();
    bool exploded();
    void init(int16_t x, int16_t y, Level& level);
    bool insideFlames(int16_t x, int16_t y, Level& level);
    
  private:
    void drawFire(Level& level, Adafruit_ST7735& tft);
    void removeFire(Level& level, Adafruit_ST7735& tft);
    bool drawExplosion(int16_t, int16_t, char* tile, Level& level, Adafruit_ST7735& tft);
    void removeExplosion(byte, byte, Level& level, Adafruit_ST7735& tft);
    void findMajority(int16_t x, int16_t y, byte& coordX, byte& coordY, Level& level);
    
    byte m_x;
    byte m_y;
    byte m_fireLength;
    byte m_ttl;
    
};

void Bomb::draw(Level& level, Adafruit_ST7735& tft) {
  if (m_ttl == 3) {
    drawFire(level, tft);
  } else if(m_ttl > 5) {
    char* sprite;
    if (m_ttl % 2 == 0) {
      sprite = "bomb2.bmp";
    } else {
      sprite = "bomb1.bmp";
    }
    level.getTileset()->drawBlock(sprite, (m_x-level.getCameraX()) * 16, (m_y-level.getCameraY()) * 16, tft);
  }
  m_ttl--;
  if(m_ttl == 0){
    level.setTile(' ', m_y, m_x);
    removeFire(level, tft);
  }
}

bool Bomb::isAlive() {
  if (m_ttl == 0)
    return false;
  return true;
}

bool Bomb::exploded(){
  if(m_ttl > 1 && m_ttl <= 2)
    return true;
  return false;
}

void Bomb::init(int16_t x, int16_t y, Level& level) { 
  findMajority(x, y, m_x, m_y, level);
  m_ttl = 28;
  level.setTile('B', m_y, m_x);
}

void Bomb::drawFire(Level& level, Adafruit_ST7735& tft){
  level.getTileset()->drawBlock("fire1C.bmp", (m_x-level.getCameraX()) * 16, (m_y-level.getCameraY()) * 16, tft);
  
  for(byte len = 1; len <= m_fireLength; len++){
    if(len == m_fireLength){
      drawExplosion(m_x-len, m_y, "fire1L.bmp", level, tft);
      break;
    }
    if(!drawExplosion(m_x-len, m_y, "fire1H.bmp", level, tft))
      break;
  }
  for(byte len = 1; len <= m_fireLength; len++){
    if(len == m_fireLength){
      drawExplosion(m_x, m_y-len, "fire1U.bmp", level, tft);
      break;
    }
    if(!drawExplosion(m_x, m_y-len, "fire1V.bmp", level, tft))
      break;
  }
  for(byte len = 1; len <= m_fireLength; len++){
    if(len == m_fireLength){
      drawExplosion(m_x+len, m_y, "fire1R.bmp", level, tft);
      break;
    }
    if(!drawExplosion(m_x+len, m_y, "fire1H.bmp", level, tft))
      break;
  }
  for(byte len = 1; len <= m_fireLength; len++){
    if(len == m_fireLength){
      drawExplosion(m_x, m_y+len, "fire1D.bmp", level, tft);
      break;
    }
    if(!drawExplosion(m_x, m_y+len, "fire1V.bmp", level, tft))
      break;
  }
}

void Bomb::removeFire(Level& level, Adafruit_ST7735& tft){
  if(m_x >= level.getCameraX() && m_y >= level.getCameraY())
    tft.fillRect((m_x-level.getCameraX())*16, (m_y-level.getCameraY())*16, 16, 16, DARK_GREEN);

  for(byte len = 1; len <= m_fireLength; len++){
    removeExplosion(byte(m_x+len), m_y, level, tft);
    removeExplosion(byte(m_x-len), m_y, level, tft);
    removeExplosion(m_x, byte(m_y+len), level, tft);
    removeExplosion(m_x, byte(m_y-len), level, tft);
  }
}

bool Bomb::drawExplosion(int16_t x, int16_t y, char* tile, Level& level, Adafruit_ST7735& tft){
  char matrixValue = level.getTileAt(y, x);
  bool flag = true;
  switch(matrixValue){
    case 'Q':
      level.setTile('H', y, x);
      level.getTileset()->drawBlock("home.bmp", (x-level.getCameraX()) * 16, (y-level.getCameraY()) * 16, tft);
      flag = false;
      break;
    case 'W':
      level.setTile('F', y, x);
      level.getTileset()->drawBlock("wb2.bmp", (x-level.getCameraX()) * 16, (y-level.getCameraY()) * 16, tft);
      flag = false;
      break;
    case 'S':
      flag = false;
      break;
    case 'H':
      flag = false;
      break;
    case ' ':
      level.setTile('F', y, x);
      level.getTileset()->drawBlock(tile, (x-level.getCameraX()) * 16, (y-level.getCameraY()) * 16, tft);
      break;
  }
  return flag;
}

void Bomb::removeExplosion(byte x, byte y, Level& level, Adafruit_ST7735& tft){
  if(level.getTileAt(y, x) == 'F'){
      level.setTile(' ', y, x);
      if(x >= level.getCameraX() && y >= level.getCameraY())
        tft.fillRect((x-level.getCameraX())*16, (y-level.getCameraY())*16, 16, 16, DARK_GREEN);
    }
}

bool Bomb::insideFlames(int16_t x, int16_t y, Level& level){
  byte posX, posY;
  findMajority(x, y, posX, posY, level);

  if(level.getTileAt(posY, posX) == 'F')
    return true;
  return false;
}

void Bomb::findMajority(int16_t x, int16_t y, byte& coordX, byte& coordY, Level& level){
  if(x % 16 == 0){
    coordX = x / 16;
    if(y % 16 < 8)
      coordY = y / 16;
    else
      coordY = y / 16 + 1;
  }else{
    coordY = y / 16;
    if(x % 16 < 8)
      coordX = x / 16;
    else
      coordX = x/ 16 + 1;
  }
}

#endif
