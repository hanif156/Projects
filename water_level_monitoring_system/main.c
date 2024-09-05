#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi.h"
#include "send_data.h"
#include "water_level_sensor.h"

TaskHandle_t wifi_task_handle = NULL;
TaskHandle_t water_level_sensor_handle = NULL;

void app_main(void)
{
    xTaskCreate(wifi_task, "WIFI", 4096, NULL, 5, &wifi_task_handle);
    xTaskCreate(water_level_sensor_task, "Water_Level", 4096, NULL, 1, &water_level_sensor_handle);
}
