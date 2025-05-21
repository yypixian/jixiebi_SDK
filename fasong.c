#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

// 角度转PWM值 (0-270度 -> 500-2500)
int angle_to_pwm(int angle) {
    // 确保角度在有效范围内
    if (angle < -135) angle = -135;
    if (angle > 135) angle = 135;
    
    // 线性映射：-135度->500，0度->1500，135度->2500
    float pwm = 1500 + (angle * (1000.0 / 135.0));
    return (int)(pwm + 0.5);  // 四舍五入到最接近的整数
}

// 生成所有舵机指令的复合字符串
char *generate_compound_command(int angles[], int servo_count, int move_time) {
    // 计算需要的缓冲区大小 (每个指令15字节 * 舵机数量 + 3{}+1)
    int buf_size = 15 * servo_count + 4;
    char *command = (char *)malloc(buf_size * sizeof(char));
    if (command == NULL) {
        perror("内存分配失败");
        exit(1);
    }
    
    // 添加起始大括号
    strcpy(command, "{");
    
    for (int i = 0; i < servo_count; i++) {
        int pwm = angle_to_pwm(angles[i]);
        // 确保PWM值在有效范围内
        if (pwm < 500) pwm = 500;
        if (pwm > 2500) pwm = 2500;
        
        // 追加单个舵机指令到复合字符串
        char temp[16];
        snprintf(temp, sizeof(temp), "#%03dP%04dT%04d!", i, pwm, move_time);
        strcat(command, temp);
    }
    
    // 添加结束大括号
    strcat(command, "}");
    
    return command;
}

int main(int argc, char *argv[]) {
    int fd;
    const char *device = "/dev/ttyCH341USB0";
    int angles[6];          // 存储6个舵机的角度值
    int move_time = 1000;   // 默认移动时间(ms)

    // 检查命令行参数数量
    if (argc < 7 || argc > 8) {
        printf("用法: %s <角度1> <角度2> <角度3> <角度4> <角度5> <角度6> [时间(ms)]\n", argv[0]);
        printf("例如: %s 90 45 0 45 90 135 1000\n", argv[0]);
        printf("注意: 角度范围0-270度, 时间范围0-9999ms\n");
        return 1;
    }

    // 将命令行参数转换为整数并存储到 angles 数组
    for (int i = 0; i < 6; i++) {
        angles[i] = atoi(argv[i + 1]);
        if (angles[i] < -135 || angles[i] > 135) {
            printf("错误: 角度值必须在 -135 到 135 之间\n");
            return 1;
        }
    }

    // 如果有时间参数，则使用
    if (argc == 8) {
        move_time = atoi(argv[7]);
        if (move_time < 0 || move_time > 9999) {
            printf("错误: 时间值必须在 0 到 9999 之间\n");
            return 1;
        }
    }

    // 生成复合指令字符串
    char *compound_command = generate_compound_command(angles, 6, move_time);
    printf("Generated compound command: %s\n", compound_command);

    // 打开串口设备
    fd = open(device, O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        perror("无法打开设备");
        free(compound_command);
        return 1;
    }

    // 配置串口参数
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("无法获取串口属性");
        close(fd);
        free(compound_command);
        return 1;
    }

    // 设置波特率为115200
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // 8N1 (8数据位, 无奇偶校验, 1停止位)
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // 禁用硬件流控制
    tty.c_cflag &= ~CRTSCTS;

    // 禁用软件流控制
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // 设置为原始模式
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;

    // 应用配置
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("无法设置串口属性");
        close(fd);
        free(compound_command);
        return 1;
    }

    // 发送复合指令
    ssize_t written = write(fd, compound_command, strlen(compound_command));
    if (written < 0) {
        perror("写入数据失败");
    } else {
        printf("成功写入 %zd 字节指令: %s\n", written, compound_command);
    }

    // 释放内存
    free(compound_command);

    // 清空串口缓冲区
    if (tcflush(fd, TCIOFLUSH) == 0) {
        printf("串口缓冲区已清空\n");
    } else {
        perror("清空串口缓冲区失败");
    }

    // 关闭设备
    close(fd);
    return 0;
}