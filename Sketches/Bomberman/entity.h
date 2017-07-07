#ifndef ENTITY_H
#define ENTITY_H

#include "colors.h"
#include "level.h"
#include <Adafruit_ST7735.h>

class Entity {
  public:
    Entity(){}
    Entity(uint16_t x, uint16_t y, byte sprite, byte vel, Level* level);
    virtual ~Entity(){};

    int16_t getX();
    int16_t getY();
    bool collideWithEntity(Entity* entity);
    virtual void draw(Adafruit_ST7735& tft){};
    bool collide(int8_t vel[], Adafruit_ST7735& tft);
    virtual void collideWithCamera(Adafruit_ST7735& tft){};
    virtual void updateEntity(Adafruit_ST7735& tft, byte button) = 0;

  protected:
    Level* m_level;
    uint16_t m_posX;
    uint16_t m_posY;
    int8_t m_vel;
    byte m_sprite;

};

Entity::Entity(uint16_t x, uint16_t y, byte sprite, byte vel, Level* level) {
  m_posX = x;
  m_posY = y;
  m_level = level;
  m_vel = vel;
  m_sprite = sprite;
}


int16_t Entity::getX() {
  return m_posX;
}
int16_t Entity::getY() {
  return m_posY;
}

bool Entity::collideWithEntity(Entity* entity){
  return !(m_posX+16 < entity->getX()+3 || entity->getX()+16 < m_posX+3 ||
           m_posY+16 < entity->getY()+3 || entity->getY()+16 < m_posY+3);
}

bool Entity::collide(int8_t vel[], Adafruit_ST7735& tft){
  if(vel[0] == 0 && vel[1] == 0){
    return false;
  }
  
  int16_t x, y;
  x = m_posX-m_level->getCameraX()*16;
  y = m_posY-m_level->getCameraY()*16;
  if(x >= 0 && y >= 0)
    tft.fillRect(m_posX-m_level->getCameraX()*16, m_posY-m_level->getCameraY()*16, 16, 16, DARK_GREEN);
  m_posX += vel[0] * m_vel;
  m_posY += vel[1] * m_vel;

  collideWithCamera(tft);
  
  if(m_posX < 16)
    m_posX = 16;
  if(m_posX+16 >= (m_level->getLvlWidth()-1)*16)
    m_posX = (m_level->getLvlWidth()-2)*16;
  if(m_posY < 16)
    m_posY = 16;
  if(m_posY+16 >= (m_level->getLvlHeight()-1)*16)
    m_posY = (m_level->getLvlHeight()-2)*16;

  byte cameraOffsetX = m_level->getCameraX()*16;
  byte cameraOffsetY = m_level->getCameraY()*16;
  
  char topLeftTile = m_level->getTileAt(m_posY / 16, m_posX / 16);
  char topRightTile = m_level->getTileAt(m_posY / 16, (m_posX + 15) / 16);
  char bottomLeftTile = m_level->getTileAt((m_posY + 15) / 16, m_posX / 16);
  char bottomRightTile = m_level->getTileAt((m_posY + 15) / 16, (m_posX + 15) / 16);

  int8_t depth;
  bool allGood = true;
  
  if(topLeftTile != ' ' && topLeftTile != 'H' && topLeftTile != 'F'){
    if(vel[0] == 0){
      depth = ((m_posX / 16) + 1) * 16 - m_posX;
      if(depth < 5){
        m_posX += depth;
      }else if(depth == 16){
        m_posY = m_posY%16 == 0 ? m_posY : ((m_posY / 16) + 1) * 16;
      }else{
        allGood = false;
      }
    }else{
      depth = ((m_posY / 16) + 1) * 16 - m_posY;
      if(depth < 5){ 
        m_posY += depth;
      }else if(depth == 16){
        m_posX = m_posX%16 == 0 ? m_posX : ((m_posX / 16) + 1) * 16;
      }else{
        allGood = false;
      }
    }
  }
  if(topRightTile != ' ' && topRightTile != 'H' && topRightTile != 'F'){
    if(vel[0] == 0){
      depth = m_posX + 16 - ((m_posX + 15) / 16) * 16;
      if(depth < 5){
        m_posX -= depth;
      }else if(depth == 16){
        m_posY = m_posY%16 == 0 ? m_posY : ((m_posY / 16) + 1) * 16;
      }else{
        allGood = false;
      }
    }else{
      depth = ((m_posY / 16) + 1) * 16 - m_posY;
      if(depth < 5){
        m_posY += depth;
      }else if(depth == 16){
        m_posX = m_posX%16 == 0 ? m_posX : (m_posX / 16) * 16;
      }else{
        allGood = false;
      }
    }
  }
  if(bottomLeftTile != ' ' && bottomLeftTile != 'H' && bottomLeftTile != 'F'){
    if(vel[0] == 0){
      depth = ((m_posX / 16) + 1) * 16 - m_posX;
      if(depth < 5){
        m_posX += depth;
      }else if(depth == 16){
        m_posY = m_posY%16 == 0 ? m_posY : (m_posY / 16) * 16;
      }else{
        allGood = false;
      }
    }else{
      depth = m_posY + 16 - ((m_posY + 15) / 16) * 16;
      if(depth < 5){ 
        m_posY -= depth;
      }else if(depth == 16){
        m_posX = m_posX%16 == 0 ? m_posX : ((m_posX / 16) + 1) * 16;
      }else{
        allGood = false;
      }
    }
  }
  if(bottomRightTile != ' ' && bottomRightTile != 'H' && bottomRightTile != 'F'){
    if(vel[0] == 0){
      depth = m_posX + 16 - ((m_posX + 15) / 16) * 16;
      if(depth < 5){
        m_posX -= depth;
      }else if(depth == 16){
        m_posY = m_posY%16 == 0 ? m_posY : (m_posY / 16) * 16;
      }else{
        allGood = false;
      }
    }else{
      depth = m_posY + 16 - ((m_posY + 15) / 16) * 16;
      if(depth < 5){
        m_posY -= depth;
      }else if(depth == 16){
        m_posX = m_posX%16 == 0 ? m_posX : (m_posX / 16) * 16;
      }else{
        allGood = false;
      }
    }
  }
  if(!allGood){
    m_posX -= vel[0] * m_vel;
    m_posY -= vel[1] * m_vel;
    return true;
  }
  return false;
}

#endif
