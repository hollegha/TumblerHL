#pragma once

class TTMailBox {
public:
  TaskHandle_t t_recv;
public:
  TTMailBox()
  {
    _msg = 0;
    t_recv = (TaskHandle_t)0;
  }
  void clear()
  {
    _msg = 0;
  }
  void send(int aMsg)
  {
    if (t_recv == 0)
      return;
    _msg = aMsg;
    xTaskNotifyGive(t_recv);
  }
  int recv()
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    int ret = _msg;
    _msg = 0;
    return ret;
  }
  // set value NO trigger
  void set(int aVal)
  {
    _msg = aVal;
  }
  // check only non blocking
  bool check(int aVal)
  {
    return _msg == aVal;
  }
  bool doCancel()
  {
    return _msg == -1;
  }
private:
  int _msg;
};

