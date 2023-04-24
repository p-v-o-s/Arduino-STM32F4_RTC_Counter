/*
  STM32_RTC_Counter.h - Library for accurate timing
  Created by Craig Wm. Versek, 2023-04-20
 */
#ifndef _STM32F4_RTC_COUNTER_H_INCLUDED
#define _STM32F4_RTC_COUNTER_H_INCLUDED
#include <Arduino.h>
#include <STM32RTC.h>

/*******************************************************************************
  RTC_CounterClass
  
*******************************************************************************/
class RTC_CounterClass{
public:
  //Constructor
  RTC_CounterClass(void);
  //Configuration methods
  void begin();
  //Functionality methods
  void reset();
  void store();
  uint64_t get_count_micros();
  uint64_t get_offset_epoch_micros();
  uint64_t get_stored_epoch_micros();
  uint64_t __epochMicros_offset(){return _epochMicros_offset;};
  uint64_t __epochMicros_stored(){return _epochMicros_stored;};
private:
  uint32_t _convert_subSeconds_to_micros(uint32_t subSeconds);
  //Attributes
  int8_t  _predivA;
  int16_t _predivS;
  uint64_t _epochMicros_offset;
  uint64_t _epochMicros_stored;
};
#endif /* _STM32F4_RTC_COUNTER_H_INCLUDED */
