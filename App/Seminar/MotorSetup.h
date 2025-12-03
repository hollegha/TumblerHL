
// Balance
// const int PWA = 4, AI1 = 32, PWB = 33, BI1 = 5, ST_BY = 18;
// const int M2A = 19, M1A = 23, M2B = 36, M1B = 39;

const int PWA = 4, AI1 = 16, PWB = 18, BI1 = 23, ST_BY = 17;
const int M2A = 19, M1A = 27, M2B = 25, M1B = 26;

// Pinzuordnung unkritisch es werden keine ESP-Funktionen aufgerufen
Motor motL(PWA, AI1, -1); // -1==Tumbler Motor
Motor motR(PWB, BI1, -1);
GpIoOut stdby(ST_BY);
Encoder encL(M2A, M2B, &motL), encR(M1A, M1B, &motR); // 2-Chan Encoder

void InitIO()
{
  stdby.Init(); stdby.Set(1);

  // PWM-Init in dieser Reihenfolge
  // Im mail hast Du das Init nicht für beide Motoren aufgerufen
  motL.inv = true;
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  
  // Beide motoren sicherheitshalber auf 0
  motR.setPow2(0.0); motL.setPow2(0.0);
  
  encL.Init(); encR.Init(); 
  
  // Die .inv Flags kann man immer und überall setzen
  encL.inv = true;
  
  printf("InitIO finished\n");
}

