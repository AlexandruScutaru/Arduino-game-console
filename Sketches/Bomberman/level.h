#ifndef LEVEL_H
#define LEVEL_H

#include <Adafruit_ST7735.h>
#include <SD.h>
#include "tileset.h"

class Level {
  public:
    Level(Tiles* tiles, byte ballooms){
      m_tiles = tiles;
      m_ballooms = ballooms;
      m_done = false;
    }
    char getTileAt(int16_t row, int16_t col) {
      return data[row][col];
    }
    void setTile(char tile, byte row, byte col) {
      data[row][col] = tile;
    }
    Tiles* getTileset() {
      return m_tiles;
    }
    byte& getCameraX(){
      return m_cameraX;
    }
    byte& getCameraY(){
      return m_cameraY;
    }
    byte getLvlWidth(){
      return m_levelWidth;
    }
    byte getLvlHeight(){
      return m_levelHeight;
    }
    byte getBalloomsCount(){
      return m_ballooms;
    }
    void decBallooms(){
      if(m_ballooms > 0)
        m_ballooms--;
    }
    void initLevel();
    void drawLevel(Adafruit_ST7735& tft);
    bool isDone(){
      return m_done;
    }
    void setDone(){
      m_done = m_ballooms == 0;
    }
  private:
    byte m_ballooms;
    char data[13][17];
    byte m_levelWidth;
    byte m_levelHeight;
    byte m_cameraX;
    byte m_cameraY;
    uint16_t m_bricks;
    Tiles* m_tiles;
    File m_levels;
    bool m_done;
};

void Level::drawLevel(Adafruit_ST7735& tft) {
  tft.fillScreen(DARK_GREEN);
  for (byte row = m_cameraY; row < m_cameraY + 8; row++) {
    for (byte col = m_cameraX; col < m_cameraX + 10; col++) {
      switch (data[row][col]) {
        case 'S':
          m_tiles->drawBlock("ws.bmp", (col - m_cameraX) * 16, (row - m_cameraY) * 16, tft);
          break;
        case 'W':
        case 'Q':
          m_tiles->drawBlock("wb.bmp", (col - m_cameraX) * 16, (row - m_cameraY) * 16, tft);
          break;
        case 'H':
          m_tiles->drawBlock("home.bmp", (col - m_cameraX) * 16, (row - m_cameraY) * 16, tft);
          break;
        default:
          break;
      }
    }
  }
}

void Level::initLevel() {
  m_levelWidth = 17;
  m_levelHeight = 13;
  m_cameraX = 0;
  m_cameraY = 0;
  m_bricks = 10 * m_levelWidth * m_levelHeight / 100;
  for (byte col = 0; col < m_levelWidth; col++) {
    data[0][col] = 'S';
    data[m_levelHeight - 1][col] = 'S';
  }
  for (byte row = 1; row < m_levelHeight; row++) {
    data[row][0] = 'S';
    data[row][m_levelWidth - 1] = 'S';
  }
  for (byte row = 1; row < m_levelHeight - 1; row++) {
    for (byte col = 1; col < m_levelWidth - 1; col++) {
      if (row % 2 == 0 && col % 2 == 0) {
        data[row][col] = 'S';
      } else {
        data[row][col] = ' ';
      }
    }
  }
  //reserving 3 blocks for player spawn
  data[1][1] = 'r'; data[1][2] = 'r'; data[2][1] = 'r';
  while (1) {
    randomSeed(analogRead(A1));
    byte row = random(1, m_levelHeight);
    byte col = random(1, m_levelWidth);
    if ((row % 2 == 1 && col % 2 == 0) || (row % 2 == 0 && col % 2 == 1)) {
      if (data[row][col] == ' ') {
        data[row][col] = 'Q';
        m_bricks--;
        break;
      }
    }
  }
  
  while (m_bricks > 0) {
    randomSeed(analogRead(A1));
    byte row = random(1, m_levelHeight);
    byte col = random(1, m_levelWidth);
    if ((row % 2 == 1 && col % 2 == 0) || (row % 2 == 0 && col % 2 == 1)) {
      if (data[row][col] == ' ') {
        data[row][col] = 'W';
        m_bricks--;
      }
    }
  }
  
  //unreserving 3 blocks for player spawn
  data[1][1] = ' '; data[1][2] = ' '; data[2][1] = ' ';
}

#endif
