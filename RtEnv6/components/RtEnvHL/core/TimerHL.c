
#include "TimerHL.h"
#include "esp_timer.h"



void GpIoInitOut2(uint64_t aBitMask)
{
  const gpio_config_t conf = {
    .pin_bit_mask = aBitMask,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
  };
  ESP_ERROR_CHECK(gpio_config(&conf));
}

PwmTim InitPwmTimer(int grpId, uint32_t resHz, uint32_t periodTicks)
{
	PwmTim hltim;
  hltim.period = periodTicks;
  mcpwm_timer_config_t tim_conf = {
    .group_id = grpId,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT, // GPTIMER_CLK_SRC_DEFAULT S2
    .resolution_hz = resHz,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    .period_ticks = periodTicks,
  };
  ESP_ERROR_CHECK(mcpwm_new_timer(&tim_conf, &(hltim.tim)));

  mcpwm_operator_config_t op_conf = {
    .group_id = grpId, // operator must be in the same group to the timer
  };
  ESP_ERROR_CHECK(mcpwm_new_operator(&op_conf, &(hltim.oper)));
  ESP_ERROR_CHECK(mcpwm_operator_connect_timer(hltim.oper, hltim.tim));
  return hltim;
}

PwmHL InitPwm(PwmTim hltim, int ioPin)
{
  PwmHL pwm;
  pwm.period = hltim.period;

  mcpwm_comparator_config_t comp_conf = {
    .flags.update_cmp_on_tez = true,
  };
  ESP_ERROR_CHECK(mcpwm_new_comparator(hltim.oper, &comp_conf, &(pwm.comp)));

  mcpwm_generator_config_t generator_config = {
    .gen_gpio_num = ioPin,
  };
  ESP_ERROR_CHECK(mcpwm_new_generator(hltim.oper, &generator_config, &(pwm.gen)));

  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(pwm.comp, 1000));

  // go high on counter empty
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(pwm.gen,
    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));

  // go low on compare threshold
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(pwm.gen,
    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, pwm.comp, MCPWM_GEN_ACTION_LOW)));

  return pwm;
}

void StartPwmTimer(PwmTim hltim)
{
  ESP_ERROR_CHECK(mcpwm_timer_enable(hltim.tim));
  ESP_ERROR_CHECK(mcpwm_timer_start_stop(hltim.tim, MCPWM_TIMER_START_NO_STOP));
}

void SetPw(PwmHL pwm, uint32_t width)
{
  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(pwm.comp, width));
}


void GpIoInitInterrupt( int aPinNum, gpio_isr_t aISR, void* aParam, 
                        gpio_int_type_t aEdge )
{
	static bool isInstalled = false;
  const gpio_config_t conf = {
    .pin_bit_mask = (1ULL << aPinNum),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = 1,
    .pull_down_en = 0,
    .intr_type = aEdge,
  };
  ESP_ERROR_CHECK(gpio_config(&conf));
  // gpio_set_intr_type(aPinNum, GPIO_INTR_POSEDGE);

  // ESP_INTR_FLAG_DEFAULT=0; ESP_INTR_FLAG_LEVEL1....
  if( !isInstalled ) {
  	gpio_install_isr_service(0); // nur 1x aufrufen
  	isInstalled=true;
  }

  gpio_isr_handler_add(aPinNum, aISR, aParam);
}

void GpIoInitForISR(int aPinNum)
{
  const gpio_config_t conf = {
    .pin_bit_mask = (1ULL << aPinNum),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = 1,
    .pull_down_en = 0,
    .intr_type = GPIO_INTR_POSEDGE,
  };
  ESP_ERROR_CHECK(gpio_config(&conf));
}


void GpTimSetPeriod(gptimer_handle_t aTim, uint64_t aPeriod)
{
  gptimer_alarm_config_t alarmConf = {
    .reload_count = 0,
    .alarm_count = aPeriod,
    .flags.auto_reload_on_alarm = true,
  };
  gptimer_set_alarm_action(aTim, &alarmConf);
}

gptimer_handle_t GpTimCreate(gptimer_alarm_cb_t aCB)
{
  gptimer_handle_t tim = NULL;
  gptimer_config_t conf = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000, // 1MHz, 1 tick=1us
  };
  gptimer_new_timer(&conf, &tim);

  gptimer_event_callbacks_t cbs = {
    .on_alarm = aCB,
  };
  gptimer_register_event_callbacks(tim, &cbs, 0);
  gptimer_enable(tim);
  return tim;
}


esp_timer_handle_t EspTimSetup(int aFrequ, esp_timer_cb_t aCB, 
  void* aArg, bool useISR)
{
  esp_timer_create_args_t args = {
    .callback = aCB,
    .arg = aArg,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "rt",
    .skip_unhandled_events = true
  };
  /* if (useISR) // geht in der Lib momentan nicht
    args.dispatch_method = ESP_TIMER_ISR; */
  esp_timer_handle_t htim;
  ESP_ERROR_CHECK(esp_timer_create(&args, &htim));
  ESP_ERROR_CHECK(esp_timer_start_periodic(htim, 1000000 / aFrequ));
  return htim;
}

static void task_trg_isr(void* arg)
{
  TaskHandle_t* tsk = (TaskHandle_t*)arg;
  BaseType_t flag = pdFALSE;
  vTaskNotifyGiveFromISR(*tsk, &flag);
  // portYIELD_FROM_ISR(flag);
}

esp_timer_handle_t TimerISR2Task(int aFrequ, TaskHandle_t* task)
{
  return EspTimSetup(aFrequ, task_trg_isr, task, true);
}

