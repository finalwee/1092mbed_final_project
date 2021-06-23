#include "DigitalIn.h"
#include "PinNames.h"
#include "mbed.h"
#include "wifiSpec.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
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

static EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);

//Player 1: whose have the external button on D7 
InterruptIn button(USER_BUTTON);
DigitalIn input(D7);

//Player 2: use USER_BUTTON for the hit
//DigitalIn input(USER_BUTTON);

Thread thread;

UDPSocket socket;
Sensor _sensor(event_queue, input);
WIFI   _wifi(wifi, &_sensor, event_queue, &socket);

void reset() {
    event_queue.call(callback(&_sensor, &Sensor::Calibrate));
}

int main()
{
    printf("==========================================\n");
    printf("====Pikachu Volleyball(STM32 and WiFi)====\n");
    printf("==========================================\n");
    button.fall(&reset);
    event_queue.dispatch_forever();
    printf("\nDone\n");
}