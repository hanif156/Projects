#include<string>
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "esp_log.h"

static const char* DB_TAG = "DB";

extern "C"
{
    esp_err_t client_post_handler(esp_http_client_event_handle_t evt)
    {
        switch (evt->event_id)
        {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(DB_TAG, "Sending data");
            break;

        default:
            break;
        }
        return ESP_OK;
    }

    void send_data(int water_level_data)
    {
        esp_http_client_config_t config_post = 
        {
            .url = "http://192.168.1.5:8085/post",
            .method = HTTP_METHOD_POST,
            .event_handler = client_post_handler
        };
        esp_http_client_handle_t client_post = esp_http_client_init(&config_post);

        std::string water_level = std::to_string(water_level_data);
        std::string post_data = "{\"water_level_data\" : "+water_level+"}";

        const char *convert_post_data = post_data.c_str();
        esp_http_client_set_post_field(client_post, convert_post_data, strlen(convert_post_data));
        esp_http_client_set_header(client_post, "Content-Type", "application/json");

        esp_http_client_perform(client_post);
        esp_http_client_cleanup(client_post);
    }
}
