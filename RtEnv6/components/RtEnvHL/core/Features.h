
#pragma once
#include "stdint.h"

const uint32_t SVVIS_BAS = 0x00000001;
const uint32_t SVVIS_ADV1 = 0x00000002;
const uint32_t SVVIS_ADV2 = 0x00000004;
const uint32_t SVVIS_ADV3 = 0x00000008;

const uint32_t LIB_BAS = 0x00000010;
const uint32_t FILT_1 = 0x00000020;
const uint32_t FILT_2 = 0x00000040;

const uint32_t FEAT_SEM = 0x80000000;

#ifndef MY_ERR_DEFINED
#define MY_ERR_DEFINED
extern uint32_t gFeatures;
void MyError(const char* aMsg);
#endif

inline bool ChkFeature(uint32_t aKey)
{
  return (gFeatures & aKey);
}

inline void ChkFeature2(uint32_t aKey)
{
  if (!(gFeatures & aKey))
    MyError("Not Supported");
}
