
#pragma once

#ifndef TP_COE_DEFINED
#define TP_COE_DEFINED
#define TP_COE_p100  0.06745527388907,0.13491054777814,0.06745527388907,-1.14298050253990,0.41280159809619
#define TP_COE_p025  0.00554271721028,0.01108543442056,0.00554271721028,-1.77863177782458,0.80080264666571
#define TP_COE_p010  0.00094469184384,0.00188938368768,0.00094469184384,-1.91119706742607,0.91497583480143
#define TP_COE_p05   0.020083365564211,0.040166731128423,0.020083365564211,-1.561018075800718,0.641351538057563
#define TP_COE_p04   0.013359200027856,0.026718400055713,0.013359200027856,-1.647459981076977,0.700896781188403

#define HP_COE_p100  0.638945525159022,-1.277891050318045,0.638945525159022,-1.142980502539901,0.412801598096189
#define HP_COE_p05   0.800592403464570,-1.601184806929141,0.800592403464570,-1.561018075800718,0.641351538057563
#define HP_COE_p025  0.894858606122573,-1.789717212245146,0.894858606122573,-1.778631777824585,0.800802646665708
#define HP_COE_p01   0.956543225556877,-1.913086451113754,0.956543225556877,-1.911197067426073,0.914975834801434
#endif

class Tp1Ord {
private:
  float _alpha;
  float _beta;
public:
  float y; // momentaner ausgangswert des Filtes
public:
  Tp1Ord();

  // Grenzfrequenz verstellen
  void SetAlpha(float aAlpha);

  void CalcOneStep(float aX);
};

class Tp2OrdF {
public:
  float _b0, _b1, _b2, _a1, _a2;
  float xd1, xd2, yd2;
  float yn;
public:
  void Init(float b0, float b1, float b2, float a1, float a2);

  void Reset()
  {
    xd1 = xd2 = yd2 = yn = 0;
  }

  float CalcOneStep(float aX)
  {
    float accu;
    accu = aX * _b0 + xd1 * _b1 + xd2 * _b2
      - yn * _a1 - yd2 * _a2;
    //---Delays-------
    yd2 = yn; yn = accu;
    xd2 = xd1; xd1 = aX;
    return yn;
  }
};

class Tp4OrdF {
public:
  Tp2OrdF f1, f2;
public:
  void Init(float b0, float b1, float b2, float a1, float a2);

  float CalcOneStep(float aX)
  {
    return f2.CalcOneStep(f1.CalcOneStep(aX));
  }
};


inline void Tp2OrdF::Init(float b0, float b1, float b2, float a1, float a2)
{
  _b0 = b0; _b1 = b1; _b2 = b2;
  _a1 = a1; _a2 = a2;
  xd1 = xd2 = yd2 = yn = 0;
}

inline void Tp4OrdF::Init(float b0, float b1, float b2, float a1, float a2)
{
  f1.Init(b0, b1, b2, a1, a2); f2.Init(b0, b1, b2, a1, a2);
}


inline Tp1Ord::Tp1Ord()
{
  y = 0;
  SetAlpha(0.1); // Vernünftiges Alpha setzen
}

inline void Tp1Ord::SetAlpha(float aAlpha)
{
  _alpha = aAlpha;
  _beta = 1.0f - aAlpha;
}

inline void Tp1Ord::CalcOneStep(float aX)
{
  y = _alpha * aX + _beta * y;
}



