
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
 
class RateLim:
  def __init__(self, rate):
    self.rate = rate
    self.inp = 0
    self.outp = 0
    self.stw = Stw()
    self.dt = 0.1
  def start(self):
    self.stw.reset()
  def step(self):
    self.dt = self.stw.val(); self.stw.reset()
    if self.outp == self.inp: # ramp finished
      return False
    if self.outp < self.inp:
      self.outp += self.rate * self.dt
      if self.outp >= self.inp:
        self.outp = self.inp # up finished
        return False
      return True
    # else outp>inp
    self.outp -= self.rate * self.dt
    if self.outp <= self.inp:
      self.outp = self.inp # down finished
      return False
    return True
 
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
    self.dt = self.stw.val(); self.stw.reset()
    # self.dt = 0.01
    sbrems = self.sges - actDist
    if sbrems<=0:
      self.v = 0
      return False
    accBrems = (self.v * self.v) / (2 * sbrems)
    if accBrems > self.accel: # bremsen
      if self.v==0:
        return False # profile finished
      self.v -= self.accel * self.dt
      if self.v<=0:
        self.v = 0
        return False; # profile finished
      else:
        return True
    #else else
    if self.v>=self.vmax: # flat phase
      return True
    self.v += self.accel * self.dt # acc phase
    return True


#main main main
# gtim=0; gdt=0.01 # for debugging
tim = Stw(); spp = SpeedProfile(100,100,30);
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


