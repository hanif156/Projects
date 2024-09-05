#ifndef WATER_LEVEL_SENSOR_H
#define WATER_LEVEL_SENSOR_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "send_data.h"

#define WATER_LEVEL_SENSOR_PIN ADC_CHANNEL_6  // GPIO34, corresponding to ADC1_CHANNEL_6
#define ADC_WIDTH ADC_BITWIDTH_12
#define ADC_ATTEN ADC_ATTEN_DB_11
#define UPPER_THRESHOLD 740
#define LOWER_THRESHOLD 600

static const char *WATER_TAG = "WATER_LEVEL_SENSOR";

adc_cali_handle_t adc_cali_setup(void)
{
    // ADC Calibration setup
    adc_cali_handle_t adc_cali_handle;
    adc_cali_line_fitting_config_t config_cali = 
    {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    esp_err_t result =  adc_cali_create_scheme_line_fitting(&config_cali, &adc_cali_handle);

    if(result != ESP_OK)
    {
        ESP_LOGE(WATER_TAG, "Failed to initialize calibration scheme");
        return NULL;
    }
    return adc_cali_handle;
}

adc_oneshot_unit_handle_t adc_config(void)
{
    // ADC Configuration
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    adc_oneshot_config_channel(adc_handle, WATER_LEVEL_SENSOR_PIN, &config);
    return adc_handle;
}

void water_level_reading(adc_cali_handle_t adc_cali_handle, adc_oneshot_unit_handle_t adc_handle)
{
    while (true)
    {
        int adc_reading = 0;
        adc_oneshot_read(adc_handle, WATER_LEVEL_SENSOR_PIN, &adc_reading);

        // Convert ADC reading to voltage
        int voltage;
        esp_err_t result = adc_cali_raw_to_voltage(adc_cali_handle, adc_reading, &voltage);

        if (result == ESP_OK) {
            ESP_LOGI(WATER_TAG, "Raw: %d, Voltage: %dmV", adc_reading, voltage);
        } else {
            ESP_LOGE(WATER_TAG, "Failed to convert raw ADC to voltage");
        }

        if(adc_reading >= UPPER_THRESHOLD)
        {
            ESP_LOGE(WATER_TAG, "Water at Critical LeveL!!");
        }
        else if(adc_reading > LOWER_THRESHOLD && adc_reading < UPPER_THRESHOLD)
        {
            ESP_LOGI(WATER_TAG, "Water Level increasing. Please be caustious");
        }
        else if(adc_reading < LOWER_THRESHOLD)
        {
            ESP_LOGI(WATER_TAG, "Water level normal");
        }

        //Send data to database
        send_data(adc_reading);

        vTaskDelay(pdMS_TO_TICKS(5000));  // Delay for 5 second
    }
}

void water_level_sensor_task(void * pvParameters)
{
    adc_cali_handle_t adc_cali_handle =  adc_cali_setup();
    adc_oneshot_unit_handle_t adc_handle =  adc_config();

    water_level_reading(adc_cali_handle, adc_handle);
}

#endif //WATER_LEVEL_SENSOR_H