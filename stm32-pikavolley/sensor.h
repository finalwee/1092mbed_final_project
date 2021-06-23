#include "mbed.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"

class Sensor{

public:
    Sensor(events::EventQueue &event_queue, mbed::DigitalIn &input);

    void Calibrate();

    void update();

    void check_left_right(uint8_t& right, uint8_t& left);

    void check_up_down(uint8_t& up, uint8_t& down);

    void check_jump(uint8_t& jump);

    void getAction(uint8_t& right, uint8_t& left, uint8_t& up, uint8_t& down, uint8_t& hit, uint8_t& jump);

private:
    events::EventQueue  &_event_queue;
    mbed::DigitalIn     &input;
    int16_t             _pAccDataXYZ[3] = {0};
    float               _pGyroDataXYZ[3] = {0};

    float               rotation_distance = 0;
    int                 accumulate_y = 0;
    int                 accumulate_x = 0;
    //int8_t            _hit = 0;
    //int8_t            check = 0;
    int                 _AccOffset[3] = {};
    float               _GyroOffset[3] = {};
};
