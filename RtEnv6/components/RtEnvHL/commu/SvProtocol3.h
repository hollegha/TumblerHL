
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stdint.h"
#include "driver/uart.h"

#define USE_UDP 

void MyError(const char* txt);

extern "C" void print_task_list();

extern "C" void wifi_disconnect_impl();

inline void wifi_disconnect()
{
#ifdef USE_UDP
  wifi_disconnect_impl();
  #warning "!!USE_UDP!!"
#endif
}

void InitStation();
void InitSoftAp(const char* aName, int aChan);

void InitUart(uart_port_t uaNum, int aBaud);

void cycleBlink(int rateReduce);

class UdpRd {
public:
  int rdLen;
private:
  static const int BUFF_SZ = 100;
  uint8_t buff[BUFF_SZ];
  int rdIdx;
public:
  void ReadMsg();

  uint8_t ReadByte()
  {
    uint8_t val = buff[rdIdx];
    rdIdx++;
    return val;
  }

  int16_t ReadI16()
  {
    int16_t* ptr = (int16_t*)&buff[rdIdx];
    rdIdx += 2;
    return *ptr;
  }

  void ReadI16Ary(int16_t* ary, int len)
  {
    int16_t* ptr = (int16_t*)&buff[rdIdx];
    for (int i = 0; i < len; i++)
      ary[i] = *ptr++;
    rdIdx += 2 * len;
  }

  float ReadF()
  {
    float* ptr = (float*)&buff[rdIdx];
    rdIdx += 4;
    return *ptr;
  }
};


class SvProtocol3 {
private:
  static const int BUFF_SZ = 256;
  uint8_t buff[BUFF_SZ];
  int idx;
  int roundCnt;
  uint8_t resetFlag;
#ifdef USE_UDP
  UdpRd udp;
#endif 
public:
  uint8_t acqON;
  int sendTim;
  int maxSendTim;
public:
  SvProtocol3()
  {
    idx = acqON = 0;
    resetFlag = 1;
    sendTim = maxSendTim = roundCnt = 0;
  }

  static void Init();

  void LockOStream();

  void UnlockOStream();

  void WriteIO(void* aData, size_t aLen);

  int ReadIO(void* aBuff, uint32_t aLen);

  void Flush()
  {
    WriteIO(buff, idx); idx = 0;
  }

  void Flush2(int aNumBytes)
  {
    if (idx >= aNumBytes)
      Flush();
  }

  void Flush3(int aNumRounds)
  {
    roundCnt++;
    if (roundCnt >= aNumRounds) {
      WriteIO(buff, idx); idx = 0;
      roundCnt = 0;
    }
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
    *ptr = aId + 10; ptr++;
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
#ifdef USE_UDP
    return udp.ReadByte();
#else
    uint8_t val;
    ReadIO(&val, 1);
    return val;
#endif
  }

  int16_t ReadI16()
  {
#ifdef USE_UDP
    return udp.ReadI16();
#else
    int16_t val;
    ReadIO(&val, 2);
    return val;
#endif
  }

  void ReadI16Ary(int16_t* ary, int len)
  {
#ifdef USE_UDP
    return udp.ReadI16Ary(ary,len);
#else
    ReadIO(ary, 2 * len);
#endif
  }

  SvProtocol3& rdI16(int& val)
  {
    val = ReadI16();
    return *this;
  }
  
  float ReadI16SC(float aFactor)
  {
    return aFactor * (float)ReadI16();
  }

  float ReadF()
  {
#ifdef USE_UDP
    return udp.ReadF();
#else
    float val;
    ReadIO(&val, 4);
    return val;
#endif
  }

  SvProtocol3& rdF(float& val)
  {
    val = ReadF();
    return *this;
  }

  SvProtocol3& WrB(uint8_t aVal)
  {
    uint8_t* ptr = buff + idx;
    *ptr = aVal; idx++;
    return *this;
  }

  SvProtocol3& WrF(float aVal)
  {
    uint8_t* ptr = buff + idx;
    *(float*)ptr = aVal; idx += 4;
    return *this;
  }

  SvProtocol3& WrStr(const char* aStr);
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
// 52: disconnect WiFi
// 55: JoyStick X Y
// 200: PID-Parameters




