/* 
 *    HeadLight by Vexilurz 
 *   -----------------------
 * e-mail:        vexilurz@gmail.com
 * Create date:   30.01.2018
 * 
 * One fast click - turn the light on the maximum
 * One more fast click - turn the light off
 * Click and hold - slowly rise the brightness
 * 
 * Idea from:                           https://geektimes.ru/post/255004/
 * For ATtiny13 support in Arduino IDE: https://github.com/MCUdude/MicroCore
 * For interrupts support:              https://github.com/NicoHood/PinChangeInterrupt
 * For program ATtiny13 via UNO:        https://drive.google.com/open?id=1-w3zYSKzwpRuDV7iiXSNdWmjPVqMFLKMCIAcvBqBYi4
 */

/*
 *               AtTiny13
 *                +-\/-+
 *  !RESET  PB5  1|    |8   VCC
 *          PB3  2|    |7   PB2  SCK         [SWITCH]
 *          PB4  3|    |6   PB1  MISO  PWM1  [LED]
 *          GND  4|    |5   PB0  MOSI  PWM0
 *                +----+
*/

// PWM support only PB0 and PB1 (5 and 6 pins on chip)
const byte ledPin = 1; // PB1 (6 pin on the chip)
const byte swPin  = 2; // PB2 (7 pin on the chip)

#include <avr/sleep.h> // Library for attiny sleep while headlight is off

// Interrupts libraries:
#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

//Less than this interpreted as fast click
#define FAST_CLICK_MS 500
//Delay between click repeat while SW pressed
#define NEXT_CLICK_MS 40

#define MIN_BRIGHT 255
#define MAX_BRIGHT 0

#define BRIGHT_INC255 1
#define BRIGHT_INC200 5
#define BRIGHT_INC100 10

void setup() {  
  pinMode(ledPin, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);
  //GIMSK |= _BV(PCIE); // Enable Pin Change Interrupts
  MCUCR = ~_BV(SM0) & _BV(SM1); // Select "Power-down" sleep mode
  attachPCINT(digitalPinToPCINT(swPin), InterruptMethod, FALLING); // Прерывание на кнопку
  interrupts();
}

uint8_t bright = MIN_BRIGHT;
uint8_t lastkey = 0;
uint8_t keydown = 0;
uint8_t wasEvent = LOW; // for "continue" word replace

void loop() {
  keydown = digitalRead(swPin);
  wasEvent = LOW;
  
  if(keydown != lastkey)
  { //Switch key change state
    CheckOneClick();
    lastkey = keydown;
  }

  if (wasEvent == LOW) // If no switch key change state event:
  {    
    if(keydown == LOW) // If switch pressed and holded:
    {      
      IncBright();      
      analogWrite(ledPin, bright);      
      if(bright == MIN_BRIGHT - BRIGHT_INC255)           
        _delay_ms(FAST_CLICK_MS); //5.5mA            
    }  
    _delay_ms(NEXT_CLICK_MS); //1.64mA
        
    //If light is off, goto sleep.
    if(bright==MIN_BRIGHT)
    {
      sleep(); //0.3uA
    }
  }
}

// Switch one short click check
void CheckOneClick()
{
  if((bright < MIN_BRIGHT) && (keydown == LOW)) // If light was turned on and button pressed:
  { 
    _delay_ms(FAST_CLICK_MS);
    if(digitalRead(swPin) != LOW) // If after FAST_CLICK_MS switch released
    { // Turn light off
      bright = MIN_BRIGHT;
      analogWrite(ledPin, bright);   
      wasEvent = HIGH;       
    }
  }
  else if((bright == MIN_BRIGHT) && (keydown == LOW)) // If light was NOT turned on and button pressed:
  { 
    _delay_ms(FAST_CLICK_MS);
    if(digitalRead(swPin) != LOW) // If after FAST_CLICK_MS switch released
    { // Turn MAX brightness (It was one short click, not holding)
      bright = MAX_BRIGHT;
      analogWrite(ledPin, bright);   
      wasEvent = HIGH;       
    }
  }
}

// Slowly increase brightness
void IncBright()
{
  if(bright > MAX_BRIGHT) 
  { //Increase light until MAX while SW pressed
    if(bright <= 100)
    {
      if(bright > BRIGHT_INC100)
        bright -= BRIGHT_INC100;              
      else
        bright = MAX_BRIGHT;              
    }
    else if(bright <= 200)
      bright -= BRIGHT_INC200;             
    else
      bright -= BRIGHT_INC255;             
  }
}

void InterruptMethod() { 
  if (bright == MIN_BRIGHT) // if light is off
  { // wake up    
    noInterrupts();
    sleep_disable();  
    interrupts();
  }
}

void sleep()
{
  sleep_enable();  
  sleep_cpu();
}
