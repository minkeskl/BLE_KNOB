/**
 * @file knob.h
 * @author minke (minkeskl@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef KNOB_H
#define KNOB_H

#include <SimpleFOC.h>

#define NOT_SET -12345.0

typedef struct
{
    float P;
    float I;
    float D;
} PIDType;

enum knobMode
{
    powerOffMode=0,
    triggerMode,
    spinMode,
    dampMode,
};

class Knob : public BLDCMotor
{
    // 触发的回调函数
    // 偏移角
    // 内置PID

public:
    Knob(int pp, float R = NOT_SET, float KV = NOT_SET);

    int mode;

    // BLDCMotor *motor;

    void setMode(int _mode);

    void linkMotor(BLDCMotor *motor);

    void powerOffInit(void);

    void powerOffMainFunction(void);

    // 模式1扳机模式
    //  float triggerOffset;

    float triggerAngle;

    int triggerLast;

    int triggerPeriod;

    PIDType triggerVelocityPid;

    PIDType triggerAnglePid;

    PIDType triggeringPid;

    void triggerInit(void);

    void (*triggerLessEvent)(void);

    void (*triggerOverEvent)(void);

    void triggerMainFunction(void);

    PIDType spinVelocityPid;

    PIDType spinAnglePid;

    void spinInit(void);

    void spinMainFunction(void);

    float dampAngle;

    int dampLast;

    int dampPeriod;

    PIDType dampVelocityPid;

    PIDType dampAnglePid;

    void dampInit(void);

    void (*dampLessEvent)(void);

    void (*dampOverEvent)(void);

    void dampMainFunction(void);

    void run();

private:
};
#endif
