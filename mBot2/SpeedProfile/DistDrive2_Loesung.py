
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
    def valMS(self):
        return (timer.get()-self.tstart)*1E3
    def valTxt(self):
        txt =  'T:' + '%1.0f ' % self.val()*1E3
        return txt

class RateLim:
  def __init__(self, rate):
    self.rate = rate #rate in sec
    self.inp=0
    self.outp=0
    self.stw=Stw()
  def start(self):
    self.stw.reset()
  def step(self):
    dt=self.stw.val(); self.stw.reset()
    if self.outp==self.inp:
      return False # finished
    if self.outp<self.inp: # ramp up
      self.outp += self.rate * dt
      if self.outp>=self.inp:
        self.outp=self.inp # up finished
        return False
      return True
    # else outp>inp ramp down
    self.outp -= self.rate * dt
    if self.outp<=self.inp:
      self.outp=self.inp # down finished
      return False
    return True


AMAX1=50; AMAX2=200

def DriveDist(Vmax,dist):
  global AMAX1, AMAX2
  resetEnc(1); resetEnc(2)
  rt=RateLim(AMAX1); rt.inp=Vmax; rt.start()
  while encL()<dist: # UP
    rt.step()
    motL(rt.outp); motR(rt.outp)
    sleep(0.01)
  rt.rate=AMAX2; rt.inp=0
  while rt.step(): # Down
    motL(rt.outp); motR(rt.outp)
    sleep(0.01)
  motL(0); motR(0)

#main main main
console.println('RampDrive')
while not IsButton('a'):
  pass
DriveDist(190,2000) # Vmax dist
txt = '%d %d' % (encL(),encR())
console.println(txt)





  
    
    


    
    
