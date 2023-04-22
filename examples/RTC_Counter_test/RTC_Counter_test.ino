/*
 STM32F4_RTC_Counter.ino - Arduino Library Example sketch for interacting 
  Created by Craig Wm. Versek, 2016-04-11
  Released into the public domain.
*/
#include "STM32F4_RTC_Counter.h"
#include "stm32f4xx_ll_rtc.h"

#define LED_PIN 13
#define LOOP_DELAY_MILLIS 250


#include <STM32RTC.h>
// Get the rtc object 
STM32RTC& rtc = STM32RTC::getInstance();

// Instantiate an RTC counter passing in the STM32RTC object
RTC_CounterClass RTC_counter;

bool led_state = HIGH;

void setup() {
  Serial.begin(9600);
  //setup the RTC
  // Select RTC clock source: LSI_CLOCK, LSE_CLOCK or HSE_CLOCK.
  // By default the LSI is selected as source.
  rtc.setClockSource(STM32RTC::LSE_CLOCK);
  rtc.setPrediv(0x00,0x7FFF); //this should maximize subsecond precision to 30.5us, at the expense of power
  rtc.begin(); // initialize RTC 24H format
  rtc.setEpoch(1682025898); // set the datetime to 2023-04-20-17:26:00
  // initialize the digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, led_state);   // turn the LED on (HIGH is the voltage level)
  delay(5000);
  RTC_counter.begin();
  RTC_counter.store(); //lock in a value for this moment
  int8_t  predivA;
  int16_t predivS;
  rtc.getPrediv(&predivA, &predivS);
  Serial.print("predivA=");Serial.println(predivA);
  Serial.print("predivS=");Serial.println(predivS);
}


int loop_counter = 0;
void loop() {
  Serial.print("loop: ");Serial.println(loop_counter);
  //-------------------------------------------------------
  // this block demonstrates how to use the low level API
  // to grab the highest resolution subsecond register
  // and to convert it into integer microseconds
  int8_t  predivA;
  int16_t predivS; //needed for calculation
  rtc.getPrediv(&predivA, &predivS);
  // NOTE: Do not use rtc.getSubSeconds() it is limited to millisecond precision
  uint32_t ss = LL_RTC_TIME_GetSubSecond(RTC); //get the raw SSR value
  uint64_t numMicros = predivS-ss;    // WARNING may overflow uint32
  numMicros *= 1000000;
  //the dividend should fit smaller type
  uint32_t micros = (uint32_t)(numMicros/(predivS-1));  // cast it down
  // theis calculation compute the seconds fraction as floating point
  float num = (predivS-ss);
  float ssfrac = num/(predivS-1);
  /-----------------------------------------
  //now export the value in all the possible ways
  Serial.print("    LL_RTC_TIME_GetSubSecond(RTC): "); Serial.println(LL_RTC_TIME_GetSubSecond(RTC));
  Serial.print("    micros:                        "); Serial.println(micros);  
  Serial.print("    ssfrac:                        "); Serial.println(ssfrac,10);  
  Serial.print("    rtc.getEpoch():                "); Serial.println(rtc.getEpoch()); 
  Serial.print("    rtc.getSubSeconds():           "); Serial.println(rtc.getSubSeconds()); 
  Serial.print("    _epochMicros_offset:           "); Serial.println(RTC_counter.__epochMicros_offset()); 
  Serial.print("    _epochMicros_stored:           "); Serial.println(RTC_counter.__epochMicros_stored()); 
  Serial.print("    get_micros_uint64:             "); Serial.println(RTC_counter.get_micros_uint64()); 
  Serial.print("    get_seconds_float32:           "); Serial.println(RTC_counter.get_seconds_float32(),10);
  Serial.print("    get_seconds_float64:           "); Serial.println(RTC_counter.get_seconds_float64(),10); 
  //pulse LED on-off each cycle
  digitalWrite(LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(LOOP_DELAY_MILLIS >> 1);
  digitalWrite(LED_PIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(LOOP_DELAY_MILLIS >> 1);
  loop_counter++;
  RTC_counter.store(); //lock in a value for this moment
}
