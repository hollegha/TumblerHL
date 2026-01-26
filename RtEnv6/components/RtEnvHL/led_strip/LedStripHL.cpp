
#include "LedStripHL.h"

LedStripHL leds;

void MyDelay(int aMSec); // from RtEnv

void LedStripHL::Init()
{
  led_strip_config_t strip_config = {
      .strip_gpio_num = 15,
      .max_leds = 8,
      .led_model = LED_MODEL_WS2812,
      .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
      .flags = {
          .invert_out = false, // don't invert the output signal
      }
  };

  led_strip_rmt_config_t rmt_config = {
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = 10 * 1000 * 1000,
      .mem_block_symbols = 0,
      .flags = {
          .with_dma = 0,
      }
  };
  
  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &_ls));
}


void LedStripHL::blink(int from, int to, uint16_t aHue, int aN)
{
  for (int i = 0; i < aN; i++) {
    setHueFT(from, to, aHue); refresh();
    MyDelay(200);
    clear();
    MyDelay(200);
  }
}


void LedStripHL::setAmplitude(int i, float val)
{
  float MAX_INTEN = 200.0;

  if (val > 1.0)
    val = 1.0;
  int r, g, b;
  r = g = b = 0;
  if (val < 0.33)
    r = MAX_INTEN * val;
  else if (val < 0.66)
    b = MAX_INTEN * (val - 0.33);
  else
    g = MAX_INTEN * (val - 0.66);
  led_strip_set_pixel(_ls, i, r, g, b);
}
  

