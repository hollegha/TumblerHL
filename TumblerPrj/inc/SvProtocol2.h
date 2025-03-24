
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stdint.h"
#include "driver/uart.h"

void LibVers();

void InitRtEnvHL();

void InitUart(uart_port_t uaNum, int aBaud);

// void WriteIO(void* aData, size_t aLen);
// int ReadIO(void* aBuff, uint32_t aLen);

class SvProtocol2 {
private:
  static const int BUFF_SZ = 100;
  uint8_t buff[BUFF_SZ];
  int idx;
  uint8_t resetFlag;
public:
  uint8_t acqON;
  int tcpErr;
public:
	SvProtocol2() 
  {
    idx = acqON = 0;
    resetFlag = 1; tcpErr = 2;
  }

  void WriteIO(void* aData, size_t aLen);

  void ReadIO(void* aBuff, uint32_t aLen);

  void Flush()
  {
    WriteIO(buff, idx); idx = 0;
  }

  void CkeckFlush(int aNumBytes)
  {
  }

  void Reset()
  {
    idx = 0;
  }
  
  int GetCommand();

  void SvPrintf(const char* format, ...);

  void SvMessage(const char* aTxt);

  void SvMessage2(const char* aTxt, bool aOnOff);

  void WriteSvI16(int aId, int16_t aData)
  { 
    uint8_t* ptr = buff + idx;
    *ptr=aId+10; ptr++;
    *(int16_t*)ptr = aData;
    idx += 3;
  }
      
  void WriteSvF(int aId, float aData)
  {
    uint8_t* ptr = buff + idx;
    *ptr = aId + 20; ptr++;
    *(float*)ptr = aData;
    idx += 5;
  }
     
  uint8_t ReadByte()
  {
    uint8_t val;
    ReadIO(&val, 1);
    return val;
  }
      
  int16_t ReadI16()
  {
    int16_t val;
    ReadIO(&val, 2);
    return val;
  }

  float ReadI16SC(float aFactor)
  {
    return aFactor * (float)ReadI16();
  }

  float ReadF()
  {
    float val;
    ReadIO(&val, 4);
    return val;
  }

  SvProtocol2& WrB(char aVal)
  {
    WriteIO(&aVal, 1);
    return *this;
  }
  
  SvProtocol2& WrF(float aVal)
  {
    WriteIO(&aVal, 4);
    return *this;
  }
};


// SV-Id Ranges and DataTypes for SvVis3 Visualisation-Tool
//----------------------------------------------------------
// Id = 10       : string
// Id = 1 .. 9   : format 3.13  2 Bytes
// Id = 11 .. 20 : short        2 Bytes
// Id = 21 .. 30 : float        4 Bytes

// Reserved Commands
// 50 - 59: reserved
// 50: reset uC
// 55: JoyStick X Y
// 200: PID-Parameters




