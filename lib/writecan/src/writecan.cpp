#include "socketcan.hpp"
#include "writecan.hpp"
#include <iostream>
#include <thread>
#include <chrono>


bool WriteUserInputToCan(SocketCan &socket, database_type::Database &db, const int &msdelay){
    bool ret = true;

    can_data_base::StartButton cb_ignition; 
    database_type::Ignition db_ignition = db.ignition; 
    const CanFrame ignition = ConvertToCanFrame(db_ignition, cb_ignition);

    can_data_base::GearPosition cb_gear; 
    database_type::Gear db_gear = db.gear; 
    const CanFrame gear = ConvertToCanFrame(db_gear, cb_gear);

    can_data_base::PedalPosition cb_gas; 
    unsigned int db_gas = db.gas; 
    const CanFrame gas = ConvertToCanFrame(db_gas, cb_gas);

    auto write_ignition_status = socket.WriteToCan(ignition);
    std::this_thread::sleep_for(std::chrono::milliseconds(msdelay)); //delay for frame
    auto write_gear_status = socket.WriteToCan(gear);
    std::this_thread::sleep_for(std::chrono::milliseconds(msdelay)); //delay for frame
    auto write_gas_status = socket.WriteToCan(gas);
    
    if (write_ignition_status != kStatusOk){ //kolla alla samtidigt och return false om någon misslyckas?
        std::cout << "Something went wrong on socket write for ignition, error code: "<< write_ignition_status << std::endl;
        ret = false;
    }
    else if(write_gear_status != kStatusOk){
        std::cout << "Something went wrong on socket write for gear, error code : " << write_gear_status << std::endl;
        ret = false;
    }
    else if(write_gas_status != kStatusOk){
        std::cout << "Something went wrong on socket write for gas, error code : " << write_gas_status  << std::endl;
        ret = false;
    }
    return ret; 
}

bool WriteCanFrameEmulator(SocketCan &socket, database_type::Database &db, const int &msdelay){
    bool ret = true;
    can_data_base::Rpm cb_rpm;
    unsigned int db_rpm = db.RPM;
    const CanFrame rpm = ConvertToCanFrame(db_rpm, cb_rpm);
    
    auto write_rpm_status = socket.WriteToCan(rpm);
    
    if (write_rpm_status != kStatusOk){
        std::cout << "Something went wrong on socket write for rpm, error code : " << write_rpm_status  << std::endl;
        ret = false;
    }
    return ret;
}
