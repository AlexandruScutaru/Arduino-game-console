#ifndef LEVEL_H
#define LEVEL_H

#include <Adafruit_ST7735.h>
#include <SD.h>
#include "tileset.h"

class Level{
public:
  Level(Tiles* tiles){
    m_tiles = tiles;
  }
  char getTileAt(byte row, byte col){
    return data[row][col];
  }
  void setTile(char tile, byte row, byte col){
    data[row][col] = tile;
  }
  byte getWidth(){
    return m_levelWidth;
  }
  byte getHeight(){
    return m_levelHeight;
  }
  byte& getPlayerX(){
    return m_playerX;
  }
  byte& getPlayerY(){
    return m_playerY;
  }
  Tiles* getTileset(){
    return m_tiles;
  }
  byte getLevel(){
    return m_currentLevel;
  }
  void setLevel(byte level){
    m_currentLevel = level;
  }
  void init();
  void drawLevel(Adafruit_ST7735& tft);
  bool isDone();
  
private:
  bool readNextLevel(uint16_t offset);
  uint16_t computeOffset();
  void writeCurrentLevel();
  
  char data[8][8];
  byte m_levelWidth = 8;
  byte m_levelHeight = 8;
  byte m_currentLevel;
  byte m_playerX;
  byte m_playerY;
  Tiles* m_tiles;
  File m_levels;
};

void Level::drawLevel(Adafruit_ST7735& tft){
  tft.fillScreen(BLACK);
  if(!readNextLevel(computeOffset())){
    Serial.println("Error reding levels file!");
    return;
  }else{
    for(byte row = 0; row < m_levelHeight; row++){
      for(byte col = 0; col < m_levelWidth; col++){
        switch(data[row][col]){
          case 'X':
              m_tiles->drawBlock("wall.bmp", col*16, row*16, tft);
              break;
          case 'B':
              m_tiles->drawBlock("crate.bmp", col*16, row*16, tft);
              break;
          case 'S':
              m_tiles->drawBlock("spot.bmp", col*16, row*16, tft);
              break;
          case 'V':
              m_tiles->drawBlock("crate.bmp", col*16, row*16, tft);
              tft.drawRect(col*16, row*16, 16, 16, GREEN);
              break;
          default:
               break;  
        }
      }
    }
    writeCurrentLevel();
  }
  m_tiles->drawPlayer(B01000, m_playerX*16, m_playerY*16, tft);
  tft.setTextWrap(false);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(135, 20);
  tft.print("Lvl");
  char lvl[3] = {0};
  sprintf(lvl, "%d", m_currentLevel);
  tft.setCursor(137, 30); 
  tft.print(lvl);
  
}

bool Level::readNextLevel(uint16_t offset){
  m_levels = SD.open("levels");
  if(offset >= m_levels.size()){
    offset = 0;
  }
  if(m_levels){
    m_levels.seek(offset);
    m_currentLevel = m_levels.readStringUntil('\n').toInt();
    m_playerX = m_levels.read() - '0';
    m_playerY = m_levels.read() - '0'; m_levels.read();m_levels.read();
    for(byte i = 0; i < m_levelHeight; i++){
      for(byte j = 0; j < m_levelWidth; j++){
        data[i][j] = m_levels.read();
      }
      m_levels.read();m_levels.read();
    }
    m_levels.close();  
    return true;
  }
  return false;
}

bool Level::isDone(){
  for(byte i = 0; i < m_levelHeight; i++)
    for(byte j = 0; j < m_levelWidth; j++)
      if(data[i][j] == 'S')
        return false;
  return true;
}

uint16_t Level::computeOffset(){
  byte digits = 0; 
  uint16_t offset = 86 * m_currentLevel;
  for(byte levels = 0; levels <= m_currentLevel; levels++){
    byte n = levels;
    while(n != 0){ n /= 10; digits++; }
  }
  return offset + digits;
}

void Level::init(){
  File file = SD.open("currLvl");
  if(file){
    String level = file.readStringUntil('\n');
    m_currentLevel = level.toInt()-1;
  }
  file.close();
}

void Level::writeCurrentLevel(){
  SD.remove("currLvl");
  File file = SD.open("currLvl", FILE_WRITE);
  if(file){
    file.println(m_currentLevel);
    file.close();
  }
}

#endif
