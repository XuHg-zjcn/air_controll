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
#include "dht11.h"
#include "driver/gpio.h"
#include "config.h"
#include "esp_timer.h"

void dht11_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT_OD;
    io_conf.pin_bit_mask = 1ULL<<GPIO_NUM_DHT11;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    gpio_set_level(GPIO_NUM_DHT11, 1);
}

static uint32_t gpio_wait_with_timeout(gpio_num_t num, int state, uint32_t timeout)
{
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = t0;
    int64_t tmax = t0 + timeout;
    do{
        if(gpio_get_level(num) == state){
            break;
        }
    }while((t1 = esp_timer_get_time()) < tmax);
    return t1 - t0;
}

int dht11_readdata(DHT11_RawData *data)
{
    gpio_set_direction(GPIO_NUM_DHT11, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(GPIO_NUM_DHT11, 0);
    delay_ms(20);
    gpio_set_level(GPIO_NUM_DHT11, 1);
    gpio_set_direction(GPIO_NUM_DHT11, GPIO_MODE_INPUT);
    uint32_t t;
    t = gpio_wait_with_timeout(GPIO_NUM_DHT11, 0, 100);
    if(t >= 100){
        return 1;
    }
    t = gpio_wait_with_timeout(GPIO_NUM_DHT11, 1, 120);
    if(t <= 50 || t >= 120){
        return 2;
    }
    t = gpio_wait_with_timeout(GPIO_NUM_DHT11, 0, 120);
    if(t <= 50 || t >= 120){
        return 2;
    }

    uint8_t byte = 0;
    for(int i=0;i<40;i++){
        t = gpio_wait_with_timeout(GPIO_NUM_DHT11, 1, 100);
        if(t <= 30 || t >= 100){
            return 3;
        }
        t = gpio_wait_with_timeout(GPIO_NUM_DHT11, 0, 100);
        if(t < 10 || t >= 100){
            return 3;
        }
        if(t > 47){
            byte |= 1ULL << (7-(i%8));
        }
        if(i%8 == 7){
            data->arr[i/8] = byte;
            byte = 0;
        }
    }
    t = gpio_wait_with_timeout(GPIO_NUM_DHT11, 1, 100);
    if(t <= 30 || t >= 100){
        return 3;
    }

    byte = 0;
    for(int i=0;i<4;i++){
        byte += data->arr[i];
    }
    if((byte&0xff) == data->stru.chksum){
        return 0;
    }else{
        return 4;
    }
}

int dht11_hum(DHT11_RawData *data)
{
    return (data->stru.humH*10) + (data->stru.humL&0x0f);
}

int dht11_temp(DHT11_RawData *data)
{
    int tmp = (data->stru.tmpH*10) + (data->stru.tmpL&0x0f);
    if(data->stru.tmpL & 0x80){
        return -tmp;
    }else{
        return tmp;
    }
}
