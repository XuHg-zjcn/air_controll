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
#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

#pragma pack(push)
#pragma pack(1)
typedef union {
    struct{
        uint8_t humH;
        uint8_t humL;
        uint8_t tmpH;
        uint8_t tmpL;
        uint8_t chksum;
    } stru;
    uint8_t arr[5];
}DHT11_RawData;
#pragma pack(pop)

void dht11_init();
int dht11_readdata(DHT11_RawData *data);
int dht11_hum(DHT11_RawData *data);
int dht11_temp(DHT11_RawData *data);

#endif
