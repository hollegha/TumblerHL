
from cyberpi import timer, console, controller
from time import sleep
import mbot2

# 'up' 'down' 'a' 'b'
def IsButton(btn):
    return controller.is_press(btn)

def motL(speed): # max 1200
  mbot2.EM_set_speed(speed/6,1)

def motR(speed):
  mbot2.EM_set_speed(-speed/6,2)

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
    def valMS(self):
        return (timer.get()-self.tstart)*1E3
    def valTxt(self):
        txt =  'T:' + '%1.0f ' % self.val()*1E3
        return txt

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
    return False

AMAX = 200 # Grad/sec
lst = []

def DriveDist(Vmax,dist,dir):
  global AMAX
  pass

#main main main
console.println('Profile1')
dir=True
while True:
  while not IsButton('a'):
    pass
  DriveDist(1000,2000,dir) # Vmax dist
  txt = '%d %d' % (encL(),encR())
  console.println(txt)
  dir = not dir

# for x in lst:
  # txt = '%1.1f %1.1f' % (x[0],x[1])
  # console.println(txt)
  # sleep(0.5)





  
    
    


    
    
