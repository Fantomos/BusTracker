#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <OneButton.h>
#include <TFT_eSPI.h>

class Display {
  private:
    TFT_eSPI  _tft;
    uint32 _mainClock;
    uint32 _mainClockLast;
    uint32 _secondaryClock;
    uint32 _secondaryClockLast;
    bool _updateStatus;
    uint32 _updateStatusMillis;
    uint8 _selection;
    uint8 _selectionLast;
    bool _alarmEnabled;
    bool _alarmEnabledLast;

  public:
    void init();
    void showBootMenu();
    void showMainMenu();
    void refresh();
    void setBacklight(bool on);
    void setSleep(bool sleep);
    void setSelection(uint8 selection);
    uint8 getSelection();
    void nextSelection();
    void prevSelection();
    void setAlarmIcon(bool alarm);
    bool getAlarmIcon();
    void setUpdateStatus(bool status);
    void setUpdateStatusMillis(uint32 time);
    void setClock(uint32 m_MainClockVal,uint32 m_SecondaryClockVal);
};

#endif //DISPLAY_H