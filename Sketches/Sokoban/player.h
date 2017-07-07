#ifndef PLAYER_H
#define PLAYER_H

#include "colors.h"
#include "level.h"
#include "sounds.h"
#include <Adafruit_ST7735.h>

class Player{
public:
  Player(Level* level){
    m_level = level;
    m_lastMove[0] = 0;
    m_lastMove[1] = 0;
    m_lastMove[2] = 0;
    m_lastMove[3] = B01000;
  }
  void updatePlayer(Adafruit_ST7735& tft, byte button, SoundEffects& sounds);
private:
  void movePlayer(int8_t mvDir[], Adafruit_ST7735& tft, SoundEffects& sounds);
  void undo(Adafruit_ST7735& tft);
  bool canMove(int8_t mvDir[]);
  void resetLevel(Adafruit_ST7735& tft);
  
  Level* m_level;
  int8_t m_lastMove[4];
};

void Player::updatePlayer(Adafruit_ST7735& tft, byte button, SoundEffects& sounds){
  int8_t dir[2] = {0, 0};
  switch(button){
    case 1: //up
      dir[0] = -1; 
      movePlayer(dir, tft, sounds);
      break;
    case 2: //down
      dir[0] = 1; 
      movePlayer(dir, tft, sounds);
      break;
    case 3: //left
      dir[1] = -1; 
      movePlayer(dir, tft, sounds);
      break;
    case 4: //right
      dir[1] = 1;
      movePlayer(dir, tft, sounds);
      break;
    case 5: //A
      undo(tft);
      break;
    case 6: //B
      resetLevel(tft);
      break;
    default:
      break;
  }
}

void Player::movePlayer(int8_t mvDir[], Adafruit_ST7735& tft, SoundEffects& sounds){
   if(canMove(mvDir)){
      m_lastMove[0] = mvDir[0];
      m_lastMove[1] = mvDir[1];
      m_lastMove[2] = 0;
      byte col = m_level->getPlayerX();
      byte row = m_level->getPlayerY();
      char tile = m_level->getTileAt(row+mvDir[0], col+mvDir[1]);
      byte sprite = B00000;
      
      if(tile == 'B' || tile == 'V'){
        tile = m_level->getTileAt(row + 2*mvDir[0], col + 2*mvDir[1]);
        if(tile != 'X' && tile != 'B' && tile != 'V'){
          if(tile == 'S'){
            m_level->getTileset()->drawBlock("crate.bmp", (col + 2*mvDir[1])*16, (row + 2*mvDir[0])*16, tft);
            tft.drawRect((col + 2*mvDir[1])*16, (row + 2*mvDir[0])*16, 16, 16, GREEN);
            m_level->setTile('V', row + 2*mvDir[0], col + 2*mvDir[1]);
            sounds.playCratePlaced();
          }else if(tile == ' '){
            m_level->getTileset()->drawBlock("crate.bmp", (col + 2*mvDir[1])*16, (row + 2*mvDir[0])*16, tft);
            m_level->setTile('B', row + 2*mvDir[0], col + 2*mvDir[1]);
          }
          if(m_level->getTileAt(row+mvDir[0], col+mvDir[1]) == 'V'){
              m_level->setTile('S', row+mvDir[0], col+mvDir[1]);
              sounds.playCrateRemoved();  
          }else{
              m_level->setTile(' ', row+mvDir[0], col+mvDir[1]);
          }
          sprite = B10000;
          m_lastMove[2] = 1;
        }
      }
      
      tile = m_level->getTileAt(row+mvDir[0], col+mvDir[1]);
      if(tile == ' ' || tile == 'S'){
        if(m_level->getTileAt(row, col) == 'S'){
            m_level->getTileset()->drawBlock("spot.bmp", col*16, row*16, tft);
        }else{
            tft.fillRect(col*16, row*16, 16, 16, BLACK);
        }
        
        sprite = mvDir[0] != 0 ? (mvDir[0] == 1 ? sprite | B00010 : sprite | B01000) : 
                                 (mvDir[1] == 1 ? sprite | B00100 : sprite | B00001);
          
        m_level->getTileset()->drawPlayer(sprite, (col+mvDir[1])*16, (row+mvDir[0])*16, tft);
        m_level->getPlayerY() += mvDir[0];
        m_level->getPlayerX() += mvDir[1];
      }
      m_lastMove[3] = sprite;
   }
}

void Player::undo(Adafruit_ST7735& tft){  
  byte col = m_level->getPlayerX();
  byte row = m_level->getPlayerY();

  if(m_level->getTileAt(row, col) == 'S'){
    m_level->getTileset()->drawBlock("spot.bmp", col*16, row*16, tft);
  }else{
    tft.fillRect(col*16, row*16, 16, 16, BLACK);
  }
  m_level->getTileset()->drawPlayer(m_lastMove[3], (col-m_lastMove[1])*16, (row-m_lastMove[0])*16, tft);
  m_level->getPlayerY() -= m_lastMove[0];
  m_level->getPlayerX() -= m_lastMove[1];

  if(m_lastMove[2] == 1){
    char tile = m_level->getTileAt(row+m_lastMove[0], col+m_lastMove[1]);    
    if(tile == 'B' || tile == 'V'){
        if(tile == 'B'){
          tft.fillRect((col+m_lastMove[1])*16, (row+m_lastMove[0])*16, 16, 16, BLACK);
          m_level->setTile(' ', row+m_lastMove[0], col+m_lastMove[1]);
        }else if(tile == 'V'){
          m_level->getTileset()->drawBlock("spot.bmp", (col+m_lastMove[1])*16, (row+m_lastMove[0])*16, tft);
          m_level->setTile('S', row+m_lastMove[0], col+m_lastMove[1]);
        }
        m_level->getTileset()->drawBlock("crate.bmp", col*16, row*16, tft);
        if(m_level->getTileAt(row, col) == 'S'){
          tft.drawRect(col*16, row*16, 16, 16, GREEN);
          m_level->setTile('V', row, col);
        }else{
          m_level->setTile('B', row, col);
        }
    }
  }
  m_lastMove[0] = 0;
  m_lastMove[1] = 0;
  m_lastMove[2] = 0;
}

bool Player::canMove(int8_t mvDir[]){
  byte col = m_level->getPlayerX();
  byte row = m_level->getPlayerY();
  char tile = m_level->getTileAt(row+mvDir[0], col+mvDir[1]);
  if(tile == 'B' || tile == 'V'){
      tile = m_level->getTileAt(row + 2*mvDir[0], col + 2*mvDir[1]);
      if(tile != 'X' && tile != 'B' && tile != 'V'){
        return true;
      }
  }else if(tile == ' ' || tile == 'S'){
    return true;
  }
  return false;
}

void Player::resetLevel(Adafruit_ST7735& tft){
  m_level->setLevel(byte(m_level->getLevel()-1));
  m_level->drawLevel(tft);
  m_lastMove[0] = 0;
  m_lastMove[1] = 0;
  m_lastMove[2] = 0;
  m_lastMove[3] = B01000;
}

#endif

