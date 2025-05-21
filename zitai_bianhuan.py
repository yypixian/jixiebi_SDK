import roboticstoolbox as rtb
from spatialmath import *
from spatialmath.base import *
import numpy as np
import matplotlib.pyplot as plot




#########二维空间
#代表的是二维空间里面姿态变换，前面的分别是相较于x轴和y轴的位移，后面的是相较于x轴，y轴的旋转
T = SE2(1, 2, np.pi/3) 
print(T)
T = SE2(1, 2, 60, unit='deg')
print(T)

#定义一个二维空间初始点（0,0）
initial_point = np.array([[0],[0],[1]])
print(initial_point.shape)
#左边乘以一个齐次的姿态矩阵，做空间变换
post_point = np.array(T).dot(initial_point)#初始点左边乘以姿态矩阵
print(post_point)




T1 = SO2(np.pi/3)#二维空间上仅仅旋转
#print(T1)

T2 = SE2.Tx(2)#二维空间x轴仅平移
#print(T2)

T3 = SE2.Ty(2)#二维空间y轴仅平移
#print(T3)

T4 = transl2([1,2])#二维空间同时x轴和y轴作平移
#print(T4)




########三维空间变换
T = SE3.Rx(np.pi/3)#三维矩阵绕x轴进行旋转
#print(T)
#T = SE3.rotx(np.pi/3)也可以用这个函数表示,但是仅会得到一个旋转矩阵，不会是齐次的形式
T = SE3.Ry(np.pi/3)#三维矩阵绕y轴进行旋转
#print(T)
T = SE3.Rz(np.pi/3)#三维矩阵绕z轴进行旋转
#print(T)

T1= SE3.Rx(np.pi/2,t=np.array([0.1,0,0]))#三维矩阵绕x轴进行旋转和平移
T2 = SE3.Ry(np.pi/2,t=np.array([0,0.1,0]))#三维矩阵绕y轴进行旋转和平移
T3 = SE3.Rz(np.pi/2,t=np.array([0,0,0.1]))#三维矩阵绕z轴进行旋转和平移
print(T1)
print(T2)
print(T3)


#如果将这几个姿态变换进行叠加，就是左乘的操作（即相当于固定原始坐标轴的变换）
T = T3*T2*T1
print(T)
#T.plot()
#plot.show()



#动画形式表示绕x轴进行旋转,并进行左乘一个旋转齐次矩阵，即相较于世界坐标轴进行变换
X = T1
X.animate(frame='A',color='green')
X.animate(start=None)
X.plot()
X = T2*T1
X.animate(frame='A',color='green')
X.animate(start=None)
plot.show()

#动画形式表示绕x轴进行旋转,并进行右乘一个旋转齐次矩阵，即相较于原先的坐标轴进行变换
X = T1
X.animate(frame='A',color='green')
X.animate(start=None)
X.plot()
X = T1*T2
X.animate(frame='A',color='green')
X.animate(start=None)
plot.show()











