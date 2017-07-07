#ifndef ENEMY_H
#define ENEMY_h

#include "entity.h"

class Balloom : public Entity {
  public:
    Balloom() {}
    Balloom(uint16_t x, uint16_t y, Level* level)
      : Entity(x, y, B0000010, 4, level)
    {
      m_update = true;
      m_length = 0;
      m_vel[0] = 0;
      m_vel[1] = 0;
    }
    virtual ~Balloom() {}

    virtual void draw(Adafruit_ST7735& tft);
    virtual void collideWithCamera(Adafruit_ST7735& tft);
    virtual void updateEntity(Adafruit_ST7735& tft, byte button);
  private:
    int8_t genLength();
    void genVelocity();

    bool m_update;
    int8_t m_length;
    int8_t m_vel[2];
};

void Balloom::draw(Adafruit_ST7735& tft) {
  if (m_update) {
    if (m_posX >= m_level->getCameraX() * 16 && m_posY >= m_level->getCameraY() * 16)
      m_level->getTileset()->drawSprite(m_sprite, m_posX - m_level->getCameraX() * 16, m_posY - m_level->getCameraY() * 16, tft);
    m_update = false;
  } else {
    m_update = true;
  }
}

void Balloom::collideWithCamera(Adafruit_ST7735& tft) {

}

void Balloom::updateEntity(Adafruit_ST7735& tft, byte button) {
  if (m_update) {
    if (m_length > 0) {
      if(collide(m_vel, tft)){
        m_length = genLength();
        genVelocity();
      }
      m_length--;
    } else {
      m_length = genLength();
      genVelocity();
      if(collide(m_vel, tft)){
        m_length = genLength();
        genVelocity();
      }
      m_length--;
    }
  }
}

int8_t Balloom::genLength() {
  randomSeed(analogRead(A1));
  return random(1, 10);
}

void Balloom::genVelocity() {
  randomSeed(analogRead(A1));
  byte dir = random(0, 31);
  if (dir >= 0 && dir < 10)
    m_vel[0] = -1;
  else if (dir >= 10 && dir < 20)
    m_vel[0] = 0;
  else
    m_vel[0] = 1;
  if (m_vel[0] == 0) { 
    randomSeed(analogRead(A1));
    dir = random(0, 31);
    if (dir >= 0 && dir < 10)
      m_vel[1] = -1;
    else if (dir >= 10 && dir < 20)
      m_vel[1] = 0;
    else
      m_vel[1] = 1;
  }else{
    m_vel[1] = 0;
  }
}

#endif
