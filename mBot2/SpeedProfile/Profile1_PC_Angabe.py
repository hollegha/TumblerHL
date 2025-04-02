import sys
import numpy as np
import matplotlib.pyplot as plt
import time

class Stw:
  def __init__(self):
    self.tstart = time.monotonic()
  def reset(self):
    self.tstart = time.monotonic()   
  def val(self):
    return (time.monotonic() - self.tstart)
 
class SpeedProfile:
  def __init__(self, accel, vmax, dist):
    self.accel = accel
    self.vmax = vmax
    self.sges = dist
    self.v = 0
    self.stw = Stw()
    self.dt = 0.1
  def start(self):
    self.stw.reset(); self.v=0
  def step(self,actDist):
    return True


#main main main
# gtim=0; gdt=0.01 # for debugging
tim = Stw(); spp = SpeedProfile(100,100,150);
tl=[]; vl=[]; sl=[] # t-List v-List s-List
dist = 0 # driven distance (s)
print('start')
spp.start(); tim.reset()
while spp.step(dist): #gtim <= 4.0
  dist += spp.v*spp.dt
  tl.append(tim.val()); vl.append(spp.v); sl.append(dist)
  time.sleep(0.01)
  # gtim += gdt
print(f'{tim.val()}  {dist:1.1f}')

ta = np.array(tl); va = np.array(vl); sa = np.array(sl)
plt.scatter(ta,va,s=20,marker='*',c='r')
plt.scatter(ta,sa,s=20,marker='*',c='b')
plt.grid(); plt.show()


