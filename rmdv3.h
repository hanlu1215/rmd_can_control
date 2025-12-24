#ifndef _RMDV3_H_
#define _RMDV3_H_
#include "Arduino.h"
//#include <SoftwareSerial.h>
#include "IBusBM.h"
#include <SPI.h>
#include "mcp2515.h"
extern IBusBM IBus;

extern float Offset_angle ;//偏移度数
extern uint16_t speed_0;
extern uint16_t speed_MAX;
extern struct can_frame canMsg1;
extern float w, f ;
extern unsigned long time_start;
extern unsigned long myTime;
extern int delaytime;
extern char flag;
extern int ibus_val[6];
extern int32_t angle, a;
extern int32_t speed_control;

extern MCP2515 mcp2515;

void loop_action1(void);
void loop_action0(void);
void loop_action2(int32_t);
void angles_control_speed(int32_t angle, uint16_t speedt);
String get_can_msg_a5(struct can_frame canMsg0);
void print_can_msg_60(struct can_frame canMsg0);
void stop_motor(void);
void cmd_resolve(String cmd);
void set_zero(void);
void read_motor_state(void);
void print_can_msg(struct can_frame canMsg0);
void reset_motor(void);
void read_motor__pose(void);
void control_speed_long(int32_t);
#endif
