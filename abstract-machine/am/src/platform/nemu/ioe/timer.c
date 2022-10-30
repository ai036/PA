#include <am.h>
#include <nemu.h>
#include <time.h>
#include <sys/time.h>
uint64_t init_time;

void __am_timer_init() {
    uint64_t low=inl(RTC_ADDR+4);
    uint64_t high=(uint64_t)inl(RTC_ADDR)<<32;
    init_time=low|high;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t low=inl(RTC_ADDR+4);
  uint64_t high=(uint64_t)inl(RTC_ADDR)<<32;
  uint64_t new_time=low|high;
  uptime->us = new_time-init_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
