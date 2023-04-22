/*
  STM32_RTC_Counter.cpp - Library for accurate timing
  Created by Craig Wm. Versek, 2023-04-20
 */

#include "STM32F4_RTC_Counter.h"
#include <Arduino.h>
#include <STM32RTC.h>
#include "stm32f4xx_ll_rtc.h"

/*******************************************************************************
  RTC_CounterClass
  
*******************************************************************************/

/**
* Constructor makes sure some things are set.
*/
RTC_CounterClass::RTC_CounterClass(void)
    : _predivA(1),
      _predivS(1),
      _epochMicros_offset(0),
      _epochMicros_stored(0)
  {};
  
//Configuration methods
void RTC_CounterClass::begin(){
    //fetch the prediv scalers for calculations
    STM32RTC& rtc = STM32RTC::getInstance(); //fetch singleton instance
    rtc.getPrediv(&_predivA, &_predivS);
    reset();
}

//Helper methods
uint32_t RTC_CounterClass::_convert_subSeconds_to_micros(uint32_t subSeconds){
    //convert into fractional microseconds
    //ref datasheet 26.6.11 RTC sub second register (RTC_SSR)
    //the numerator will be large, so promote to a large type
    uint64_t num = _predivS-subSeconds;
    num *= 1000000;  // WARNING may overflow uint32
    //the dividend should fit smaller type
    uint32_t micros = (uint32_t)(num/(_predivS-1));  // cast it back down
    return micros;
}
//Functionality methods
void RTC_CounterClass::reset(){
    STM32RTC& rtc = STM32RTC::getInstance();
    // CRITICAL SECTION --------------------------------------------------------
    // 1. back up interrupt state; `__get_PRIMASK()` returns 0 if interrupts
    // are **enabled**, and non-zero if they are **disabled**.
    bool interrupts_enabled = (__get_PRIMASK() == 0);
    // 2. Disable interrupts
    __disable_irq();
    //do atomic time sample
    // NOTE: Do not use rtc.getSubSeconds() it is limited to millisecond precision
    uint32_t ss = LL_RTC_TIME_GetSubSecond(RTC);  //get the raw SSR value
    uint64_t es = rtc.getEpoch();
    // 3. Restore backed-up-state
    if (interrupts_enabled) {
        __enable_irq();
    }
    // END CRITICAL SECTION ----------------------------------------------------
    _epochMicros_offset  =  _convert_subSeconds_to_micros(ss);
    _epochMicros_offset += 1000000*es;
    //set the stored values equal to the offset
    _epochMicros_stored = _epochMicros_offset;

}

void RTC_CounterClass::store(){
    //use to quickly mark the time for use in timestamps
    STM32RTC& rtc = STM32RTC::getInstance(); //fetch singleton instance
    // CRITICAL SECTION --------------------------------------------------------
    // 1. back up interrupt state; `__get_PRIMASK()` returns 0 if interrupts
    // are **enabled**, and non-zero if they are **disabled**.
    bool interrupts_enabled = (__get_PRIMASK() == 0);
    // 2. Disable interrupts
    __disable_irq();
    //do atomic time sample
    // NOTE: Do not use rtc.getSubSeconds() it is limited to millisecond precision
    uint32_t ss = LL_RTC_TIME_GetSubSecond(RTC);  //get the raw SSR value
    uint64_t es = rtc.getEpoch();
    // 3. Restore backed-up-state
    if (interrupts_enabled) {
        __enable_irq();
    }
    // END CRITICAL SECTION ----------------------------------------------------
    _epochMicros_stored  = _convert_subSeconds_to_micros(ss);
    _epochMicros_stored += 1000000*es;
}
  
float RTC_CounterClass::get_seconds_float32(){
    uint64_t micros_count = get_micros_uint64();
    float seconds = micros_count/1e6;
    return seconds;
}

double RTC_CounterClass::get_seconds_float64(){
    uint64_t micros_count = get_micros_uint64();
    double seconds = micros_count/1e6;
    return seconds;
}
 
uint64_t RTC_CounterClass::get_micros_uint64(){
    return _epochMicros_stored - _epochMicros_offset;
}
