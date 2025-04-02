import sys
import numpy as np
import matplotlib.pyplot as plt
import time

# time.monotonic()
# time.monotonic_ns()
def getTime1():
  return time.monotonic() * 1000

class Stw:
  def __init__(self):
    self.tstart = time.monotonic()
  def reset(self):
    self.tstart = time.monotonic()   
  def val(self):
    return (time.monotonic() - self.tstart) * 1000
  def valUS(self):
    return (time.monotonic() - self.tstart) * 1E6


stw1 = Stw(); cnt = 0
lst = []
for i in range(20):
  time.sleep(0.5)
  cnt += 1
  tim = stw1.val()
  lst.append((cnt, tim))
  print(f'{cnt}  {tim:0.1f}')

print('List'); print(lst);


