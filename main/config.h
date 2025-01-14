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
#ifndef CONFIG_H
#define CONFIG_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_NUM_DHT11 16
#define GPIO_NUM_RELAY 4

//unit: sec
#define TIME_DELAY_BOOT     180
#define TIME_RUN_MAX       7200
#define TIME_RUN_MIN        300
#define TIME_STOP_MIN       300

//unit: 0.1oC
#define TEMP_TARGET_DEFAULT 280
#define TEMP_DELTA_POS       10
#define TEMP_DELTA_NEG       10

#define delay_ms(x) vTaskDelay((x) / portTICK_PERIOD_MS)
#define delay_us(x) ets_delay_us(x)

#endif
