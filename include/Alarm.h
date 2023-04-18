// Alarm.h
#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

class Alarm {
  private:
  uint8 _pinBuzzer;
  const uint16* _melodyNote;
  const uint16* _melodyDuration;
  uint8 _melodyIndex;
  const uint8* _melodyLength;
  uint32 _lastMillis;

  public:
  void init(uint8 pin);
  void playBootSound();
  void playAlarmSound();
  void playBipSound();
  void tick();
};

#endif // #define ALARM_H