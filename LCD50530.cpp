#include "LCD50530.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
	
LCD50530::LCD50530(uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
	init(ioc1, ioc2, rw, ex, d4, d5, d6, d7);
}

void LCD50530::init(uint8_t ioc1, uint8_t ioc2, uint8_t rw, uint8_t ex, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  _ioc1_pin = ioc1;
  _ioc2_pin = ioc2;
  _rw_pin = rw;
  _ex_pin = ex;
  
  _data_pins[0] = d4;
  _data_pins[1] = d5;
  _data_pins[2] = d6;
  _data_pins[3] = d7; 

  DDRD = B11111111; // Default: Set all ports to output (Atmel shortcut)    
}

void LCD50530::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {  
  int totalChar = cols*lines;

  uint8_t displaySize = 0x00;
  
  if(lines <= 1)
	displaySize |= LCD_1LINE;
  else if (lines == 2)
	displaySize |= LCD_2LINE;
  else 
	displaySize |= LCD_4LINE;
  
  if(totalChar <= 160) {
	displaySize |= LCD_160CHAR;
	_cols = 160/lines;
  }
  else if(totalChar <= 192) {
	displaySize |= LCD_192CHAR;	
	_cols = 192/lines;	
  }
  else if(totalChar <= 224) {
	displaySize |= LCD_224CHAR; 
	_cols = 224/lines;	
  }
  else{ // 256 characters - 
	displaySize |= LCD_256CHAR;
	_cols = 256/lines;
  }	
  
  _functionmode = LCD_4BITMODE | LCD_5x8DOTS | displaySize;
  command(LCD_FUNCTIONMODESET | _functionmode); 
  
  _displaymode = LCD_DISPLAYON;
  command(LCD_DISPLAYMODESET | _displaymode);

  _entrymode = LCD_CURSORMOVEWRITE | LCD_CURSORMOVERIGHT;
  command(LCD_ENTRYMODESET | _entrymode); 
  
  command(LCD_CLEARDISPLAY);
}

/********** high level commands, for the user! */
void LCD50530::clear()
{
  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
}

void LCD50530::home()
{
  command(LCD_RETURNHOME);  // set cursor position to zero
}

void LCD50530::setCursor(uint8_t col, uint8_t row)
{
  uint8_t instruction = row*_cols+col;
  send(instruction, 1<<_ioc1_pin | 1<<_ioc2_pin);
}

// Turn the display on/off (quickly)
void LCD50530::noDisplay() {
  _displaymode &= ~LCD_DISPLAYON;
  command(LCD_DISPLAYMODESET | _displaymode);
}
void LCD50530::display() {
  _displaymode |= LCD_DISPLAYON;
  command(LCD_DISPLAYMODESET | _displaymode);
}

// Turns the underline cursor on/off
void LCD50530::noCursor() {
  _displaymode &= ~LCD_CURSORON;
  command(LCD_DISPLAYMODESET | _displaymode);
}
void LCD50530::cursor() {
  _displaymode |= LCD_CURSORON;
  command(LCD_DISPLAYMODESET | _displaymode);
}

// Turns the underline cursor on/off
void LCD50530::noUnderline() {
  _displaymode &= ~LCD_UNDERLINEON;
  command(LCD_DISPLAYMODESET | _displaymode);
}
void LCD50530::underline() {
  _displaymode |= LCD_UNDERLINEON;
  command(LCD_DISPLAYMODESET | _displaymode);
}
// Turn on and off the blinking cursor
void LCD50530::noCursorBlink() {
  _displaymode &= ~LCD_CURSORBLINKON;
  command(LCD_DISPLAYMODESET | _displaymode);
}
void LCD50530::cursorBlink() {
  _displaymode |= LCD_CURSORBLINKON;
  command(LCD_DISPLAYMODESET | _displaymode);
}

// Turn on and off the blinking cursor
void LCD50530::noCharBlink() {
  _displaymode &= ~LCD_CHRBLINKON;
  command(LCD_DISPLAYMODESET | _displaymode);
}
void LCD50530::charBlink() {
  _displaymode |= LCD_CHRBLINKON;
  command(LCD_DISPLAYMODESET | _displaymode);
}

// These commands moves the cursor one space in the directopn without changing the RAM
void LCD50530::shiftCursor(uint8_t direction) {    
  command(LCD_SHIFTCOMMAND | LCD_CURSORSHIFT | (direction<<2));
}

// These commands moves the display one space in the directopn without changing the RAM
void LCD50530::shiftDisplay(uint8_t direction) {
  command(LCD_SHIFTCOMMAND | LCD_DISPLAYSHIFT | direction);
}
// This is for text that flows Left to Right
void LCD50530::moveCursorAfterInstruction(uint8_t direction, uint8_t instruction) {
  _entrymode &= B01000111;
  _entrymode |= ((instruction | direction) <<3);
  command(LCD_ENTRYMODESET | _entrymode);
}

void LCD50530::moveDisplayAfterInstruction(uint8_t direction, uint8_t instruction) {
  _entrymode &= B01111000;
  _entrymode |= instruction | direction;
  command(LCD_ENTRYMODESET | _entrymode);
}

void LCD50530::holdCursor(void) {
	command(LCD_ENTRYMODESET & B01000111);
}

void LCD50530::holdDisplay(void) {
	command(LCD_ENTRYMODESET & B01111000);
}

// This is for text that flows Left to Right
void LCD50530::leftToRight(void) {
  moveCursorAfterInstruction(LCD_WRITE, LCD_RIGHT);
}

// This is for text that flows Right to Left
void LCD50530::rightToLeft(void) {
	moveCursorAfterInstruction(LCD_WRITE, LCD_LEFT);
}

// This will 'right justify' text from the cursor
void LCD50530::autoscroll(void) {
  moveDisplayAfterInstruction(LCD_WRITE, LCD_RIGHT);
}

// This will 'left justify' text from the cursor
void LCD50530::noAutoscroll(void) {
  moveDisplayAfterInstruction(LCD_WRITE, LCD_LEFT);
}

/*********** mid level commands, for sending data/cmds */

inline void LCD50530::command(uint8_t value) {
  send(value, 0); 
}

void LCD50530::writechar(uint8_t value) {
  send(value, 1<<_ioc2_pin);  
}

/************ low level data pushing commands **********/

// write either command or data, with automatic 4/8-bit selection
void LCD50530::send(uint8_t value, uint8_t controlpins) {
  while(busyState());	// Wait while the lcd controller is still busy       
  write4bits(value, controlpins);
  write4bits(value<<4, controlpins);
}

void LCD50530::pulseExecute(void) {
  digitalWrite(_ex_pin, HIGH);
  delayMicroseconds(2);
  digitalWrite(_ex_pin, LOW);
  delayMicroseconds(2);
}

void LCD50530::write4bits(uint8_t value, uint8_t controlpins) {
  PORTD = (value & B11110000) | controlpins;
  pulseExecute();
}

/*
	Returns the state of the lcd controller
	When 0 is returned, the controller is ready for a new instruction
*/
uint8_t LCD50530::busyState() {
  uint8_t state = 0;
  DDRD = B00001111; // Set data pins to input
  PORTD = 1<<_rw_pin;
  
  digitalWrite(_ex_pin, HIGH);
  delayMicroseconds(2);
  state = digitalRead(_data_pins[3]);
  digitalWrite(_ex_pin, LOW);
  delayMicroseconds(2);
  
  PORTD = 1<<_rw_pin;
  
  digitalWrite(_ex_pin, HIGH);
  delayMicroseconds(2);
  state |= digitalRead(_data_pins[3]);
  digitalWrite(_ex_pin, LOW);
  delayMicroseconds(2);
  
  DDRD = B11111111; // Reset pins to output
  return state;
}
