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
#include "msgproc.h"
#include <stdint.h>
#include <string.h>


typedef enum{
    CmdType_Probe = 0,
    CmdType_Write = 1,
    CmdType_Read = 2,
}CmdType_t;


static const char magic_rx[] = {'T', 'H', '8', 'r'};
static const char magic_tx[] = {'T', 'H', '8', 't'};
static const char probe_info[] = "ESP8266 air controll";
extern volatile int temp_target;
extern int temp_curr;

static int cmd_write(void *buff_rx)
{
  int32_t value;
  memcpy(&value, buff_rx, sizeof(value));
  if(160 <= value && value <= 310){
    temp_target = value;
  }
  return 4;
}

int process_msg(void *buff_rx, void *buff_tx, int size_rx)
{
    if(memcmp(buff_rx, magic_rx, sizeof(magic_rx)) != 0){
        return -1;
    }
    int size_tx = 0;
    buff_rx += sizeof(magic_rx);
    size_rx -= sizeof(magic_rx);
    memcpy(buff_tx, magic_tx, sizeof(magic_tx));
    buff_tx += sizeof(magic_tx);
    size_tx += sizeof(magic_tx);
    switch((CmdType_t)((uint8_t *)buff_rx)[0]){
    case CmdType_Probe:
        memcpy(buff_tx, probe_info, sizeof(probe_info));
        size_tx += sizeof(probe_info);
        break;
    case CmdType_Write:
        cmd_write(buff_rx+1);
        break;
    case CmdType_Read:
        memcpy(buff_tx, &temp_curr, sizeof(temp_curr));
        size_tx += sizeof(temp_curr);
        break;
    }
    return size_tx;
}