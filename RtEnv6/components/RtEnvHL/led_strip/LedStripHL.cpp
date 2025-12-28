
#include "LedStripHL.h"

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

