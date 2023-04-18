#include "Arduino.h"
#include "Controller.h"
#include <RotaryEncoder.h>
#include <OneButton.h>

Controller * Controller::_instance;

Controller::Controller(uint8 pinCLK,uint8 pinDT, uint8 pinSW):
    _encoder(pinCLK, pinDT, RotaryEncoder::LatchMode::FOUR3),
    _button(pinSW,true,true)
{
  _instance = this;
  attachInterrupt(digitalPinToInterrupt(pinCLK), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinDT), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinSW), checkPosition, CHANGE);
}

void Controller::init()
{
  _lastActionMillis = 0;
  _singleClick = false;
  _longPress = false;
  _button.attachClick(singleClick);
  _button.setPressTicks(3000);
  _button.attachLongPressStart(longPress);
}

IRAM_ATTR void Controller::checkPosition()
{
   _instance->tick();
   _instance->_lastActionMillis = millis();
}

void Controller::singleClick()
{
  _instance->_singleClick = true;
  _instance->_lastActionMillis = millis();
}

void Controller::longPress()
{
  _instance->_longPress = true;
  _instance->_lastActionMillis = millis();
}

uint32 Controller::getLastActionMillis()
{
  return _lastActionMillis;
}

void Controller::tick()
{
  _button.tick();
  _encoder.tick();
}

int8 Controller::getDirection()
{
  return (int8) _encoder.getDirection();
}

bool Controller::isSingleClick()
{
  if(_singleClick)
  {
    _singleClick = false;
    return true;
  }
  else
  {
    return false;
  }
}

bool Controller::isLongPress()
{
  if(_longPress)
  {
    _longPress = false;
    return true;
  }
  else
  {
    return false;
  }
}