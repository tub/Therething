#include "Arduino.h"
#include "Bar.h"

Bar::Bar()
{
}

/* 
 Set up custom characters
 on the LCD to allow display of bar graphs
 */
void Bar::init(LiquidCrystal lcd){
  byte barChar[8] = {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000
  };

  for(int i = 0; i < 4; i++){
    lcd.createChar(i, barChar);
    byte next = (barChar[0] >> 1) | B10000;
    for(int i = 0; i < 8; i++){
      barChar[i] = next;
    }
  }
}

void Bar::draw(LiquidCrystal lcd, byte startChar, byte endChar, byte row, int maxValue, int value){
  char _charForCols[] = {0x20, 0x00, 0x01, 0x02, 0x03, 0x04};
  byte barWidthChars = endChar - (startChar - 1);

  byte numCols = barWidthChars * 5 ;// 5 cols per char
  float colsPerStep = (float) numCols / (float) maxValue;
  numCols = (float)value * colsPerStep;

  byte wholeBlocks = numCols / 5;
  char lastChar = _charForCols[numCols % 5];

  lcd.setCursor(startChar, row);

  //write out whole blocks
  for(byte i = 0; i < wholeBlocks; i++){
    lcd.write(0xff);
  }
  lcd.write(lastChar);
  if(wholeBlocks < (barWidthChars - 1)){
    for(byte i = wholeBlocks + 2; i <= barWidthChars; i++){
      lcd.write(0x20);
    }
  }
}

