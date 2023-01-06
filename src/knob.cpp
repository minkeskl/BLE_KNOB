/**
 * @file knob.cpp
 * @author minke (minkeskl@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "knob.h"
#include <SimpleFOC.h>

/**
 * @brief Construct a new Knob:: Knob object
 * 
 * @param pp 
 * @param _R 
 * @param _KV 
 */
Knob::Knob(int pp, float _R, float _KV) : BLDCMotor(pp, _R, _KV)
{
    // FOC模型选择
    foc_modulation = FOCModulationType::SpaceVectorPWM;
    // 运动控制模式设置
    controller = MotionControlType::angle;

    // 速度PI环设置
    PID_velocity.P = 0.02;
    PID_velocity.I = 5;

    // 角度P环设置
    P_angle.P = 1;

    // 最大电机限制电机
    voltage_limit = 12;

    // 速度低通滤波时间常数
    LPF_velocity.Tf = 0.01;

    // 设置最大速度限制
    velocity_limit = 40;

    mode = powerOffMode;

    triggerAngle = 0.6;

    triggerVelocityPid.P = 0.05;
    triggerVelocityPid.I = 0;
    triggerVelocityPid.D = 0;
    triggerAnglePid.P = 3;
    triggerAnglePid.I = 0;
    triggerAnglePid.D = 0;
    triggeringPid.P = 4;
    triggeringPid.I = 0;
    triggeringPid.D = 0;

    triggerLast = 0;

    triggerLessEvent = nullptr;

    triggerOverEvent = nullptr;

    dampLessEvent = nullptr;
    dampOverEvent = nullptr;
}

void Knob::setMode(int _mode)
{
    target = sensor->getAngle();
    mode = _mode;
    switch (mode)
    {
    case powerOffMode:
        powerOffInit();
        break;
    case triggerMode:
        triggerInit();
        break;
    case spinMode:

        break;
    case dampMode:
        dampInit();
        break;
    default:
        mode = powerOffMode;
        powerOffInit();
        break;
    }
}

// void Knob::linkMotor(BLDCMotor *_motor)
// {
//     motor = _motor;
// }
/**
 * @brief 
 * 脱力初始化
 */
void Knob::powerOffInit(void)
{
    controller = MotionControlType::torque;
    target = 0;
}

/**
 * @brief 
 * 脱力主函数
 */
void Knob::powerOffMainFunction(void)
{
    target = 0;
}

/**
 * @brief 
 * 扳机初始化
 */
void Knob::triggerInit()
{
    controller = MotionControlType::angle;
    target = sensor->getAngle();
}

/**
 * @brief 
 * 扳机主函数
 */
void Knob::triggerMainFunction(void)
{
    if (sensor->getAngle() > target + triggerAngle)
    {
        if (triggerLast != 1)
        {
            // 速度PI环设置
            PID_velocity.P = 0.2;
            PID_velocity.I = 0.1;
            // 角度P环设置
            P_angle.P = 3;

            triggerOverEvent();
        }
        else if (triggerLast == 1)
        {
        }
        triggerLast = 1;
    }
    else if (sensor->getAngle() < target - triggerAngle)
    {
        if (triggerLast != -1)
        {
            // 速度PI环设置
            PID_velocity.P = 0.2;
            PID_velocity.I = 0.1;
            // 角度P环设置
            P_angle.P = 3;

            triggerLessEvent();
        }
        else if (triggerLast == -1)
        {
        }
        triggerLast = -1;
    }
    else
    {
        // 速度PI环设置
        PID_velocity.P = 0.2;
        PID_velocity.I = 0.1;
        // 角度P环设置
        P_angle.P = 7;

        if (triggerLast != 0)
        {
            // 这里切换电机PID
        }
        else if (triggerLast == 0)
        {
        }
        triggerLast = 0;
    }
}

// void Knob::spinInit(void)
// {
//     target = sensor->getAngle();
//         controller = MotionControlType::torque;
//     dampAngle = sensor->getAngle();
// }

// void Knob::spinMainFunction(void)
// {
//     // if (sensor->getAngle() > target + triggerAngle)
//     // {
//     //     if (triggerLast != 1)
//     //     {
//     //         triggerOverEvent();
//     //     }
//     //     else if (triggerLast == 1)
//     //     {
//     //     }
//     //     triggerLast = 1;
//     // }
//     // else if (sensor->getAngle() < target - triggerAngle)
//     // {
//     //     if (triggerLast != -1)
//     //     {
//     //         triggerLessEvent();
//     //     }
//     //     else if (triggerLast == -1)
//     //     {
//     //     }
//     //     triggerLast = -1;
//     // }
//     // else
//     // {
//     //     if (triggerLast != 0)
//     //     {
//     //         // 这里切换电机PID
//     //     }
//     //     else if (triggerLast == 0)
//     //     {
//     //     }
//     //     triggerLast = 0;
//     // }
// }

/**
 * @brief 
 * 散点初始化
 */
void Knob::dampInit(void)
{
    controller = MotionControlType::torque;
    dampAngle = sensor->getAngle();
}

/**
 * @brief 
 * 散点主函数
 */
void Knob::dampMainFunction(void)
{
    float tmpAngle = sensor->getAngle();
    if (tmpAngle > dampAngle + 0.5)
    {
        if (triggerLast != 1)
        {
            target = -2;
            dampOverEvent();
        }
        else if (triggerLast == 1)
        {
            dampAngle = tmpAngle;
        }
        triggerLast = 1;
    }
    else if (tmpAngle < dampAngle - 0.5)
    {
        if (triggerLast != -1)
        {
            target = 2;
            dampLessEvent();
        }
        else if (triggerLast == -1)
        {
            dampAngle = tmpAngle;
        }
        triggerLast = -1;
    }
    else
    {

        if (triggerLast != 0)
        {
        }
        else if (triggerLast == 0)
        {
            target = 0;
        }
        triggerLast = 0;
    }
}

/**
 * @brief 
 * 核心循环
 */
void Knob::run()
{
    // triggerMainFunction();
    // dampMainFunction();
    switch (mode)
    {
    case powerOffMode:
        powerOffMainFunction();
        break;
    case triggerMode:
        triggerMainFunction();
        break;
    case spinMode:

        break;
    case dampMode:
        dampMainFunction();
        break;
    default:
        mode = powerOffMode;
        powerOffMainFunction();
        break;
    }
    loopFOC();
    move(target);
}
