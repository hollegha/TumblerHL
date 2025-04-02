
from cyberpi import timer, console, led
from time import sleep

def Led(col, port):
  led.on(col,id=port)

class Stw:
  def __init__(self):
    self.tstart = timer.get()
  def reset(self):
    self.tstart = timer.get()
  def val(self):
    return (timer.get() - self.tstart)
  
class RTBlinker:
  def __init__(self,cycTime,port,col):
    self.port = port
    self.col = col
    self.DT = cycTime
    self.stw = Stw()
    self.turn = False
  def checkAction(self):
    if self.stw.val() < self.DT:
      return
    self.stw.reset()
    if self.turn:
      Led(self.col,self.port)
    else:
      Led('black',self.port)
    self.turn = not self.turn

# main main main
console.println('RT_Demo2')
bl1 = RTBlinker(1/5,1,'r')
bl2 = RTBlinker(1/1,3,'b')
while True:
  bl1.checkAction()
  bl2.checkAction()
 



