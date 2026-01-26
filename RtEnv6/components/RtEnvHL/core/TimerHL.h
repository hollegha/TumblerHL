
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gptimer.h"
#include "esp_timer.h"

#ifdef __cplusplus
extern "C" {
#endif
  
void GpIoInitOut2(uint64_t pin_bit_mask);

typedef struct {
  mcpwm_timer_handle_t tim;
  mcpwm_oper_handle_t oper;
  uint32_t period; // Ticks
} PwmTim;

PwmTim InitPwmTimer(int grpId, uint32_t resHz, uint32_t periodTicks);

typedef struct {
  mcpwm_cmpr_handle_t comp;
  mcpwm_gen_handle_t gen;
  uint32_t period; // Ticks
} PwmHL;

PwmHL InitPwm(PwmTim hltim, int ioPin);

void StartPwmTimer(PwmTim hltim);

void SetPw(PwmHL pwm, uint32_t width);

inline void SetPwPercent(PwmHL pwm, float aVal)
{
  SetPw(pwm, aVal*(float)pwm.period);
}


gptimer_handle_t GpTimCreate(gptimer_alarm_cb_t aCB);

void GpTimSetPeriod(gptimer_handle_t aTim, uint64_t aPeriod);

inline void GpTimSetFrequ(gptimer_handle_t aTim, uint64_t aPeriod)
{
  GpTimSetPeriod(aTim, 1000000/aPeriod);
}


esp_timer_handle_t EspTimSetup(int aFrequ, esp_timer_cb_t aCB,
  void* aArg, bool useISR);

esp_timer_handle_t TimerISR2Task(int aFrequ, TaskHandle_t* task);


// GPIO_INTR_POSEDGE NEGEDGE ANYEDGE
void GpIoInitInterrupt( int aPinNum, gpio_isr_t aISR, void* aParam, 
                        gpio_int_type_t aEdge );

void GpIoInitForISR(int aPinNum);

#ifdef __cplusplus
}
#endif








