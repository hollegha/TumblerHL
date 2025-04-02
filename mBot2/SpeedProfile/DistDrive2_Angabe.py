
from cyberpi import timer, console, controller
from time import sleep
import mbot2

# 'up' 'down' 'a' 'b'
def IsButton(btn):
    return controller.is_press(btn)

def motL(speed):
  mbot2.EM_set_speed(speed,1)

def motR(speed):
  mbot2.EM_set_speed(-speed,2)

def encL():
  return mbot2.EM_get_angle(1)

def encR():
  return -mbot2.EM_get_angle(2)

def resetEnc(port):
  mbot2.EM_reset_angle(port)

class Stw:
    def __init__(self):
        self.tstart=timer.get()
    def reset(self):
        self.tstart=timer.get()
    def val(self): # in sec
        return (timer.get()-self.tstart)
 

class RateLim:
  def __init__(self, rate):
    self.rate = rate #rate in sec
    self.inp=0
    self.outp=0
    self.stw=Stw()
  def start(self):
    self.stw.reset()
  def step(self):
    return True

AMAX1=50; AMAX2=200

def DriveDist(Vmax,dist):
  global AMAX1, AMAX2
  motL(0); motR(0)

#main main main
console.println('DistDrive_ang')
while not IsButton('a'):
  pass
DriveDist(190,2000) # Vmax dist
txt = '%d %d' % (encL(),encR())
console.println(txt)





  
    
    


    
    
