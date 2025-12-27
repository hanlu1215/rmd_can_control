#include "rmdv3.h"
void cmd_resolve(String cmd) {
  String cmd_head = cmd.substring(0, 4);
  String cmd_head_2 = cmd.substring(0, 2);
  Serial.println("cmd_head:" + cmd_head);
  if (cmd_head == "0x81") {
    stop_motor();
  }
  if (cmd_head == "0x64") {
    set_zero();
    reset_motor();
    Serial.println("set_zero:");

  }
  if (cmd_head == "0x9C") {
    read_motor_state();
    Serial.println("read_motor_state:");

  }
  if (cmd_head == "0x60") {
    read_motor__pose();
    Serial.println("read_motor__pose:");
  }
  if (cmd_head_2 == "b") {
    String duration_str = cmd.substring(2);
    duration_str.trim();
    unsigned long duration = duration_str.toInt();
    if (duration > 0) {
      Serial.println("Starting loop_action1 for " + String(duration) + " seconds");
      loop_action1_timed(duration * 1000); // 转换为毫秒
    } else {
      Serial.println("Invalid duration for cb command");
    }
  }
  if (cmd_head_2 == "cs") {
    if (cb_running) {
      cb_running = false;
      Serial.println("Stopping cb command");
    } else {
      Serial.println("No cb command is running");
    }
  }

}

//循环进行周期运动
void loop_action0(void) {
  stop_motor();
}
void loop_action2(int32_t speed_control) {

  while (true) {
    char temp_falg = 'b';
    int i, j, val; //定义i、j、val三个整型变量
    char A[8];  //定义一个无符号数组A
    control_speed_long(speed_control);
    if ( Serial.read() == 'c') {
      delay(10);
      j = Serial.available();  // 读取串口寄存器中的信息的帧数
      if (j != 0) { // 如果串口寄存器中有数据，那么依次读取这些数据，并存放到数组A中
        temp_falg = Serial.read();
        for (i = 0; i < 8; i++) {
          A[i] = ' ';
        }
        for (i = 0; i < j - 1; i++) {
          A[i] = Serial.read();
        }
        val = strtol(A, NULL, 10); // 将A中的字符转换成十进制数

        if (temp_falg == 's') {
          break;
        }
      }
    }
    delay(delaytime);
  }
  control_speed_long(0);
  delay(delaytime);
  Serial.println("you wait 3s !");
  delay(3000);
  Serial.println("1 over!");
  stop_motor();
}
//循环进行周期运动
void loop_action1(void) {
  unsigned long myTime = 0;
  float t = 0;
  float dt = 0;
  float local_Offset_angle = Offset_angle;
  int temp2 = 0;
  float Offset_angle = 0;
  char temp_falg = 'b';
  int i, j, val; //定义i、j、val三个整型变量
  char A[8];  //定义一个无符号数组A
  w = 2.0 * 3.1415926 * f;
  set_zero();
  delay(1000);
  time_start = millis();
  while (true) {
    myTime = millis() - time_start;
    dt = myTime / 1000.0 - t;
    t = myTime / 1000.0;
    //    if (local_Offset_angle != Offset_angle) {
    //      if (local_Offset_angle > 0) {
    //        if (Offset_angle >= 0) {
    //          if (sin(w * t) > 0.95) {
    //            local_Offset_angle = Offset_angle;
    //          }
    //        }
    //        else {
    //          if (sin(w * t) > 0.95) {
    //            local_Offset_angle = 0;
    //          }
    //        }
    //      }
    //      if (local_Offset_angle < 0) {
    //        if (Offset_angle <= 0) {
    //          if (sin(w * t) < -0.95) {
    //            local_Offset_angle = Offset_angle;
    //          }
    //        }
    //        else {
    //          if (sin(w * t) < -0.95) {
    //            local_Offset_angle = 0;
    //          }
    //        }
    //      }
    //      if (local_Offset_angle == 0) {
    //        if (Offset_angle > 0) {
    //          if (sin(w * t) > 0.95) {
    //            local_Offset_angle = Offset_angle;
    //          }
    //        }
    //        else {
    //          if (sin(w * t) < -0.95) {
    //            local_Offset_angle = Offset_angle;
    //          }
    //        }
    //      }
    //    }
    //    angle = long((1 - exp(-t)) * a * sin(w * t) + local_Offset_angle - abs(local_Offset_angle) * sin(w * t));
    angle = long((1 - exp(-t)) * a * sin(w * t) );
    angles_control_speed(angle, speed_MAX);
    if ( Serial.read() == 'c') {
      delay(10);
      j = Serial.available();  // 读取串口寄存器中的信息的帧数
      if (j != 0) { // 如果串口寄存器中有数据，那么依次读取这些数据，并存放到数组A中
        temp_falg = Serial.read();
        for (i = 0; i < 8; i++) {
          A[i] = ' ';
        }
        for (i = 0; i < j - 1; i++) {
          A[i] = Serial.read();
        }
        val = strtol(A, NULL, 10); // 将A中的字符转换成十进制数

        if (temp_falg == 's') {
          break;
        }
        if (temp_falg == 'o') {
          if (abs(val * 120.0) < a) {
            Offset_angle = val * 100.0;
            Serial.print("Offset_angle:");
            Serial.println(Offset_angle);
          }

        }
        if (temp_falg == 'r') {
          Offset_angle = 10 * 100.0;
        }
        if (temp_falg == 'l') {
          Offset_angle = -10 * 100.0;
        }
        if (temp_falg == 'z') {
          Serial.println(A);
        }
        Serial.print(temp_falg);
        Serial.print("  dt(ms): ");
        Serial.println(dt * 1000);
      }
    }
    delay(delaytime);
  }
  angles_control_speed(0, 6 * 6);
  delay(delaytime);
  Serial.println("you wait 3s !");
  delay(3000);
  Serial.println("1 over!");
  stop_motor();
}

//定时运行loop_action1，可以通过cs指令提前停止
void loop_action1_timed(unsigned long duration) {
  unsigned long myTime = 0;
  float t = 0;
  float dt = 0;
  float local_Offset_angle = Offset_angle;
  int temp2 = 0;
  float Offset_angle = 0;
  char temp_falg = 'b';
  int i, j, val; //定义i、j、val三个整型变量
  char A[8];  //定义一个无符号数组A
  w = 2.0 * 3.1415926 * f;
  set_zero();
  delay(1000);
  time_start = millis();
  cb_running = true;
  cb_duration = duration;
  unsigned long cb_start_time = millis();
  
  while (cb_running) {
    // 检查是否超过指定时长
    if (millis() - cb_start_time >= duration) {
      Serial.println("cb command duration completed");
      break;
    }
    
    myTime = millis() - time_start;
    dt = myTime / 1000.0 - t;
    t = myTime / 1000.0;
    angle = long((1 - exp(-t)) * a * sin(w * t) );
    angles_control_speed(angle, speed_MAX);
    
    if ( Serial.read() == 'c') {
      delay(10);
      j = Serial.available();  // 读取串口寄存器中的信息的帧数
      if (j != 0) { // 如果串口寄存器中有数据，那么依次读取这些数据，并存放到数组A中
        temp_falg = Serial.read();
        for (i = 0; i < 8; i++) {
          A[i] = ' ';
        }
        for (i = 0; i < j - 1; i++) {
          A[i] = Serial.read();
        }
        val = strtol(A, NULL, 10); // 将A中的字符转换成十进制数

        if (temp_falg == 's') {
          Serial.println("cs command received, stopping cb");
          cb_running = false;
          break;
        }
        if (temp_falg == 'o') {
          if (abs(val * 120.0) < a) {
            Offset_angle = val * 100.0;
            Serial.print("Offset_angle:");
            Serial.println(Offset_angle);
          }

        }
        if (temp_falg == 'r') {
          Offset_angle = 10 * 100.0;
        }
        if (temp_falg == 'l') {
          Offset_angle = -10 * 100.0;
        }
        if (temp_falg == 'z') {
          Serial.println(A);
        }
        Serial.print(temp_falg);
        Serial.print("  dt(ms): ");
        Serial.println(dt * 1000);
      }
    }
    delay(delaytime);
  }
  
  cb_running = false;
  angles_control_speed(0, 6 * 6);
  delay(delaytime);
  Serial.println("you wait 3s !");
  delay(3000);
  Serial.println("cb command finished!");
  stop_motor();
}

void stop_motor(void) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x81;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = 0x00;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0x00;
  mcp2515.sendMessage(&canMsg1);
}
void set_zero(void) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x64;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = 0x00;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0x00;
  mcp2515.sendMessage(&canMsg1);
  canMsg1.data[0] = 0x76;
  delay(100);
  mcp2515.sendMessage(&canMsg1);
}
void reset_motor(void) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x76;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = 0x00;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0x00;
  mcp2515.sendMessage(&canMsg1);
}

void read_motor_state(void) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x9C;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = 0x00;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0x00;
  mcp2515.sendMessage(&canMsg1);
}
void read_motor__pose(void) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x60;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  canMsg1.data[4] = 0x00;
  canMsg1.data[5] = 0x00;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0x00;
  mcp2515.sendMessage(&canMsg1);
}

void control_speed_long(int32_t speed_long) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0xA2;
  canMsg1.data[1] = 0x00;
  canMsg1.data[2] = 0x00;
  canMsg1.data[3] = 0x00;
  for (int i = 0 ; i < 4; i++) {
    canMsg1.data[i + 4] = (speed_long >> (8 * i)) & 0xff;
  }
  mcp2515.sendMessage(&canMsg1);
}

void angles_control_speed(int32_t angle, uint16_t speedt) {
  canMsg1.can_id  = 0x141;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0xA4;
  canMsg1.data[1] = 0x00;
  if (speedt > speed_MAX ) {
    speedt = speed_MAX;
    Serial.println("Speedt erro max");
  } else {
    if (speedt <= 0) {
      speedt = 1;
      Serial.println("Speedt erro min");
    }
  }
  for (int i = 0 ; i < 2; i++) {
    canMsg1.data[i + 2] = (speedt >> (8 * i)) & 0xff;
  }
  for (int i = 0 ; i < 4; i++) {
    canMsg1.data[i + 4] = (angle >> (8 * i)) & 0xff;
  }
  mcp2515.sendMessage(&canMsg1);
}


void print_can_msg(struct can_frame canMsg0) {
  Serial.print(canMsg0.can_id, HEX); // print ID
  Serial.print(" ");
  Serial.print(canMsg0.can_dlc, HEX); // print DLC
  Serial.print(" ");
  for (int i = 0; i < canMsg0.can_dlc; i++)  { // print the data
    Serial.print(canMsg0.data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
String get_can_msg_a5(struct can_frame canMsg0) {
  int speed_temp;
  int Temperature_temp;
  float angle_temp;
  speed_temp = canMsg0.data[4] + (canMsg0.data[5] << 8);
  Temperature_temp = canMsg0.data[1];
  angle_temp = canMsg0.data[6] + (canMsg0.data[7] << 8);
  return "\n  id: " + String(canMsg0.can_id) + "  speed: " + String(speed_temp) + "  angle:" + String(angle_temp) + "  Temperature:" + String(Temperature_temp);

}
void print_can_msg_60(struct can_frame canMsg0) {
  int32_t encoder;
  encoder = canMsg0.data[4] + (canMsg0.data[5] * 0x100) + (canMsg0.data[6] * 0x10000) + (canMsg0.data[7] * 0x1000000);
  Serial.println("encoder: " + String(encoder));

}
