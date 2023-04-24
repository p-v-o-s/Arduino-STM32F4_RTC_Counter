/*
 STM32F4_RTC_Counter.ino - Arduino Library Example sketch for interacting 
  Created by Craig Wm. Versek, 2016-04-11
  Released into the public domain.
*/
#include "STM32F4_RTC_Counter.h"
#include <STM32RTC.h>

#define LED_PIN 13
#define LOOP_DELAY_MILLIS 250

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
  rtc.setEpoch(1682025898); // set the datetime to ~2023-04-20-17:26:00
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
  Serial.print("offset_epoch_micros=");Serial.println(RTC_counter.get_offset_epoch_micros());

}

int loop_counter = 0;
long long loopMicrosAccum = 0;
void loop() {
  long long u0 = micros();
  uint64_t count_micros = RTC_counter.get_count_micros();
  Serial.print("loop: ");Serial.println(loop_counter);
  //-----------------------------------------
  Serial.print("    rtc.getEpoch():                "); Serial.println(rtc.getEpoch()); 
  Serial.print("    rtc.getSubSeconds():           "); Serial.println(rtc.getSubSeconds()); 
  Serial.print("    stored_epoch_micros:           "); Serial.println(RTC_counter.get_stored_epoch_micros()); 
  Serial.print("    count_micros:                  "); Serial.println(count_micros);
  Serial.print("    accum_micros:                  "); Serial.println(loopMicrosAccum);
  Serial.print("    diff_micros:                   "); Serial.println(count_micros - loopMicrosAccum);

  // pulse LED on-off each cycle
  digitalWrite(LED_PIN, HIGH);      // turn the LED on (HIGH is the voltage level)
  delay(LOOP_DELAY_MILLIS >> 1);
  digitalWrite(LED_PIN, LOW);       // turn the LED on (HIGH is the voltage level)
  delay(LOOP_DELAY_MILLIS >> 1);
  loop_counter++;
  loopMicrosAccum += micros() - u0; // increment the accumulated delays
  RTC_counter.store();              // lock in a value for this moment
}
