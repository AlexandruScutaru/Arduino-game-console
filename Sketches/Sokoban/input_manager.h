#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
                        
#include <Arduino.h>

class InputManager{
public:
  byte getButton(){
    int val = analogRead(0);
    return val >= 837 && val <= 838 ? 1 :
           val >= 733 && val <= 736 ? 2 :
           val >= 609 && val <= 610 ? 3 :
           val >= 316 && val <= 318 ? 4 :
           val >= 174 && val <= 178 ? 5 :
           val >=  89 && val <=  91 ? 6 :
           val >= 671 && val <= 674 ? 34 : 0;
  }
 
};

#endif
