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
#include "config.h"
#include "dht11.h"
#include "relay.h"


void app_main()
{
    printf("ESP8266 air controll\n");
    relay_init();
    dht11_init();

    int temp_target = TEMP_TARGET_DEFAULT;
    int restart_count = 0;
    bool running = false;
    int64_t t, t0=0;
    DHT11_RawData data;
    while(1){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        int res = dht11_readdata(&data);
        if(res != 0){
            printf("err=%d, ", res);
        }
        int temp = dht11_temp(&data);
        int hum = dht11_hum(&data);
        t = esp_timer_get_time();
        printf("%dC, %d%%\n", temp, hum);
        if(res != 0){
            continue;
        }
        if(!running && (temp > (temp_target+TEMP_DELTA_POS))){
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
        if(running && (temp < (temp_target-TEMP_DELTA_NEG))){
            if(t-t0>=TIME_RUN_MIN){
                relay_set(0);
                running = false;
                int32_t time_run = (t - t0)/1000000;
                t0 = t;
                printf("compressor off, restart=%d, run=%ds\n", restart_count, time_run);
            }
        }
    }
}
