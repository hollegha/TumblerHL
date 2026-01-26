
// Balance
// const int PWA = 4, AI1 = 32, PWB = 33, BI1 = 5, ST_BY = 18;
// const int M2A = 19, M1A = 23, M2B = 36, M1B = 39;

const int PWA = 4, AI1 = 16, PWB = 18, BI1 = 23, ST_BY = 17;
const int M2A = 19, M1A = 27, M2B = 25, M1B = 26;

// Pinzuordnung unkritisch es werden keine ESP-Funktionen aufgerufen

// kleine Räder
Motor motR(PWA, AI1, -1);
Encoder2 encR(M2A, M2B);
Motor motL(PWB, BI1, -1);
Encoder2 encL(M1A, M1B);

GpIoOut stdby(ST_BY);

void InitMotors()
{
  stdby.Init(); stdby.Set(1);
  Motor::InitTimer(); motR.Init(); motL.Init(); Motor::StartTimer();
  motR.setPow2(0.0); motL.setPow2(0.0);
  
  encL.Init(); encR.Init();
  encR.inv = true; motR.inv = true; // kleine Räder
  // encR.inv = true; motR.inv = true; // ls back
  
  // Die .inv Flags kann man immer und überall setzen
  // printf("InitMotors finished\n");
}

