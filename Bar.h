#ifndef Bar_h
#define Bar_h

#include "Arduino.h"
#include <LiquidCrystal.h>

class Bar
{
  public:
    Bar();
    void draw(LiquidCrystal lcd, byte startChar, byte endChar, byte row, int maxValue, int value);
    void init(LiquidCrystal lcd);
  private:
    char _charForCols[];
};

#endif
