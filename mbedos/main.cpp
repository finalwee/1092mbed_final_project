/* WiFi Example
 * Copyright (c) 2016 ARM Limited
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
 */

#include "mbed.h"
#include "TCPSocket.h"
#include "netsocket/TCPServer.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include <cstdint>
#include <string>

#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
ISM43362Interface wifi(false);

#else

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif
#define SCALE_MULTIPLIER    0.004
static EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);
#define IP_address  "172.20.10.8"
#define Port_number 54087
#define SEND_INT    5
#define SAMPLE_RATE 2
#define PLAYER      1
#define SAMPLE_PERIOD 2
#define Rotation_threshold 1

InterruptIn button(USER_BUTTON);
DigitalIn input(D7);
SocketAddress addr(IP_address,Port_number);

class Sensor{
#define TimeStep  (float)SAMPLE_RATE / 1000
public:
    Sensor(events::EventQueue &event_queue):_event_queue(event_queue){
        BSP_ACCELERO_Init();    
        BSP_GYRO_Init();
        Calibrate();
        //_hit = 0;
        _event_queue.call_every(SAMPLE_RATE, this, &Sensor::update);
    }
    void Calibrate(){
        printf("Calibrating Sensors.....\n");
        int n = 0;
        for(int i = 0; i < 3; ++i){
            _AccOffset[i] = 0;
            _GyroOffset[i] = 0;
        }
        while(n < 2000){
            BSP_ACCELERO_AccGetXYZ(_pAccDataXYZ);
            BSP_GYRO_GetXYZ(_pGyroDataXYZ);
            for(int i = 0; i < 3; ++i){
                _AccOffset[i] += _pAccDataXYZ[i];
                _GyroOffset[i] += _pGyroDataXYZ[i];
            }
            ThisThread::sleep_for(SAMPLE_PERIOD);
            ++n;
        }
        for(int i = 0; i < 3; ++i){
            _AccOffset[i] /= n;
            _GyroOffset[i] /= n;
        }
        for (int i = 0; i < 3; ++i){
            printf("%d ", _AccOffset[i]);
            printf("%f ", _GyroOffset[i]);
        }
        printf("Done calibration\n");
    }

    void update(){
        BSP_GYRO_GetXYZ(_pGyroDataXYZ);
        rotation_distance += ((_pGyroDataXYZ[0] - _GyroOffset[0]) * SCALE_MULTIPLIER)*TimeStep; 
        ThisThread::sleep_for(SAMPLE_PERIOD);
    }

    void check_left_right(uint8_t& right, uint8_t& left) {
        BSP_ACCELERO_AccGetXYZ(_pAccDataXYZ);
        /*if((_pAccDataXYZ[0] - _AccOffset[0] - accumulate_x)*SCALE_MULTIPLIER > 0.7)
            left = 1;
        else if((_pAccDataXYZ[0] - _AccOffset[0] - accumulate_x)*SCALE_MULTIPLIER < -0.7)
            right = 1;*/
        if((_pAccDataXYZ[0] - _AccOffset[0])*SCALE_MULTIPLIER > 0.8)
            left = 1;
        if((_pAccDataXYZ[0] - _AccOffset[0])*SCALE_MULTIPLIER < -0.8)
            right = 1;
        accumulate_x = 0.8 * accumulate_x + 0.2 * _pAccDataXYZ[0];
    }

    void check_up_down(uint8_t& up, uint8_t& down) {
        //printf("Rotation distance = %f", rotation_distance);
        BSP_ACCELERO_AccGetXYZ(_pAccDataXYZ);
        if((_pAccDataXYZ[1] - _AccOffset[1] - accumulate_y)*SCALE_MULTIPLIER > 1.0)
            up = 1;
        if((_pAccDataXYZ[1] - _AccOffset[1] - accumulate_y)*SCALE_MULTIPLIER < -1.0)
            down = 1;
        accumulate_y = 0.9 * accumulate_y + 0.1 * _pAccDataXYZ[1];
    }

    void check_jump(uint8_t& jump) {
        if(rotation_distance > Rotation_threshold)
            jump = 1;
        rotation_distance = 0;
    }

    void getAction(uint8_t& right, uint8_t& left, uint8_t& up, uint8_t& down, uint8_t& hit, uint8_t& jump){
        check_left_right(right, left);
        check_up_down(up, down);
        check_jump(jump);
        if(input){
            _hit = 1;
        }
        if(_hit == 1) {
            check++;
        }
        hit = _hit;
        if(check == 10) {
            _hit = 0;
        }
        if(_hit == 0){
            check = 0;
        }
        
    }

private:
    events::EventQueue &_event_queue;
    int16_t _pAccDataXYZ[3] = {0};
    float _pGyroDataXYZ[3] = {0};

    float rotation_distance = 0;
    int accumulate_y = 0;
    int accumulate_x = 0;
    int8_t _hit = 0;
    int8_t check = 0;
    int   _AccOffset[3] = {};
    float _GyroOffset[3] = {};
};

class WIFI{
public:
    WIFI( NetworkInterface *wifi ,Sensor * sensor, events::EventQueue &event_queue, UDPSocket* socket ): _wifi(wifi),_sensor(sensor), _event_queue(event_queue), _led1(LED1, 1), _socket(socket) {
        connect();
    }
    void connect(){
        printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
        int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
        if (ret != 0) {
            printf("\nConnection error\n");
        }

        printf("Success\n\n");
        printf("MAC: %s\n", wifi.get_mac_address());
        printf("IP: %s\n", wifi.get_ip_address());
        printf("Netmask: %s\n", wifi.get_netmask());
        printf("Gateway: %s\n", wifi.get_gateway());
        printf("RSSI: %d\n\n", wifi.get_rssi());
        
        printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
        
        nsapi_error_t response;
        response = _socket->open(_wifi);
        if (0 != response){
            printf("Error opening: %d\n", response);
        }
        /*
        response = _socket->connect(addr);
    
        if (0 != response){
            printf("Error connecting: %d\n", response);
        }*/


        //_socket->set_blocking(1);
        _event_queue.call_every(SEND_INT, this, &WIFI::send_data);
        _event_queue.call_every(500, this, &WIFI::blink);
    }

    ~WIFI() {
        _socket->close();
        _wifi->disconnect();
    }

    void blink() {
        _led1 = !_led1;
    }


    void send_data() {
        char data[64];
        nsapi_error_t response;
        uint8_t right = 0, left = 0, up = 0, down = 0, hit = 0, jump = 0;
        _sensor->getAction(right, left, up, down, hit, jump);
        int len = sprintf(data,"{\"right\":%d,\"left\":%d,\"up\":%d,\"down\":%d,\"hit\":%d,\"jump\":%d}",right,
                                        left, up, down, hit, jump);
        printf("{\"right\":%d,\"left\":%d,\"up\":%d,\"down\":%d,\"hit\":%d,\"jump\":%d\n}",right,left, up, down, hit,jump);
        /*
        printf("Left: %d\n", left);
        printf("Right: %d\n", right);
        printf("Up: %d\n", up);
        printf("Down: %d\n", down);
        printf("Hit: %d\n", hit);*/
        response = _socket->sendto(addr,data,len);
        if (0 >= response){
            printf("Error seding: %d\n", response);
        }
    }
private:
    NetworkInterface *    _wifi;
    Sensor *              _sensor;
    DigitalOut            _led1;
    events::EventQueue    &_event_queue;
    UDPSocket*            _socket;
};


UDPSocket socket;
Sensor _sensor(event_queue);
WIFI   _wifi(&wifi, &_sensor, event_queue, &socket);

void reset() {
    event_queue.call(callback(&_sensor, &Sensor::Calibrate));
    event_queue.call(callback(&_wifi  , &WIFI::connect));
}

int main()
{
   printf("=========================================\n");
   printf("==Pikachu Volleyball(STM32 and WiFi)== \n");
   printf("=========================================\n");
   button.fall(&reset);
   event_queue.dispatch_forever();
   printf("\nDone\n");
}