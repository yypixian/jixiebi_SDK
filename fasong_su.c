#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>

// 生成四轮速度控制指令
char *generate_wheel_command(float speeds[]) {
    // 计算需要的缓冲区大小 (每个速度最多6字符 + 分隔符 + 指令头尾)
    int buf_size = 32;
    char *command = (char *)malloc(buf_size * sizeof(char));
    if (command == NULL) {
        perror("内存分配失败");
        exit(1);
    }
    
    // 格式化指令字符串
    snprintf(command, buf_size, "$$%.1f %.1f %.1f %.1f!", 
             speeds[0], speeds[1], speeds[2], speeds[3]);
    
    return command;
}

int main(int argc, char *argv[]) {
    int fd;
    const char *device = "/dev/ttyCH341USB0";
    float speeds[4];  // 存储4个轮子的速度值

    // 检查命令行参数数量
    if (argc != 5) {
        printf("用法: %s <速度1> <速度2> <速度3> <速度4>\n", argv[0]);
        printf("例如: %s 2.0 2.0 0.1 0.5\n");
        printf("注意: 速度范围为 -3.0 到 3.0\n");
        return 1;
    }

    // 将命令行参数转换为浮点数并存储到 speeds 数组
    for (int i = 0; i < 4; i++) {
        speeds[i] = atof(argv[i + 1]);
        if (speeds[i] < -3.0 || speeds[i] > 3.0) {
            printf("错误: 速度值必须在 -3.0 到 3.0 之间\n");
            return 1;
        }
    }

    // 生成速度控制指令字符串
    char *wheel_command = generate_wheel_command(speeds);
    printf("Generated wheel command: %s\n", wheel_command);

    // 打开串口设备
    fd = open(device, O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        perror("无法打开设备");
        free(wheel_command);
        return 1;
    }

    // 配置串口参数
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("无法获取串口属性");
        close(fd);
        free(wheel_command);
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
        free(wheel_command);
        return 1;
    }

    // 发送速度控制指令
    ssize_t written = write(fd, wheel_command, strlen(wheel_command));
    if (written < 0) {
        perror("写入数据失败");
    } else {
        printf("成功写入 %zd 字节指令: %s\n", written, wheel_command);
    }

    // 释放内存
    free(wheel_command);

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