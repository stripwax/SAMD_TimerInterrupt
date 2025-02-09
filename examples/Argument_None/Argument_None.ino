/****************************************************************************************************************************
  Argument_None.ino
  For SAMD boards
  Written by Khoi Hoang
  
  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_TimerInterrupt
  Licensed under MIT license
  
  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one SAMD timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

/*
   Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.
*/

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
      || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
      || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) \
      || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD51__) || defined(__SAMD51J20A__) \
      || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__)  \
      || defined(__SAMD21E15A__) || defined(__SAMD21E16A__) || defined(__SAMD21E17A__) || defined(__SAMD21E18A__) \
      || defined(__SAMD21G15A__) || defined(__SAMD21G16A__) || defined(__SAMD21G17A__) || defined(__SAMD21G18A__) \
      || defined(__SAMD21J15A__) || defined(__SAMD21J16A__) || defined(__SAMD21J17A__) || defined(__SAMD21J18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "SAMDTimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "SAMDTimerInterrupt.h"

#ifndef LED_BUILTIN
  #define LED_BUILTIN       13
#endif

#ifndef LED_BLUE
  #define LED_BLUE          2
#endif

#ifndef LED_RED
  #define LED_RED           8
#endif

#define TIMER0_INTERVAL_MS        500   //1000

volatile uint32_t preMillisTimer0 = 0;

// Depending on the board, you can select SAMD21 Hardware Timer from TC3-TCC
// SAMD21 Hardware Timer from TC3 or TCC
// SAMD51 Hardware Timer only TC3

// Init SAMD timer TIMER_TC3
SAMDTimer ITimer0(TIMER_TC3);
  
void TimerHandler0()
{
  static bool toggle0 = false;
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(LED_BUILTIN, OUTPUT);
  }

#if (TIMER_INTERRUPT_DEBUG > 0)
    static uint32_t curMillis = 0;
    
    curMillis = millis();
    
    if (curMillis > TIMER0_INTERVAL_MS)
    {
      Serial.print(F("ITimer0: millis() = ")); Serial.print(curMillis);
      Serial.print(F(", delta = ")); Serial.println(curMillis - preMillisTimer0);
    }
    
    preMillisTimer0 = curMillis;
#endif 

  //timer interrupt toggles pin LED_BUILTIN
  digitalWrite(LED_BUILTIN, toggle0);
  toggle0 = !toggle0;
}

#if (TIMER_INTERRUPT_USING_SAMD21)

#define TIMER1_INTERVAL_MS        2000

volatile uint32_t preMillisTimer1 = 0;

// Init SAMD timer TIMER_TCC
SAMDTimer ITimer1(TIMER_TCC);

void TimerHandler1()
{
  static bool toggle1 = false;
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(LED_BLUE, OUTPUT);
  }

  #if (TIMER_INTERRUPT_DEBUG > 0)
    static uint32_t curMillis = 0;
    
    curMillis = millis();
    
    if (curMillis > TIMER1_INTERVAL_MS)
    {
      Serial.print(F("ITimer1: millis() = ")); Serial.print(curMillis);
      Serial.print(F(", delta = ")); Serial.println(curMillis - preMillisTimer1);
    }
    
    preMillisTimer0 = curMillis;
#endif

  //timer interrupt toggles outputPin
  digitalWrite(LED_BLUE, toggle1);
  toggle1 = !toggle1;
}
#endif

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial);

  delay(100);

  Serial.print(F("\nStarting Argument_None on ")); Serial.println(BOARD_NAME);
  Serial.println(SAMD_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = ")); Serial.print(F_CPU / 1000000); Serial.println(F(" MHz"));
  
  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    preMillisTimer0 = millis();
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(preMillisTimer0);
  }
  else
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));

#if (TIMER_INTERRUPT_USING_SAMD21)
  // Interval in microsecs
  if (ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, TimerHandler1))
  {
    preMillisTimer1 = millis();
    Serial.print(F("Starting ITimer1 OK, millis() = ")); Serial.println(preMillisTimer1);
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
#endif
}

void loop()
{

}
