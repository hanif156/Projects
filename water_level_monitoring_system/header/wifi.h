#ifndef WIFI_H
#define WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include <inttypes.h>

static const char* WIFI_TAG = "WIFI";

// Wi-Fi event handler function
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(WIFI_TAG, "Wi-Fi STA started, attempting to connect...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(WIFI_TAG, "Disconnected from Wi-Fi, attempting to reconnect...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
    }
    else
    {
        ESP_LOGI(WIFI_TAG, "Unhandled event base: %s, event id: %" PRIi32, event_base, event_id);
    }
}

// Function to initialize Wi-Fi in station mode
void wifi_init_sta(void)
{
    // Initialize NVS
    //Expected ESP_OK
    esp_err_t ret = nvs_flash_init(); 

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize TCP/IP stack
    printf("Netif init : %d\n", esp_netif_init());
    ESP_ERROR_CHECK(esp_netif_init());

    // Create default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default Wi-Fi station interface
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Initialize Wi-Fi with default configurations
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Configure Wi-Fi connection settings
    wifi_config_t wifi_config =
    {
        .sta =
        {
            .ssid = "Your_SSID",
            .password = "Your_Password",
        },
    };

    // Set Wi-Fi mode to station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Apply Wi-Fi configuration
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    // Register Wi-Fi event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    // Register IP event handler
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    // Start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished. Connecting to AP SSID: %s password: %s", wifi_config.sta.ssid, wifi_config.sta.password);
}

void wifi_task(void * pvParameters)
{
    ESP_LOGI(WIFI_TAG, "ESP_WIFI_MODE_STA starting...");
    wifi_init_sta();

    while(true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

#endif //WIFI_H
