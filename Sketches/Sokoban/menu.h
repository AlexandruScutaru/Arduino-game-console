#ifndef MENU_H
#define MENU_H

#include "colors.h"
#include <Adafruit_ST7735.h>

#define PLAY 0
#define SET_LVL 1
#define MAIN  2
#define LVL  3

class Menu{
public:
  Menu(){}
  void init(){
    m_option = PLAY;
    m_room = MAIN;
    m_level = 1;
  }
  void draw(Adafruit_ST7735& tft);
  bool updateMenu(Adafruit_ST7735& tft, byte button);
private:
  bool setLevel(byte level);

  byte m_option;
  byte m_room;
  byte m_level;
};

void Menu::draw(Adafruit_ST7735& tft){
  tft.setTextWrap(false);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);

  if(m_room == MAIN){
    if(m_option == PLAY){
      tft.fillRect(27, 25, 107, 25, LIGHT_GREY);
      tft.fillRect(27, 80, 107, 25, DARK_GREY);
    }else{
      tft.fillRect(27, 25, 107, 25, DARK_GREY);
      tft.fillRect(27, 80, 107, 25, LIGHT_GREY);
    }
    tft.setCursor(57, 30);
    tft.print("Play");
    
    tft.setCursor(29, 85);
    tft.print("Set Level");
  }else{
    tft.setTextColor(WHITE);
    tft.setCursor(30, 57);
    tft.print("Level: ");
    tft.print(m_level);
  }
}

bool Menu::updateMenu(Adafruit_ST7735& tft, byte button){
  switch(button){
    case 1: //up
      if(m_room != LVL && m_option != PLAY){
        m_option = PLAY;
        draw(tft);
      }
      break;
    case 2: //down
      if(m_room != LVL && m_option != SET_LVL){
        m_option = SET_LVL;
        draw(tft);
      }
      break;
    case 3: //left
      if(m_room == LVL){
        m_level > 1 ? m_level-- : 1;
        tft.fillRect(112, 56, 30, 16, BLACK);
        tft.fillRect(5, 120, 70, 8, BLACK);
        draw(tft);
      }
      break;
    case 4: //right
      if(m_room == LVL){
        m_level < 14 ? m_level++ : 14;
        tft.fillRect(112, 56, 30, 16, BLACK);
        tft.fillRect(5, 120, 70, 8, BLACK);
        draw(tft);
      }
      break;
    case 5: //A
      if(m_room == MAIN){
        if(m_option == PLAY){
          tft.fillScreen(BLACK);
          return true;
        }else{
          m_room = LVL;
          tft.fillScreen(BLACK);
          draw(tft);
        }
      }else{
        if(setLevel(m_level)){
           tft.setTextSize(1);
           tft.setTextColor(WHITE);
           tft.setCursor(5, 120);
           tft.print("Level set!");
        }
      }
      break;
    case 6: //B
      if(m_room == LVL){
        m_room = MAIN;
        m_option = PLAY;
        tft.fillScreen(BLACK);
        draw(tft);
      }
      break;
    default:
      break;
  }
  return false;
}

bool Menu::setLevel(byte level){
  SD.remove("currLvl");
  File file = SD.open("currLvl", FILE_WRITE);
  if(file){
    file.println(level);
    file.close();
    return true;
  }
  return false;
}

#endif
