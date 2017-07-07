#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

class Player : public Entity {
  public:
    Player(uint16_t x, uint16_t y, Level* level) 
      : Entity(x, y, B100000, 4, level) {}
    virtual ~Player(){}
    virtual void draw(Adafruit_ST7735& tft);
    virtual void collideWithCamera(Adafruit_ST7735& tft);
    virtual void updateEntity(Adafruit_ST7735& tft, byte button);

    void explode(Adafruit_ST7735& tft);
  
  private:
    void reachedHome();
    
};

void Player::draw(Adafruit_ST7735& tft) {
  m_level->getTileset()->drawSprite(m_sprite, m_posX-m_level->getCameraX()*16, m_posY-m_level->getCameraY()*16, tft);
}

void Player::collideWithCamera(Adafruit_ST7735& tft) {
  if (m_posX >= 8 * 16 && m_level->getCameraX() == 0) {
    m_level->getCameraX() = 7;
    //m_posX = 16;
    m_level->drawLevel(tft);
  } 
  if (m_posX < 8*16 && m_level->getCameraX() != 0) {
    m_level->getCameraX() = 0;
    //m_posX += 7 * 16;
    m_level->drawLevel(tft);
  }
  if (m_posY >= 6 * 16 && m_level->getCameraY() == 0) {
    m_level->getCameraY() = 5;
    //m_posY = 16;
    m_level->drawLevel(tft);
  }
  if (m_posY < 6*16 && m_level->getCameraY() != 0) {
    m_level->getCameraY() = 0;
    //m_posY += 5 * 16;
    m_level->drawLevel(tft);
  }
}

void Player::updateEntity(Adafruit_ST7735& tft, byte button) {
  int8_t vel[2] = {0};
  switch (button) {
    case 1: //up
      m_sprite &= B0000001;
      m_sprite = B0001000 | m_sprite ^ (1 << 0);
      vel[1] = -1;
      break;
    case 2: //down
      m_sprite &= B0000001;
      m_sprite = B0100000 | m_sprite ^ (1 << 0);
      vel[1] = 1;
      break;
    case 3: //left
      m_sprite &= B0000001;
      m_sprite = B0010000 | m_sprite ^ (1 << 0);
      vel[0] = -1;
      break;
    case 4: //right
      m_sprite &= B0000001;
      m_sprite = B1000000 | m_sprite ^ (1 << 0);
      vel[0] = 1;
      break;
    case 5: //A
      break;
    case 6: //B
      break;
    default:
      break;
  }
  collide(vel, tft);
  reachedHome();
}

void Player::explode(Adafruit_ST7735& tft){
  m_sprite = B0000100;
  draw(tft);
  delay(150);
  tft.fillRect(m_posX-m_level->getCameraX()*16, m_posY-m_level->getCameraY()*16, 16, 16, DARK_GREEN);
  m_sprite = B0000101;
  draw(tft);
  delay(180);
  tft.fillRect(m_posX-m_level->getCameraX()*16, m_posY-m_level->getCameraY()*16, 16, 16, DARK_GREEN);
}

void Player::reachedHome(){
  byte coordX, coordY;
  if(m_posX % 16 == 0){
    coordX = m_posX / 16;
    if(m_posY % 16 < 4)
      coordY = m_posY / 16;
    else
      coordY = 0;
  }else{
    coordY = m_posY / 16;
    if(m_posX % 16 < 4)
      coordX = m_posX / 16;
    else
      coordX = 0;
  }
  char tile = m_level->getTileAt(coordY, coordX);
  if(tile == 'H'){
    m_level->setDone();
  }
}

#endif
