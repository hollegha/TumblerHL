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
    self.inp=0
    self.outp=0
    self.stw=Stw()
    self.dt=0.1
  def start(self):
    self.stw.reset();
  def step(self):
    self.dt=self.stw.val(); self.stw.reset()
    if self.outp==self.inp: # ramp finished
      return False
    if self.outp<self.inp:
      self.outp += self.rate * self.dt
      if self.outp>=self.inp:
        self.outp=self.inp # up finished
        return False
      return True
    # else outp>inp
    self.outp -= self.rate * self.dt
    if self.outp<=self.inp:
      self.outp=self.inp # down finished
      return False
    return True
 

#main main main
tim=Stw(); rt1=RateLim(100) 
tl=[]; vl=[]; sl=[] # t-List v-List s-List
dist=0 # driven distance (s)
rt1.inp=100; rt1.start(); tim.reset(); 
while tim.val()<=4.0:
  if tim.val()>1.5:
    rt1.inp=0
  rt1.step(); # one step of speed-ramps
  dist += rt1.outp*rt1.dt # integrate position
  # log RT-Mesurements
  tl.append(tim.val()); vl.append(rt1.outp); sl.append(dist)
  time.sleep(0.01)

ta=np.array(tl); va=np.array(vl); sa=np.array(sl)
plt.scatter(ta,va,s=20,marker='*',c='r')
plt.scatter(ta,sa,s=20,marker='*',c='b')
plt.grid(); plt.show()



