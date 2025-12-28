
#pragma once

#ifndef MY_ERR_DEFINED
#define MY_ERR_DEFINED
extern uint32_t gFeatures;
void MyError(const char* aMsg);
#endif

#define MY_ERR1(func)  if( ESP_OK!=func ) MyError("ReadNVS")

void fls();

void PrintMac(const char* aTxt, uint8_t addr[]);

void OpenNVS();

void CloseNVS();

void Mac2NVS(uint32_t aFeatures);

void ShowMacAll();

void EraseNVS();

void ShowMac();

void CheckNodeLock();

