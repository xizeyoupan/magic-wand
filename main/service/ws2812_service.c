#include "ws2812_service.h"

// WS2812 时序定义（单位：纳秒）
#define T0H 300          // 逻辑 0 高电平时间
#define T0L 900          // 逻辑 0 低电平时间
#define T1H 900          // 逻辑 1 高电平时间
#define T1L 300          // 逻辑 1 低电平时间
#define RESET_TIME 50000 // 复位信号

static const char *TAG = "WS2812_SERVICE";
uint32_t bg_color = COLOR_NONE;
extern user_config_t user_config;
SemaphoreHandle_t ws2812_mutex;

rmt_encoder_handle_t ws2812_encoder;
rmt_encoder_handle_t ws2812_reset_encoder;
rmt_bytes_encoder_config_t bytes_encoder_config = {
    .bit0 = {.duration0 = T0H / 100, .level0 = 1, .duration1 = T0L / 100, .level1 = 0}, // 逻辑 0
    .bit1 = {.duration0 = T1H / 100, .level0 = 1, .duration1 = T1L / 100, .level1 = 0}, // 逻辑 1
    .flags.msb_first = 1                                                                // 高位先发
};
rmt_copy_encoder_config_t bytes_reset_encoder_config = {};

// 复位信号
rmt_symbol_word_t reset_symbol = {
    .duration0 = RESET_TIME / 100,
    .level0 = 0,
    .duration1 = 0,
    .level1 = 0};
rmt_transmit_config_t tx_config = {
    .loop_count = 0 // 不循环
};

rmt_channel_handle_t led_chan = NULL;

static void ws2812_send_pixel(rmt_channel_handle_t *channel, uint32_t color)
{
    uint8_t pixel_data[3]; // WS2812 颜色顺序：GRB
    pixel_data[0] = (color >> 8) & 0xff;
    pixel_data[1] = (color >> 16) & 0xff;
    pixel_data[2] = (color >> 0) & 0xff;

    ESP_ERROR_CHECK(rmt_transmit(*channel, ws2812_encoder, pixel_data, sizeof(pixel_data), &tx_config));
    ESP_ERROR_CHECK(rmt_transmit(*channel, ws2812_reset_encoder, &reset_symbol, sizeof(reset_symbol), &tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(*channel, portMAX_DELAY));
}

void ws2812_init()
{
    ws2812_mutex = xSemaphoreCreateMutex();
    configASSERT(ws2812_mutex);

    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .gpio_num = user_config.ws2812_gpio_num,
        .mem_block_symbols = 64,           // increase the block size can make the LED less flickering
        .resolution_hz = 10 * 1000 * 1000, // 10 MHz（100 ns 分辨率）,
        .trans_queue_depth = 4,            // set the number of transactions that can be pending in the background
    };
    ESP_LOGI(TAG, "Create RMT TX channel");

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, &ws2812_encoder));
    ESP_ERROR_CHECK(rmt_new_copy_encoder(&bytes_reset_encoder_config, &ws2812_reset_encoder));

    ESP_LOGI(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK(rmt_enable(led_chan));
}

void set_bg_color(uint32_t color)
{
    xSemaphoreTake(ws2812_mutex, portMAX_DELAY);
    bg_color = color;
    ws2812_send_pixel(&led_chan, bg_color);
    xSemaphoreGive(ws2812_mutex);
}

void ws2812_blink(uint32_t color)
{
    xSemaphoreTake(ws2812_mutex, portMAX_DELAY);

    ws2812_send_pixel(&led_chan, COLOR_NONE);
    vTaskDelay(pdMS_TO_TICKS(250));

    ws2812_send_pixel(&led_chan, color);
    vTaskDelay(pdMS_TO_TICKS(250));

    ws2812_send_pixel(&led_chan, COLOR_NONE);
    vTaskDelay(pdMS_TO_TICKS(250));

    ws2812_send_pixel(&led_chan, bg_color);
    vTaskDelay(pdMS_TO_TICKS(250));

    xSemaphoreGive(ws2812_mutex);
}