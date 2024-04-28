#ifndef DEVICE_H
#define DEVICE_H
#include<string>
#include<iostream>
#include"spdlog/fmt/bundled/core.h"
class Device{
private:
    std::string hostAddress;
    std::string name;
    std::string hardwareType;
    std::string positon;
    std::string literalName;
    std::string manufactureCode;
public:
    Device(std::string hostAddress, std::string name):hostAddress(hostAddress), name(name){}
    Device(std::string hostAddress, std::string name, std::string hardwareType, std::string position, std::string literalName, std::string manufactureCode):hostAddress(hostAddress), name(name){
        this->hostAddress = hostAddress;
        this->name = name;
        this->hardwareType = hardwareType;
        this->positon = position;
        this->literalName = literalName;
        this->manufactureCode = manufactureCode;
    }
    Device(const Device& device)noexcept{
        this->hostAddress = device.hostAddress;
        this->name = device.name;
        this->hardwareType = device.hardwareType;
        this->positon = device.positon;
        this->literalName = device.literalName;
        this->manufactureCode = device.manufactureCode;
    }

    Device& operator=(const Device& device)noexcept{
        if(this != &device){
            this->hostAddress = device.hostAddress;
            this->name = device.name;
            this->hardwareType = device.hardwareType;
            this->positon = device.positon;
            this->literalName = device.literalName;
            this->manufactureCode = device.manufactureCode;
        }
        return *this;
    }

    Device(Device&& device)noexcept{
        if(this != &device){
            this->hostAddress = std::move(device.hostAddress);
            this->name = std::move(device.name);
            this->hardwareType = std::move(device.hardwareType);
            this->positon = std::move(device.positon);
            this->literalName = std::move(device.literalName);
            this->manufactureCode = std::move(device.manufactureCode);
        }
    }

    Device& operator=(Device&& device)noexcept{
        if(this != &device){
            this->hostAddress = std::move(device.hostAddress);
            this->name = std::move(device.name);
            this->hardwareType = std::move(device.hardwareType);
            this->positon = std::move(device.positon);
            this->literalName = std::move(device.literalName);
            this->manufactureCode = std::move(device.manufactureCode);
        }
        return *this;

    }
    /*****************************************************************
    * 函数名称： 小于符号运算符重载
    * 功能描述： 用于比较两个device类的大小关系，方便作为树的索引
    * 参数说明： 参数说明
    * 返回值：   device1 < device2 返回device1是否小于device2
    ******************************************************************/
    bool operator < (const Device& info) const
    {
        if(this->name != info.name){
            return this->name < info.name;
        }
        return this->getHostAddress() < info.getHostAddress();
    }
    /*****************************************************************
    * 函数名称： 等于符号运算符重载
    * 功能描述： 用于比较两个device类的大小关系，方便作为树的索引
    * 参数说明： 参数说明
    * 返回值：   device1 == device2 返回device1是否等于device2
    ******************************************************************/
    bool operator == (const Device& info) const
    {
        return ((this->hostAddress == info.hostAddress) && (this->name == info.name)
                && (this->hardwareType == info.hardwareType)
                && (this->positon == info.positon)
                && (this->literalName == info.literalName)
                && (this->manufactureCode == info.manufactureCode));
    }
    /*****************************************************************
    * 函数名称： Name
    * 功能描述： 详细描述
    * 参数说明： 参数说明
    * 返回值：   返回值说明
    ******************************************************************/
    std::string getHostAddress()const{return hostAddress;}
    std::string getName()const{return name;}
    std::string getHardwareType()const{return hardwareType;}
    std::string getPosition()const{return positon;}
    std::string getLiteralName()const{return literalName;}
    std::string getManufactureCode()const{return manufactureCode;}

    std::string toString()const{
        return fmt::format("[host:{} name:{} hardwareType:{} position:{} literalName:{} manufactureCode:{}]", hostAddress, name, hardwareType, positon, literalName, manufactureCode);
    }
};
#endif // DEVICE_H
