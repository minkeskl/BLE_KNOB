
/**
 * @file main.cpp
 * @author minke (minkeskl@qq.com)
 * @brief 在串口窗口中输入：T+位置，就可以使得两个电机闭环转动
          比如让两个电机都转动180°，则输入其弧度制：T3.14
          在使用自己的电机时，请一定记得修改默认极对数，即 BLDCMotor(14) 中的值，设置为自己的极对数数字
          程序默认设置的供电电压为 16.8V,用其他电压供电请记得修改 voltage_power_supply , voltage_limit 变量中的值
          默认PID针对的电机是 GB6010 ，使用自己的电机需要修改PID参数，才能实现更好效果
 * @version 0.1
 * @date 2023-01-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "knob.h"
//#include <BleKeyboard.h>
#include "BleHid.h"

MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
TwoWire I2Cone = TwoWire(0);
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 22);
BleKeyboard bleKeyboard = BleKeyboard();
Knob knob(7);

/**
 * @brief 
 * 翻页控制,散点控制,鼠标滚轮单次触发
 */
void triggerLessEvent(void)
{
  Serial.println("<");
  bleKeyboard.write(KEY_PAGE_DOWN);
}

/**
 * @brief 
 * 翻页控制,散点控制,鼠标滚轮单次触发
 */
void triggerOverEvent(void)
{
  Serial.println(">");
  bleKeyboard.write(KEY_PAGE_UP);
}

/**
 * @brief 
 * 音频控制,扳机控制,键盘音频连续触发
 */
void dampLessEvent(void)
{
  Serial.println("<");
  if (bleKeyboard.isConnected())
  {
    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
  }
}

/**
 * @brief 
 * 音频控制,散点挡位,键盘音频单次触发
 */
void dampOverEvent(void)
{
  Serial.println(">");
  if (bleKeyboard.isConnected())
  {
    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
  }
}

/**
 * @brief 
 * 初始化
 */
void setup()
{
  bleKeyboard.begin();

  I2Cone.begin(19, 18, 400000UL);

  sensor.init(&I2Cone);
  // 连接motor对象与传感器对象
  knob.linkSensor(&sensor);

  // 供电电压设置 [V]
  driver.voltage_power_supply = 12;
  driver.init();

  // 连接电机和driver对象
  knob.linkDriver(&driver);

  // 初始化电机
  knob.init();

  // 初始化 FOC
  knob.initFOC();

  knob.triggerInit();
  knob.triggerLessEvent=triggerLessEvent;
  knob.triggerOverEvent=triggerOverEvent;

  knob.dampInit();
  knob.dampLessEvent = dampLessEvent;
  knob.dampOverEvent = dampOverEvent;

  Serial.begin(115200);
}
/**
 * @brief 
 * 程序主循环
 */
void loop()
{
  knob.run();

  if (Serial.available() > 0)
  {
    //char i = Serial.read();
    int mode = Serial.parseInt();
    // Serial.parseTint() 会解析出串口内的数字
    Serial.println(mode);
    knob.setMode(mode);
  }
}
