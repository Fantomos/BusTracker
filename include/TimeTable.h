// TimeTable.h
#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <Arduino.h>

#define WAITING_TIME_NB 3

class TimeTable {
  private:
    char _apiRequest[200];
    char _SSID[50];
    char _PSWD[50];
    int32 _waitingTime[WAITING_TIME_NB];
    uint32 _lastUpdate;
    uint32 _lastTick;
    uint32 _updateRate;
    uint32 _tickRate;
    bool _lastUpdateSuccess;

  public:
    void init(const char *apiRequest, const char *ssid, const char *password, uint32 updateRate, uint32 tickRate);
    void tick();
    void connect();
    void update();
    bool isConnected();
    void setUpdateRate(uint16 newUpdateRate);
    void setTickRate(uint16 newTickRate);
    void setWIFICredential(const char* ssid, const char* password);
    int32 getFirstWaitingTime(int32 minTime);
    int32 getSecondWaitingTime(int32 minTime);
    uint32 getUpdateTime();
    bool isLastUpdateSuccess();
};

#endif // #define TIMETABLE_H