#include "sensor.h"

class WIFI{
public:
    WIFI(WiFiInterface &wifi, Sensor * sensor, events::EventQueue &event_queue, UDPSocket* socket);
    ~WIFI();

    void connect_start();
    void send_data();
    void blink_alternately();

private:
    WiFiInterface         &_wifi;
    //NetworkInterface *   _wifi;
    Sensor *              _sensor;
    DigitalOut            _led1;
    DigitalOut            _led2;
    events::EventQueue    &_event_queue;
    UDPSocket*            _socket;
    bool                  flag = false;
    int                   count_reconnect = 0;
};