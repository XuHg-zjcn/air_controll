/*************************************************************************
 * Copyright 2024 Xu Ruijun
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "config.h"
#include "dht11.h"
#include "relay.h"
#include "protocol.h"


volatile int temp_target = TEMP_TARGET_DEFAULT;
int temp_curr;

static void temperature_task(void *p)
{
    int restart_count = 0;
    bool running = false;
    int64_t t, t0=0;
    DHT11_RawData data;
    printf("temperate task start\n");
    while(1){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        int res = dht11_readdata(&data);
        if(res != 0){
            printf("err=%d, ", res);
        }
        temp_curr = dht11_temp(&data);
        int hum = dht11_hum(&data);
        t = esp_timer_get_time();
        printf("%dC, %d%%\n", temp_curr, hum);
        if(res != 0){
            continue;
        }
        if(!running && (temp_curr > (temp_target+TEMP_DELTA_POS))){
            if((restart_count==0 && t>=TIME_DELAY_BOOT*1000000) || \
               (restart_count!=0 && t-t0>=TIME_STOP_MIN*1000000)){
                relay_set(1);
                running = true;
                restart_count++;
                int32_t time_stop = (t - t0)/1000000;
                t0 = t;
                printf("compressor on, restart=%d, stop=%ds\n", restart_count, time_stop);
            }
        }
        if(running && (temp_curr < (temp_target-TEMP_DELTA_NEG))){
            if(t-t0>=TIME_RUN_MIN*1000000){
                relay_set(0);
                running = false;
                int32_t time_run = (t - t0)/1000000;
                t0 = t;
                printf("compressor off, restart=%d, run=%ds\n", restart_count, time_run);
            }
        }
    }
}

void app_main()
{
    printf("ESP8266 air controll\n");
    relay_init();
    dht11_init();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(&udp_task, "udp", 4096, NULL, 5, NULL);
    xTaskCreate(&temperature_task, "temperature", 4096, NULL, 10, NULL);
}
