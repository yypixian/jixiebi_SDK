from roboticstoolbox import *
from spatialmath import *
from math import pi
import numpy as np

import time

from jiekou import control_arm

DFbot = DHRobot(
    [
        RevoluteMDH(d=0.04145,qlim=np.array([-np.pi*3/4,np.pi*3/4])),
        RevoluteMDH(alpha=np.pi/2,qlim=np.array([-np.pi/2,np.pi/2])),
        RevoluteMDH(a=-0.08285,qlim=np.array([-np.pi*3/4,np.pi*3/4])),
        RevoluteMDH(a=-0.08285,qlim=np.array([-np.pi*3/4,np.pi*3/4])),
        RevoluteMDH(alpha=-np.pi/2,d=0.11,qlim=np.array([0,np.pi])),  
 
    ],
    name="DFbot",
)


def convert_deg(deg):
    d1=deg[0]
    d2=-90 - deg[1]
    d3=deg[2]
    d4=deg[3]-90
    d5=deg[4]
    return [d1,d2,d3,d4,d5,0]

DFbot.plot([0,0,0,0,0], block=True)


state0 = [0,0,0,0,0]
deg0 = [0,0,0,0,0]
T0=DFbot.fkine(state0)


#初始姿态下的末端位置
deg0_gaibian = convert_deg(deg0)
print(deg0_gaibian)
time.sleep(1)  # 延时 1 秒
#control_arm(convert_deg(deg0))






state1=[0,-np.pi/6,0,np.pi,0]
DFbot.plot(state1, block=True)
deg1=[0,-30,0,180,0]
T1=DFbot.fkine(state1)
print(deg1)

time.sleep(1)  # 延时 1 秒
#control_arm(convert_deg(deg1))






T2=np.array(T1)
T2[0,-1]=-0.255
T2[2,-1]=0.02

sol = DFbot.ikine_LM(T2,q0=[0,0,0,0,0],ilimit=100, slimit=100,joint_limits=True)

print(sol.q)


state2=sol.q


deg2=[x/np.pi*180 for x in state2]

print(deg2)

#control_arm(convert_deg(deg2))

###
