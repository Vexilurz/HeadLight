/* 
 *    HeadLight by Vexilurz 
 *   -----------------------
 * e-mail:        vexilurz@gmail.com
 * Create date:   30.01.2018
 * 
 * One fast click - turn the light on the maximum
 * One more fast click - turn the light off
 * Click and hold - slowly rise the Brightness
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

// PWM support only on PB0 and PB1 (5 and 6 pins on chip)
const byte LEDpin = 1; // PB1 (6 pin on the chip)
const byte SWpin  = 2; // PB2 (7 pin on the chip)

// Comment this if you don't want to inverse Brightness logic
#define BrightInverted

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

#define MIN_BRIGHT 0   // On this Bright device go to sleep
#define MAX_BRIGHT 255 // Maximum PWM

// Increments
#define BRIGHT_INC_SLOW 1
#define BRIGHT_INC_MID  2
#define BRIGHT_INC_FAST 4

// After that threshold speed gonna MID increase
#define BRIGHT_MID_THRESHOLD  64
// After that threshold speed gonna FAST increase
#define BRIGHT_FAST_THRESHOLD 128

uint8_t Bright = MIN_BRIGHT;
uint8_t KeyState = 0;
uint8_t LastKeyState = 0;

void setup() {  
  pinMode(LEDpin, OUTPUT);
  pinMode(SWpin, INPUT_PULLUP);  
  MCUCR = ~_BV(SM0) & _BV(SM1); // Select "Power-down" sleep mode
  // Interrupt on the switch pressed calls InterruptMethod()
  attachPCINT(digitalPinToPCINT(SWpin), InterruptMethod, FALLING);   
  //interrupts();

  Bright = MIN_BRIGHT;
  SetBright();
  _delay_ms(500);
}

void ReadSwitchState() 
{
  KeyState = digitalRead(SWpin);
}

void loop() {
  ReadSwitchState();    
  if(KeyState != LastKeyState)
  { //Switch key change event
    CheckOneClick();
    LastKeyState = KeyState;
  }

  if(KeyState == LOW) // If switch pressed and holded:
  {      
    IncBright();      
    SetBright(); 
  }  
  
  _delay_ms(NEXT_CLICK_MS); 
      
  // If light is off, goto sleep.
  if (Bright == MIN_BRIGHT)
     Sleep();    
}

// Switch one short click check
void CheckOneClick()
{
  if (KeyState == LOW) // If button pressed:
  {
    _delay_ms(FAST_CLICK_MS);
    ReadSwitchState();
    if(KeyState != LOW) // If after FAST_CLICK_MS switch released
    {
      //Change flashlight state (ON/OFF)
      Bright = (Bright > MIN_BRIGHT) ? MIN_BRIGHT : MAX_BRIGHT;
      SetBright();          
    }
  }
}

void SetBright()
{
  #ifdef BrightInverted
    analogWrite(LEDpin, MAX_BRIGHT - Bright);                
  #else
    analogWrite(LEDpin, Bright);
  #endif
}

// Slowly increase Brightness
void IncBright()
{
  if(Bright < MAX_BRIGHT) 
  { //Increase light until MAX while switch pressed
    if      (Bright < BRIGHT_MID_THRESHOLD)
    {
      Bright += BRIGHT_INC_SLOW;                 
      _delay_ms(15); // Just slowly when in low Brightless increment zone
    }
    else if (Bright < BRIGHT_FAST_THRESHOLD)
      Bright += BRIGHT_INC_MID;             
    else if (MAX_BRIGHT - Bright > BRIGHT_INC_FAST)
      Bright += BRIGHT_INC_FAST;  
    else
      Bright = MAX_BRIGHT;             
  }
}

void InterruptMethod() { 
  if (Bright == MIN_BRIGHT) // if light is off
  { // wake up    
    noInterrupts();
    sleep_disable();  
    //interrupts();    
  }
}

void Sleep()
{
  interrupts();
  sleep_enable();  
  sleep_cpu();
}
