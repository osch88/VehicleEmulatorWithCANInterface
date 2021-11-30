#include <chrono>
#include <thread>
#include <iostream>
#include "your_stuff.h"
// #include "can_opener.h"



void yourStuff::YouHaveJustRecievedACANFrame(const canfd_frame * const _frame) {
    static _icons icons = {0,0,0,0,0,0,0,0,0,0,0,0};
    /*icons.hazard=0;
    icons.left_blinker=0;
    icons.right_blinker=0;
    icons.abs=0;
    icons.battery=0;
    icons.doors_open=0;
    icons.engine_check=0;
    icons.hand_break=0;
    icons.high_beam=0;
    icons.oil_check=0;
    icons.seat_belt=0;
    icons._reserved_pad=0;*/
    // double us_rpm;

    switch (static_cast<int>(_frame->can_id)) {
        case 4: 
            {
            // from CAN bus
            uint16_t rpm = *((uint16_t*)(_frame->data));
            uint8_t speed = *((uint8_t*)(_frame->data+2));
            char gearPindle = _frame->data[3];
            char gearNumber = _frame->data[4];

            if(gearPindle == 3) {
                gearPindle = 2;
                gearNumber = 1;
            }
            else if(gearPindle == 2) {
                gearPindle = 3;
            }

            this->InstrumentCluster.setRPM(rpm);
            this->InstrumentCluster.setSpeed(speed);
            this->InstrumentCluster.setGear(gearNumber);
            this->InstrumentCluster.setGearPindle_int(gearPindle);
            
            // default values seperated from CAN
            
            this->InstrumentCluster.setFuelGauges(125);
            this->InstrumentCluster.setTemperatureGauges(150);
            this->InstrumentCluster.setOilTemperatureGauges(150);
            
            break;
            }
        case 1: 
            {
            static bool previous_hazard = false;
            if (_frame->data[2] == 0) {
                icons.right_blinker=0;
                icons.left_blinker=0;
                icons.hazard=0;
                previous_hazard=false;
                this->InstrumentCluster.setIcon(&icons);
            } else if (_frame->data[2] == 1){
                icons.right_blinker=0;
                icons.left_blinker=1;
                icons.hazard=0;
                previous_hazard=false;
                this->InstrumentCluster.setIcon(&icons);
            } else if (_frame->data[2] == 2){
                icons.right_blinker=1;
                icons.left_blinker=0;
                icons.hazard=0;
                previous_hazard=false;
                this->InstrumentCluster.setIcon(&icons);
            } else if (_frame->data[2] == 3 && previous_hazard == false){
                icons.right_blinker=0;
                icons.left_blinker=0;
                icons.hazard=1;
                previous_hazard=true;
                this->InstrumentCluster.setIcon(&icons);
            }

            if (_frame->data[0]==1){
                this->InstrumentCluster.ignite(1);
            } else {
                this->InstrumentCluster.ignite(0);
            }
            /*
            static bool start = false;
            double rpm =0;
            if ((!start) && (_frame->data[0] == 1)) {
                for (int i=0; i<90; i++) {
                    rpm +=100;
                    this->InstrumentCluster.setRPM(rpm);
                }
                for (int i=0; i<90; i++) {
                    rpm -=100;
                    this->InstrumentCluster.setRPM(rpm);
                }
                
            }*/
            break;
            }
        default:
            break;
    }
}
/*
    case CAN::MSG::GAUGES_ID: {
        const struct CAN::MSG::Gauges_t::_inner* s =
                reinterpret_cast<const struct CAN::MSG::Gauges_t::_inner* >((_frame->data));
        this->InstrumentCluster.setFuelGauges(s->G_FUEL);
        this->InstrumentCluster.setTemperatureGauges(s->G_TEMP);
        this->InstrumentCluster.setOilTemperatureGauges(s->G_OILT);
        break;
    }
    case CAN::MSG::ICONSS_ID:
        struct _icons  p;// = reinterpret_cast<struct _icons * >((_frame->data));
        static bool once = false;

        if (!once) {
            p.hazard = 1;
            once = true;
        } else
            p.hazard = 0;
        this->InstrumentCluster.setIcon(&p);
        break;
//    case CAN::MSG::USERIN_ID: {
//       // const struct CAN::MSG::_userin *d = reinterpret_cast<const struct CAN::MSG::_userin * >((_frame->data));
//       // this->InstrumentCluster.ignite(d->IGNT);
//    }
//        break;

    case CAN::MSG::GEARBX_ID: {
        const struct CAN::MSG::Gearbx_t::_bits *d = reinterpret_cast<const struct CAN::MSG::Gearbx_t::_bits * >((_frame->data));
        this->InstrumentCluster.setGearPindle_int(d->GEAR_P);
        this->InstrumentCluster.setGear(d->GEAR_N);
    }
        break;
    case CAN::MSG::ENGINE_ID: {
        const struct CAN::MSG::_engine *d = reinterpret_cast<const struct CAN::MSG::_engine * >((_frame->data));
        this->InstrumentCluster.ignite(d->RUN);
        this->InstrumentCluster.setRPM(d->RPM);
        this->InstrumentCluster.setSpeed(d->SPD);

    }
        break;
    default:
        break;
    }

} */

/******************************* ANYTHING BELOW THIS LINE IS JUST BORING STUFF *******************************/

yourStuff::yourStuff(const std::string &_ifName, QObject *_vs) {
    if(!(this->CANReader.open(_ifName))) exit(-1);//emit die();

    this->InstrumentCluster.init(_vs);

    this->startTimer(1);
}

bool yourStuff::run() {
    bool ret = true;
    CANOpener::ReadStatus status = CANOpener::ReadStatus::OK;
    canfd_frame frame;
    this->CANReader.read(&frame);
    /*while*/if (status == CANOpener::ReadStatus::OK) {
        this->YouHaveJustRecievedACANFrame(&frame);
    }
    if (status == CANOpener::ReadStatus::ERROR) ret = false;
    else if (status == CANOpener::ReadStatus::NAVAL ||
             status == CANOpener::ReadStatus::ENDOF) this->Counter++;
    else   this->Counter = 0;
    //if (this->Counter > 200) ret = false;
    return ret;
}

void yourStuff::timerEvent(QTimerEvent*) {
    if(this->run());
    else {
//    emit this->die();
        exit(-1);
    }

}

