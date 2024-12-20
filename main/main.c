#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/message_buffer.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "mdns.h"
#include "nvs_flash.h"
// #include <cstring>

#include "parameter.h"

#include "tftest.h"
#include "websocket_server.h"



#include "wand_server_task.h"

MessageBufferHandle_t xMessageBufferToClient;

static const char *TAG = "MAIN";

void app_main(void)
{

    // Initialize NVS
    ESP_LOGI(TAG, "Initialize NVS");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Start web server
    xTaskCreate(&wand_server_task, "SERVER", 1024 * 5, NULL, 5, NULL);

    // Initialize WiFi
    // start_wifi();

    // Initialize mDNS
    // start_mdns();

    // Initialize i2c
    // start_i2c();

    // xQueuePdata = xQueueCreate(10, sizeof(p_data));

    // // Create Queue
    // xQueueTrans = xQueueCreate(10, sizeof(POSE_t));
    // configASSERT(xQueueTrans);

    // // Create Message Buffer
    // xMessageBufferToClient = xMessageBufferCreate(1024);
    // configASSERT(xMessageBufferToClient);

    /* Get the local IP address */
    // esp_netif_ip_info_t ip_info;
    // ESP_ERROR_CHECK(esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info));
    // char cparam0[64];
    // sprintf(cparam0, IPSTR, IP2STR(&ip_info.ip));
    // ESP_LOGI(TAG, "cparam0=[%s]", cparam0);

    // Start web socket server
    // ws_server_start();

    // Start web server
    // xTaskCreate(&server_task, "SERVER", 1024*3, (void *)cparam0, 5, NULL);

    // Start web client
    // xTaskCreate(&client_task, "CLIENT", 1024*3, (void *)0x011, 5, NULL);

    // Start imu task
    // xTaskCreate(&mpu6050, "IMU", 1024 * 8, NULL, 5, NULL);

    // xTaskCreate(&start_test, "tesst", 1024 * 20, NULL, 5, NULL);

    // xTaskCreate(&wifi_sacn_loop, "wifi_scan", 1024 * 10, NULL, 5, NULL);

    // Start udp task
    // xTaskCreate(&udp_trans, "UDP", 1024*3, NULL, 5, NULL);

    vTaskDelay(100);
}
