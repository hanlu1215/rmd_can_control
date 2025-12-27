#include "Arduino.h"
#include <SPI.h>
#include "mcp2515.h"
#include "rmdv3.h"
int32_t angle, speed_control;
uint16_t speed_0;
uint16_t speed_MAX = 600;
int temp = 0;
int i, j, val; //定义i、j、val三个整型变量
char A[8];  //定义一个无符号数组A
char flag = 'l';
String cmd = "";
float f = 0.5;
float w = 2.0 * 3.1415926 * f;
int32_t a = 100 * 10;//60度，100为系数
float Offset_angle = 0.0;//偏移度数

unsigned long time_start;
unsigned long myTime;
int delaytime = 20;
bool cb_running = false;
unsigned long cb_duration = 0;

MCP2515 mcp2515(2);
String str = "";
struct can_frame canMsg1;
struct can_frame canMsg;

int sensorPin = A7;
int sensorValue = 0;
void setup() {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x9A;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = 0x00;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0x00;
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  while (flag != 'f') {
    flag = Serial.read();
  }
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  pinMode(sensorPin, INPUT);
  Serial.println("Ready to run!");
}
void loop() {
  /*--------------------------------------------------------------------------------------*/
  //软串口读取
  Serial.println("can set f: " + String(f) + "  a: " + String(a / 100.0) + "  Offset: " + String(Offset_angle));
  delay(1000);
  j = Serial.available();  // 读取串口寄存器中的信息的帧数
  if (j != 0) { // 如果串口寄存器中有数据，那么依次读取这些数据，并存放到数组A中
    flag = Serial.read();
    for (i = 0; i < 8; i++) {
      A[i] = ' ';
    }
    for (i = 0; i < j - 1; i++) {
      A[i] = Serial.read();
    }
    val = strtol(A, NULL, 10); // 将A中的字符转换成十进制数
  }
  switch (flag) {
    case 'f':
      f = float(val) / 100.0;
      Serial.println("f:" + String(f));
      flag = '1';
      break;
    case 'v':
      speed_control = 6L * 100L * val;
      Serial.println("v:" + String(val) + " rpm");
      loop_action2(speed_control);
      flag = '1';
      break;
    case 'a':
      a = 100.0 * val;
      Serial.println("angle:" + String(val));
      flag = '1';
      break;
    case 'c':
      cmd = A;
      Serial.println("cmd:" + cmd);
      cmd_resolve(cmd);
      cmd = " ";
      flag = '1';
      break;
    case 'm':
      speed_MAX = long(val) * long(6);
      Serial.println("max speed = " + String(speed_MAX));
      flag = '1';
      break;
    case 'b':
      Serial.println("Start running..." );
      loop_action1();
      flag = '1';
      break;
    case 'o':
      Offset_angle = val;
      Serial.println("Offset:" + String(Offset_angle));
      flag = '1';
      break;
    case 'd':
      delaytime = int(val);
      Serial.println("delaytime:" + String(delaytime));
      flag = '1';
      break;
    case 's':
      stop_motor();
      Serial.println("stop");
      flag = '1';
      break;
    case 'j':
      angle = 100.0 * val;
      Serial.println("contral to :" + String(angle));
      angles_control_speed(angle, 6 * 6);
      Serial.println("you wait 2s !");
      delay(2000);
      Serial.println("0 over!");
      flag = '1';
      break;
    default:
      sensorValue = analogRead(sensorPin);
      Serial.print("Humidity Sensor:(0-1023): ");
      Serial.println(sensorValue);
      flag = '1';
      break;
  }

  /*--------------------------------------------------------------------------------------*/
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == 0x241) {
      switch (canMsg.data[0]) {
        case 0xA5:
          str = str + get_can_msg_a5(canMsg);
          break;
        case 0x60:
          print_can_msg_60(canMsg);
          break;
        default:
          break;
      }
    }
  }
  /*--------------------------------------------------------------------------------------*/
}
