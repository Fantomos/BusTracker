#include "TimeTable.h"
#include "Display.h"
#include "Controller.h"
#include "Alarm.h"
#include "user_interface.h"

#define PIN_ENCODER_CLK PIN_D2
#define PIN_ENCODER_DT  PIN_D3
#define PIN_ENCODER_SW  PIN_D4
#define PIN_BUZZER PIN_D9

#define UPDATE_RATE_DEFAULT 120000 // in ms
#define TICK_RATE_DEFAULT 1000 // in ms
#define TIME_MIN_DEFAULT 420 // in sec
#define SELECTION_TIMEOUT 5000 // in ms
#define POWER_TIMEOUT 1200000// in ms
#define TIME_ALARM 10 // in sec

const char apiRequest[] = "http://api.tisseo.fr/v2/stops_schedules.json?timetableByArea=1&lineId=line:170&stopPointId=stop_point:SP_1423&number=3&key=REDACTED";
const char ssid[] = "REDACTED";
const char password[] = "REDACTED";

Alarm alarm;
TimeTable timeTable;
Display display;
Controller controller(PIN_ENCODER_CLK, PIN_ENCODER_DT,PIN_ENCODER_SW);

void light_sleep() {
  Serial.println("sleep started");
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  gpio_pin_wakeup_enable(PIN_D3, GPIO_PIN_INTR_LOLEVEL);
  wifi_fpm_do_sleep(0xFFFFFFF); // sleep for longest possible time
  ESP.restart();
}

void setup()
{
  // Init display first to show boot menu
  display.init();
  display.showBootMenu();
  display.setBacklight(true);

  // Init other modules 
  alarm.init(PIN_BUZZER);
  alarm.playBootSound();
  controller.init();

  timeTable.init(apiRequest, ssid, password, UPDATE_RATE_DEFAULT, TICK_RATE_DEFAULT);
  timeTable.connect();

  // Wait for WIFI connect
  while (!timeTable.isConnected())
  {
    alarm.tick();
    delay(100);
  }
  
  // Boot finished, show main menu
  timeTable.update();
  display.showMainMenu();
}

void loop()
{
  // Update tick modules
  alarm.tick();
  timeTable.tick();
  controller.tick();


  // Update display
  display.setClock(timeTable.getFirstWaitingTime(TIME_MIN_DEFAULT),timeTable.getSecondWaitingTime(TIME_MIN_DEFAULT));

  // Select new element if rotation
  switch(controller.getDirection())
  {
    case -1: // Counter-clockwise rotation, decrement selection
      display.prevSelection();
      break;

    case 1: // Clockwise rotation, increment selection
      display.nextSelection();
      break;

    default: // No rotation, do nothing
      break;
  }

  // Do selected action if clicked
  if(controller.isSingleClick())
  {
    alarm.playBipSound();
    switch (display.getSelection())
    {
      case 0: // SELECTION OFF
        
        break;

      case 1: // SELECTION NOTIF
        display.setAlarmIcon(!display.getAlarmIcon());
        // TODO ALARM
        break;

      case 2: // SELECTION REFRESH
        timeTable.update();
        break;

      case 3: // SELECTION SETTING

        break;

      case 4: // SELECTION POWER
        display.setBacklight(false);
        display.setSleep(true);
        //ESP.deepSleep(10);
        light_sleep();
        break;
      
      default:
        break;
    }
  }



  uint32 time = millis();
  // Check if timeout on selection
  if(time > controller.getLastActionMillis() + SELECTION_TIMEOUT)
  {
    display.setSelection(0);
  }

  // Check if timeout on power
  if(time > controller.getLastActionMillis() + POWER_TIMEOUT)
  {
    ESP.deepSleep(0);
  }

  if(display.getAlarmIcon() && timeTable.getFirstWaitingTime(TIME_MIN_DEFAULT) == TIME_ALARM)
  {
    alarm.playAlarmSound();
  }

  display.setUpdateStatus(timeTable.isLastUpdateSuccess());
  display.setUpdateStatusMillis(timeTable.getUpdateTime());
  display.refresh(); 
}