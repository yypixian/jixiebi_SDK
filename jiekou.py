import subprocess

def control_arm(angles):
    # 确保传入的角度列表长度为 6
    if len(angles) != 6:
        raise ValueError("角度列表必须包含 6 个值")

    # 将角度值转换为字符串列表
    angle_str = [str(angle) for angle in angles]

    # 调用 C 可执行文件
    try:
        result = subprocess.run(
            ["./fasong"] + angle_str,  # 可执行文件路径 + 参数
            check=True,  # 检查返回值
            text=True,  # 以文本模式捕获输出
            capture_output=True  # 捕获标准输出和标准错误
        )
        print(result.stdout)  # 打印 C 程序的输出
    except subprocess.CalledProcessError as e:
        print(f"程序执行失败: {e}")
        print(f"错误输出: {e.stderr}")

# 示例调用
#angles = [0, 0, 0, 0, 0, 0]

#control_arm(angles)