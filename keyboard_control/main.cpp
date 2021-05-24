/**
 * @file main.cpp
 * @brief
 * @author Chenwei Jia (cwjia98@gmail.com)
 * @version 1.0
 * @date 2021-05-20
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <unistd.h>

#include <iostream>
#include <mutex>
#include <thread>

#define ACCELE_RATE 0.5             // 向前加速度
#define REVERSE_ACCELE_RATE -0.5    // 向后加速度
#define NATURAL_DEACCELE_RATE -0.2  // 自然减速度
#define BRAKE_DEACCELE_RATE -1      // 刹车减速度
#define WHEEL_RATE 10               // 转动角度的速率
#define MAX_WHEEL_ANGLE 570         // 方向盘最大转动角度

int current_max_speed = 0;
int current_wheel_angle = 0;
int current_gear_state = 1;
bool current_hand_brake_state = 1;
char current_drive_state = 0;
int current_control_state = 0;

std::mutex m;

unsigned char scan_keyboard() {
    char kb_input = 0;
    struct termios new_settings;
    struct termios stored_settings;

    tcgetattr(STDIN_FILENO, &stored_settings);  //获得stdin 输入
    new_settings = stored_settings;             //
    new_settings.c_lflag &= (~ICANON);          //
    new_settings.c_lflag &= (~ECHO);            //
    new_settings.c_cc[VTIME] = 0;

    tcgetattr(STDIN_FILENO, &stored_settings);  //获得stdin 输入
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH,
              &new_settings);  // STDIN_FILENO TCSANOW

    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    if (select(1, &rfds, NULL, NULL, &tv) > 0) {
        kb_input = getchar();
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &stored_settings);

    return kb_input;
}

int keyboard_input_map(unsigned char key) {
    std::lock_guard<std::mutex> g1(m);
    current_drive_state = 2;
    int ret = 0;
    if (key == 27) {
        ret = -1;  // 复位，
        current_wheel_angle = 0;
        current_max_speed = 0;
        current_gear_state = 4;
        current_drive_state = 0;
    } else if (key == 'w' || key == 'W') {
        ret = 1;  // 发送一个正的加速度
        current_drive_state = 1;
    } else if (key == 's' || key == 'S') {
        ret = 2;  // 发送一个负的加速度
        current_drive_state = -1;
    } else if (key == 'a' || key == 'A') {
        ret = 3;  // 向左打方向盘
        current_wheel_angle =
            (current_wheel_angle - WHEEL_RATE <= -MAX_WHEEL_ANGLE
                 ? -MAX_WHEEL_ANGLE
                 : current_wheel_angle - WHEEL_RATE);
    } else if (key == 'd' || key == 'D') {
        ret = 4;  // 向右打方向盘
        current_wheel_angle =
            (current_wheel_angle + WHEEL_RATE >= MAX_WHEEL_ANGLE
                 ? MAX_WHEEL_ANGLE
                 : current_wheel_angle + WHEEL_RATE);
    } else if (key == 'q' || key == 'Q') {
        ret = 5;  // 快速向左打方向盘
        current_wheel_angle =
            (current_wheel_angle - WHEEL_RATE * 2 <= -MAX_WHEEL_ANGLE
                 ? -MAX_WHEEL_ANGLE
                 : current_wheel_angle - WHEEL_RATE * 2);
    } else if (key == 'e' || key == 'E') {
        ret = 6;  // 快速向右打方向盘
        current_wheel_angle =
            (current_wheel_angle + WHEEL_RATE * 2 >= MAX_WHEEL_ANGLE
                 ? MAX_WHEEL_ANGLE
                 : current_wheel_angle + WHEEL_RATE * 2);
    } else if (key == 'r' || key == 'R') {
        ret = 7;  // 方向盘复位至零
        current_wheel_angle = 0;
    } else if (key == 32) {
        ret = 8;  // 刹车
        current_drive_state = 0;
    } else if (key == '1') {
        ret = 11;  // P档
        current_gear_state = 1;
    } else if (key == '2') {
        ret = 12;  // R档
        current_gear_state = 2;
    } else if (key == '3') {
        ret = 13;  // N档
        current_gear_state = 3;
    } else if (key == '4') {
        ret = 14;  // D档
        current_gear_state = 4;
    } else if (key == '>' || key == '.') {
        ret = 21;  // 拉手刹
        current_hand_brake_state = true;
    } else if (key == '<' || key == ',') {
        ret = 22;  // 放手刹
        current_hand_brake_state = false;
    } else if (key == '+' || key == '=') {
        ret = 31;  // 升高最大车速
        current_max_speed++;
        current_max_speed = current_max_speed >= 180 ? 180 : current_max_speed;
    } else if (key == '-' || key == '_') {
        ret = 32;  // 降低最大车速
        current_max_speed--;
        current_max_speed = current_max_speed <= 0 ? 0 : current_max_speed;
    } else {
        ret = 0;  // 无操作
    }
    current_control_state = ret;
    return ret;
}

void print_info() {
    std::lock_guard<std::mutex> g2(m);

    std::cout << "\033[2J\033[0;0H\033[?25l";
    std::cout << "\n\033[1m--键盘调试工具--\033[0m\n" << std::endl;

    std::cout << "0.紧急情况按住Esc\n";
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    if (current_hand_brake_state) {
        std::cout
            << "1.手刹状态:放下(,) \033[4m\033[1m\033[41m拉起(.)\033[0m\n";
    } else {
        std::cout
            << "1.手刹状态:\033[4m\033[1m\033[42m放下(,)\033[0m 拉起(.)\n";
    }

    if (current_gear_state == 1) {
        std::printf(
            "2.当前档位:\033[41m\033[1m\033[4m\033[38mP(1)\033[0m R(2) N(3) "
            "D(4)\n");
    } else if (current_gear_state == 2) {
        std::printf(
            "2.当前档位:P(1) \033[42m\033[1m\033[4m\033[38mR(2)\033[0m N(3) "
            "D(4)\n");
    } else if (current_gear_state == 3) {
        std::printf(
            "2.当前档位:P(1) R(2) \033[42m\033[1m\033[4m\033[38mN(3)\033[0m "
            "D(4)\n");
    } else if (current_gear_state == 4) {
        std::printf(
            "2.当前档位:P(1) R(2) N(3) "
            "\033[42m\033[1m\033[4m\033[38mD(4)\033[0m\n");
    }

    if (current_drive_state == 1) {
        std::cout << "3.行驶状态:\033[4m\033[1m前进(W)\033[0m 制动(Space) "
                     "倒车(S) 滑行\n";

    } else if (current_drive_state == 0) {
        std::cout << "3.行驶状态:前进(W) \033[4m\033[1m制动(Space)\033[0m "
                     "倒车(S) 滑行\n";

    } else if (current_drive_state == -1) {
        std::cout << "3.行驶状态:前进(W) 制动(Space) "
                     "\033[4m\033[1m倒车(S)\033[0m 滑行\n";

    } else {
        std::cout << "3.行驶状态:前进(W) 制动(Space) 倒车(S) "
                     "\033[4m\033[1m滑行\033[0m\n";
    }

    std::printf("4.最大车速:\033[1m%4d\033[0mkm/h (+/-)\n", current_max_speed);
    std::printf(
        "5.转向角度:\033[1m%4d\033[0mdegree (慢速:A/D, 快速:Q/E, 复位:R)\n",
        current_wheel_angle);

    int slen = int((size.ws_col - 1) / 2) - 1;

    std::string lsb(slen, ' ');
    std::string rsb(slen, ' ');

    std::string ls(0, ' ');
    std::string rs(0, ' ');

    if (current_wheel_angle > 0) {
        int rslen =
            int(float(current_wheel_angle) / MAX_WHEEL_ANGLE * (slen - 1));
        rs = std::string(rslen, '>');
        rsb = std::string(slen - rslen, ' ');
    } else if (current_wheel_angle < 0) {
        int lslen =
            int(float(-current_wheel_angle) / MAX_WHEEL_ANGLE * (slen - 1));
        ls = std::string(lslen, '<');
        lsb = std::string(slen - lslen, ' ');
    } else {
        ;
    }

    std::cout << "[" + lsb + "\033[42m" + ls + "\033[0m|\033[42m" + rs +
                     "\033[0m" + rsb + "]\n";

    printf("\33[%d;0Hcontrol state:%d\033[?25h\r\n", size.ws_row - 1,
           current_control_state);
}

void publish_message() {
    std::lock_guard<std::mutex> g3(m);
    if (current_control_state == -1) {  // 最大车速与方向盘角度复位
        ;
    } else if (current_control_state / 10 == 0) {  // 发送控车指令
        ;
    } else if (current_control_state / 10 == 1) {  // 发送档位指令
        ;
    } else if (current_control_state / 10 == 2) {  // 发送手刹指令
        ;
    } else if (current_control_state / 10 == 3) {  // 调整最大车速，控车时发出
        ;
    } else {
        ;
    }
}

void input() {
    while (1) {
        keyboard_input_map(scan_keyboard());
    }
}

void print() {
    while (1) {
        print_info();
        usleep(50000);
    }
}

void publish() {
    while (1) {
        publish_message();
        usleep(10000);
    }
}

using namespace std;

int main(int argc, char *argv[]) {
    thread thread_input(input);
    thread thread_print(print);
    thread thread_publish(publish);
    thread_input.join();
    thread_print.join();
    thread_publish.join();
    return 0;
}
