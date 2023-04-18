#include "Arduino.h"
#include "Display.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "icons.h"

#define WAITING_TIME_UNKNWON INT32_MAX
#define PIN_BKL PIN_D1

void Display::init()
{
  // Init variables
  _mainClock = 60;
  _secondaryClock = 60;
  _mainClockLast = 0;
  _secondaryClockLast = 0;
  _alarmEnabled = false;
  _alarmEnabledLast = false;
  _selection = 0;
  _selectionLast = 0;
  _updateStatus = false;
  _updateStatusMillis = 0;

  // Init ST7735S chip
  _tft.init();
  _tft.setRotation(1);

  // Set pin for brightness control
  pinMode(PIN_BKL, OUTPUT);
  setBacklight(false);
}

void Display::showBootMenu()
{
  _tft.fillScreen(TFT_BLACK);
  _tft.fillSmoothRoundRect(_tft.width()/2 - 25, _tft.height()/2 - 25, 50, 50, 5, TFT_GOLD);
}

void Display::showMainMenu()
{
  _tft.fillScreen(TFT_BLACK);
  _tft.drawBitmap(11, 100, _alarmEnabled ? notification_icon : notification_off_icon, notification_icon_width, notification_icon_height, TFT_ORANGE);
  _tft.drawBitmap(49, 100, refresh_icon, refresh_icon_width, refresh_icon_height, TFT_BLUE);
  _tft.drawBitmap(87, 100, setting_icon, setting_icon_width, setting_icon_height, TFT_BROWN);
  _tft.drawBitmap(125, 100, power_icon, power_icon_width, power_icon_height, TFT_RED);
}


void Display::setClock(uint32 m_MainClockVal, uint32 m_SecondaryClockVal)
{
  _mainClock = m_MainClockVal;
  _secondaryClock = m_SecondaryClockVal;
}

void Display::setBacklight(bool on)
{
  digitalWrite(PIN_BKL, on ? LOW : HIGH);
}

void Display::setSleep(bool sleep)
{
  if (sleep)
    {
        _tft.writecommand(0x10);
        delay(150);
    }
    else
    {
        _tft.init();
	      delay(50);   
    }
}

void Display::setSelection(uint8 selection)
{
  _selection = selection;
}

uint8 Display::getSelection()
{
  return _selection;
}

void Display::nextSelection()
{
  _selection++;
  _selection = _selection == 5 ? 0 : _selection;
}

void Display::prevSelection()
{
  _selection--;
  _selection = _selection == 255 ? 4 : _selection;
}

void Display::setAlarmIcon(bool alarm)
{
  _alarmEnabled = alarm;
}

bool Display::getAlarmIcon()
{
  return _alarmEnabled;
}

void Display::setUpdateStatus(bool status)
{
  _updateStatus = status;
}

void Display::setUpdateStatusMillis(uint32 time)
{
  _updateStatusMillis = time;
}

void Display::refresh()
{
  char buf[12];
  if (_mainClockLast != _mainClock)
  {
    if (_mainClock == WAITING_TIME_UNKNWON)
    {
      sprintf(buf, "XX:XX");
    }
    else if (_mainClock > 5999)
    {
      sprintf(buf, "99:59");
    }
    else
    {
      sprintf(buf, "%02u:%02u", _mainClock / 60, _mainClock % 60);
    }

    _tft.setTextColor(TFT_GREEN, TFT_BLACK);
    _tft.setTextDatum(TC_DATUM);
    _tft.drawString(buf, _tft.width() / 2, 5, 6);
    _mainClockLast = _mainClock;
  }

  if (_secondaryClockLast != _secondaryClock)
  {
    if (_secondaryClock == WAITING_TIME_UNKNWON)
    {
      sprintf(buf, "??:??");
    }
    else if (_secondaryClock > 5999)
    {
      sprintf(buf, "99:59");
    }
    else
    {
      sprintf(buf, "%02u:%02u", _secondaryClock / 60, _secondaryClock % 60);
    }

    _tft.setTextColor(TFT_DARKGREEN, TFT_BLACK);
    _tft.setTextDatum(TC_DATUM);
    _tft.drawString(buf, _tft.width() / 2, 52, 6);
    _secondaryClockLast = _secondaryClock;
  }

  if (_selectionLast != _selection)
  {
      _tft.drawBitmap(11, 100, _alarmEnabled ? notification_icon : notification_off_icon, notification_icon_width, notification_icon_height, _selection == 1 ? TFT_WHITE : TFT_ORANGE);
      _tft.drawBitmap(49, 100, refresh_icon, refresh_icon_width, refresh_icon_height, _selection == 2 ? TFT_WHITE : TFT_BLUE);
      _tft.drawBitmap(87, 100, setting_icon, setting_icon_width, setting_icon_height, _selection == 3 ? TFT_WHITE : TFT_BROWN);
      _tft.drawBitmap(125, 100, power_icon, power_icon_width, power_icon_height, _selection == 4 ? TFT_WHITE : TFT_RED);
      _selectionLast = _selection;
  }

  if (_alarmEnabledLast != _alarmEnabled)
  {
    if (_alarmEnabled) _tft.fillRect(11, 100, notification_icon_width, notification_icon_height, TFT_BLACK);
    _tft.drawBitmap(11, 100, _alarmEnabled ? notification_icon : notification_off_icon, notification_icon_width, notification_icon_height, _selection == 1 ? TFT_WHITE : TFT_ORANGE);
    _alarmEnabledLast = _alarmEnabled;
  }

  if (millis() < _updateStatusMillis + 5000)
  {
    _tft.fillCircle(5,5,3, _updateStatus ? TFT_GREEN : TFT_RED);
  }
  else if(millis() < _updateStatusMillis + 6000)
  {
    _tft.fillCircle(5,5,3, TFT_BLACK);
  }
}
