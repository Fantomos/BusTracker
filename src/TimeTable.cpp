#include "TimeTable.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <time.h>


#define WAITING_TIME_UNKNWON INT32_MAX

void TimeTable::init(const char *apiRequest, const char *ssid, const char *password, uint32 updateRate, uint32 tickRate)
{
    strncpy(_apiRequest, apiRequest, 200);
    setWIFICredential(ssid, password);
    for (uint8 i = 0; i < WAITING_TIME_NB; i++)
    {
        _waitingTime[i] = WAITING_TIME_UNKNWON;
    }
    _lastUpdate = 0;
    _updateRate = updateRate;
    _tickRate = tickRate;
}

void TimeTable::connect()
{
    WiFi.begin(_SSID, _PSWD);
}

bool TimeTable::isConnected()
{
    return (WiFi.status() == WL_CONNECTED);
}

uint32 TimeTable::getUpdateTime()
{
    return _lastUpdate;
}

bool TimeTable::isLastUpdateSuccess()
{   
    return _lastUpdateSuccess;
}

void TimeTable::update()
{
    _lastUpdateSuccess = true;
    
    if (WiFi.status() != WL_CONNECTED)
    {
        _lastUpdateSuccess = false;
        return;
    }
    // If WIFI connected
    WiFiClient client;
    HTTPClient http;

    // Tisseo api request
    http.begin(client, _apiRequest);

    // Send HTTP GET request
    if (http.GET() != 200)
    {
        _lastUpdateSuccess = false;
        return;
    }

    // If response is OK
    StaticJsonDocument<1024> doc;
    StaticJsonDocument<256> filter;
    filter["departures"]["stopAreas"][0]["schedules"][0]["journeys"] = true;

    if (deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter)))
    {
        _lastUpdateSuccess = false;
        return;
    }

    uint8 waitingTime_hours;
    uint8 waitingTime_minutes;
    uint8 waitingTime_seconds;

    for (uint8 i = 0; i < WAITING_TIME_NB; i++)
    {
        if (3 == sscanf(doc["departures"]["stopAreas"][0]["schedules"][0]["journeys"][i]["waiting_time"], "%hhu:%hhu:%hhu", &waitingTime_hours, &waitingTime_minutes, &waitingTime_seconds))
        {
            _waitingTime[i] = waitingTime_seconds + waitingTime_minutes * 60 + waitingTime_hours * 3600;
        }
        else{
            _waitingTime[i] = WAITING_TIME_UNKNWON;
            _lastUpdateSuccess = false;
        }
    }
    http.end();

    _lastUpdate = millis();
    return;
}

void TimeTable::tick()
{
    uint32 currentMillis = millis();
    if ((currentMillis - _lastUpdate) > _updateRate)
    {
        this->update();
    }

    currentMillis = millis();
    if ((currentMillis - _lastTick) > _tickRate)
    {
        for (uint8 i = 0; i < WAITING_TIME_NB; i++)
        {
            if(_waitingTime[i] != WAITING_TIME_UNKNWON && _waitingTime[i] > 0)
            {
                _waitingTime[i] -= (currentMillis - _lastTick) / 1000;
            }
        }
        _lastTick = currentMillis;
    }
}

void TimeTable::setUpdateRate(uint16 newUpdateRate)
{
    _updateRate = newUpdateRate;
}

void TimeTable::setTickRate(uint16 newTickRate)
{
    _tickRate = newTickRate;
}

void TimeTable::setWIFICredential(const char *ssid, const char *password)
{
    strncpy(_SSID, ssid, 50);
    strncpy(_PSWD, password, 50);
}

int32 TimeTable::getFirstWaitingTime(int32 minTime)
{
    for (uint8 i = 0; i < WAITING_TIME_NB; i++)
    {
        if(_waitingTime[i] != WAITING_TIME_UNKNWON && _waitingTime[i] > minTime)
        {
            return _waitingTime[i] - minTime;
        }
    }

    return WAITING_TIME_UNKNWON;
}

int32 TimeTable::getSecondWaitingTime(int32 minTime)
{
    bool firstFound = false;
    for (uint8 i = 0; i < WAITING_TIME_NB; i++)
    {
        if(_waitingTime[i] != WAITING_TIME_UNKNWON && _waitingTime[i] > minTime)
        {
            if(!firstFound)
            {
                firstFound = true;
            }
            else
            {
                return _waitingTime[i] - minTime;
            }
        }
    }

    return WAITING_TIME_UNKNWON;
}