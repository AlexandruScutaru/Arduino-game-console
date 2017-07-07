#ifndef TILESET_H
#define TILESET_H

#include <SD.h>
#include <Adafruit_ST7735.h>

#define BUFFPIXEL 20
#define SD_CS      4

class Tiles{
public:
  Tiles(){}
  bool init();
  void drawBlock(char* filename, uint8_t x, uint8_t y, Adafruit_ST7735& tft);
  void drawPlayer(byte sprite, uint8_t x, uint8_t y, Adafruit_ST7735& tft);
private:
  void drawTile(char *filename, uint8_t x, uint8_t y, Adafruit_ST7735& tft);
  
  uint16_t read16(File f);
  uint32_t read32(File f);
};

bool Tiles::init(){
  if (!SD.begin(SD_CS)) {
    Serial.println("Failed to initialize SD card!");
    return false;
  }
  return true;
}

void Tiles::drawBlock(char* filename, uint8_t x, uint8_t y, Adafruit_ST7735& tft){
  drawTile(filename, x, y, tft);
}

void Tiles::drawPlayer(byte sprite, uint8_t x, uint8_t y, Adafruit_ST7735& tft){
  if(sprite & B10000){
    if(sprite & B01000)
      drawTile("push_N.bmp", x, y, tft);
    else if(sprite & B00100)
      drawTile("push_E.bmp", x, y, tft);
    else if(sprite & B00010)
      drawTile("push_S.bmp", x, y, tft);
    else if(sprite & B00001)
      drawTile("push_W.bmp", x, y, tft); 
  }else{
    if(sprite & B01000)
      drawTile("idle_N.bmp", x, y, tft);
    else if(sprite & B00100)
      drawTile("idle_E.bmp", x, y, tft);
    else if(sprite & B00010)
      drawTile("idle_S.bmp", x, y, tft);
    else if(sprite & B00001)
      drawTile("idle_W.bmp", x, y, tft); 
  }
}

void Tiles::drawTile(char *filename, uint8_t x, uint8_t y, Adafruit_ST7735& tft){
  File     bmpFile;
  int      bmpWidth, bmpHeight;
  uint8_t  bmpDepth;
  uint32_t bmpImageoffset;
  uint32_t rowSize;
  uint8_t  sdbuffer[3*BUFFPIXEL];
  uint8_t  buffidx = sizeof(sdbuffer);
  boolean  goodBmp = false;
  boolean  flip    = true;
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0;

  if((x >= tft.width()) || (y >= tft.height())) return;
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File '");
    Serial.print(filename);
    Serial.println("' not found!");
    return;
  }
  if(read16(bmpFile) == 0x4D42) {
    read32(bmpFile);
    (void)read32(bmpFile);
    bmpImageoffset = read32(bmpFile);
    read32(bmpFile);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) {
      bmpDepth = read16(bmpFile);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) {
        goodBmp = true; 
        rowSize = (bmpWidth * 3 + 3) & ~3;
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) {
          if(flip)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) {
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);
          }

          for (col=0; col<w; col++) { 
            if (buffidx >= sizeof(sdbuffer)) {
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

uint16_t Tiles::read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t Tiles::read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

#endif
