#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <OneButton.h>
#include <TFT_eSPI.h>

class Controller {
  private:
    static Controller * _instance;
    RotaryEncoder _encoder;
    OneButton _button;
    bool _singleClick;
    bool _longPress;
    uint32 _lastActionMillis;
    static void singleClick();
    static void longPress();
    static void checkPosition();

  public:
    Controller(uint8 pinCLK,uint8 pinDT, uint8 pinSW);
    void init();
    int8 getDirection();
    uint32 getLastActionMillis();
    bool isSingleClick();
    bool isLongPress();
    void tick();
};

#endif