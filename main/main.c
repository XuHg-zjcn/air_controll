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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht11.h"


void app_main()
{
    printf("ESP8266 air controll\n");
    dht11_init();

    DHT11_RawData data;
    while(1){
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        int res = dht11_readdata(&data);
        if(res != 0){
            printf("err=%d, ", res);
        }
        printf("%dC, %d%%\n", dht11_temp(&data), dht11_hum(&data));
    }
}
