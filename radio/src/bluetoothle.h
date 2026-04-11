/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
//Users-Manual-4123075.pdf foxware NB4
#ifndef BLUETOOTH_LE_H
#define BLUETOOTH_LE_H

enum BluetoothLeStates {
  BLUETOOTH_STATE_OFF,//compatybile with other interface
  BLUETOOTH_LE_STATE_BAUD_DETECT,
  BLUETOOTH_LE_STATE_REQUESTING_CONFIG,
  BLUETOOTH_LE_STATE_SAVING_CONFIG,
  BLUETOOTH_LE_STATE_READY,
  BLUETOOTH_STATE_CONNECTED, //compatybile with other interface
    BLUETOOTH_STATE_FACTORY_BAUDRATE_INIT,
    BLUETOOTH_STATE_BAUDRATE_SENT,
    BLUETOOTH_STATE_BAUDRATE_INIT,
    BLUETOOTH_STATE_NAME_SENT,
    BLUETOOTH_STATE_POWER_SENT,
    BLUETOOTH_STATE_ROLE_SENT,
    BLUETOOTH_STATE_IDLE,
    BLUETOOTH_STATE_DISCOVER_REQUESTED,
    BLUETOOTH_STATE_DISCOVER_SENT,
    BLUETOOTH_STATE_DISCOVER_START,
    BLUETOOTH_STATE_DISCOVER_END,
    BLUETOOTH_STATE_BIND_REQUESTED,
    BLUETOOTH_STATE_CONNECT_SENT,
    BLUETOOTH_STATE_DISCONNECTED,
    BLUETOOTH_STATE_CLEAR_REQUESTED,
    BLUETOOTH_STATE_FLASH_FIRMWARE
};

#define BT_FIFO_SIZE                  256

#define BLUETOOTH_LE_PACKET_SIZE      200
#define BLUETOOTH_LE_LINE_LENGTH      23
#define LEN_BLUETOOTH_ADDR            16
#define MAX_BLUETOOTH_DISTANT_ADDR    6
#define BLUETOOTH_TRAINER_PACKET_SIZE 14
#define BLUETOOTH_TRAINER_CHANNELS    8

#define SENSOR_HEADER_SIZE            1
#define SENSOR_CRC_SIZE               2
#define SENSOR_TAG_HEADER_SIZE        2
#define SENSOR_TAG                    0x5E
#define DATA_TAG                      0xDA

enum BLUETOOTH_TARGET_PLATFORM_TYPE {
  BLUETOOTH_TARGET_PLATFORM_FIRST = 0,
  BLUETOOTH_TARGET_PLATFORM_UNDEFINED = 0,
  BLUETOOTH_TARGET_PLATFORM_ANDROID = 1,
  BLUETOOTH_TARGET_PLATFORM_IOS = 2,
  BLUETOOTH_TARGET_PLATFORM_LAST = 2,
};

enum BT_SENSOR_MODE {
  BT_SENSOR_MODE_DEFINITIONS = SENSOR_TAG,
  BT_SENSOR_MODE_VALUES = DATA_TAG,
};

#if defined(LOG_BLUETOOTH)
  #define BLUETOOTH_TRACE(...)  \
    f_printf(&g_bluetoothFile, __VA_ARGS__); \
    TRACE_NOCRLF(__VA_ARGS__);
#else
#if defined(DEBUG_BLUETOOTH)
  #define BLUETOOTH_TRACE(...)  TRACE_NOCRLF(__VA_ARGS__);
  #define BLUETOOTH_TRACE_TIMESTAMP(f_, ...)  debugPrintf((TRACE_TIME_FORMAT f_), TRACE_TIME_VALUE, ##__VA_ARGS__)
#if defined(DEBUG_BLUETOOTH_VERBOSE)
  #define BLUETOOTH_TRACE_VERBOSE(...) TRACE_NOCRLF(__VA_ARGS__);
#else
  #define BLUETOOTH_TRACE_VERBOSE(...)
#endif
#else
  #define BLUETOOTH_TRACE(...)
  #define BLUETOOTH_TIMESTAMP(f_, ...)
  #define BLUETOOTH_TRACE_VERBOSE(...)
#endif
#endif

#include "lierda_bt.h"



class BluetoothLE
{
  public:
    BluetoothLE();
    void start();
    void stop();
    uint8_t read(uint8_t * data, uint8_t size, uint32_t timeout=1000/*ms*/);
    char * readline(bool error_reset = true);
    void write(const uint8_t * data, uint8_t length);
    void writeTelemetryPacket(const uint8_t * data, uint8_t length);
    void sendSensors();
    void writeString(const char * str);
    void forwardTelemetry(const uint8_t * data);
    //returns task delay in COOS ticks (ms / 2)
    void wakeup();
    void getStatus(char* buffer, size_t bufferSize);

    volatile uint8_t state;
    char localAddr[LEN_BLUETOOTH_ADDR+1];
    char distantAddr[LEN_BLUETOOTH_ADDR+1];
    struct btle::configFoxware config;
    
  protected:
    uint32_t send(uint8_t* frame, size_t cmd_size);
    void setState(enum BluetoothLeStates state);
    void pushByte(uint8_t * buffer, uint8_t byte, unsigned& index, uint8_t& crc);
    void appendTrainerByte(uint8_t data);
    void readline_(uint8_t * str, uint8_t length);
    void sendTrainer();
    void receiveTrainer();
    uint32_t handleConfiguration();

    uint8_t currentBaudrate;
    uint8_t currentMode;
    tmr10ms_t wakeupTime = 0;
    bool setBaudrateFromConfig;

    uint8_t bt_data[BLUETOOTH_LE_PACKET_SIZE+1];
    uint8_t rxDataState;

    unsigned rxIndex;
    BT_SENSOR_MODE sensorMode;
    size_t name_offset; //of TelemetrySensor
    unsigned sensorDefinitionsPerFrame;
};

extern BluetoothLE bluetooth;

int32_t getBtPlatfrom();
void setBtPlatform(int32_t platform);

int32_t getBtPasscode();
void setBtPasscode(int32_t passcode);

int32_t isPasscodeEnabled();
void setBtPasscodeEnabled(int32_t enabled);
void setBtTxPower(int32_t power);
void setBtBaudrate(int32_t baudIndex);
#endif
