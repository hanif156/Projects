#ifndef SEND_DATA_H
#define SEND_DATA_H

#include "esp_err.h"
#include "esp_http_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void send_data(int water_level_data);
    esp_err_t client_post_handler(esp_http_client_event_handle_t evt);

#ifdef __cplusplus
}
#endif

#endif //SEND_DATA_H