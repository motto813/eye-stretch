/*
 * Project eye-stretch
 * Description: Reminds to stretch the eyes on an interval
 * Author: Paul Hammerschmidt
 * Date: 08/05/18
 */

 #include "SparkIntervalTimer.h"
 #include "InternetButton.h"

 class EyeSaver {
   public:
     EyeSaver() {
       Particle.variable("isRunning", EyeSaver::isRunning);
       Particle.variable("currentLed", EyeSaver::currentLed);
       Particle.variable("isAlarming", EyeSaver::isAlarming);
       Particle.variable("totalMin", EyeSaver::totalMin);

       Particle.function("setTotalMin", &EyeSaver::setTotalMin, this);
     }

     const static int ticksPerLed = 167;

     bool isRunning = false;
     int currentLed = 0;
     bool isAlarming = false;
     int totalMin = 1;

     int setTotalMin(String inputMin) {
       int parsedTotal = inputMin.toInt();
       if (parsedTotal >= 1 && parsedTotal <= 60) {
         totalMin = parsedTotal;
       }
     }

     void increment(InternetButton b) {
       isRunning = true;
       currentLed++;
       if (currentLed == 12) {
         soundAlarm(b);
         isRunning = false;
       } else {
         b.smoothLedOn(currentLed, 0, 0, 20);
       }
     }

     void stopAndReset(InternetButton b) {
       b.allLedsOff();
       isRunning = false;
       currentLed = 0;
       isAlarming = false;
     }

     void soundAlarm(InternetButton b) {
       b.allLedsOn(20, 0, 0);
       isAlarming = true;
     }
 };

 InternetButton b = InternetButton();
 EyeSaver myEyeSaver;
 IntervalTimer myTimer;
 int tickCounter;
 bool alarmTriggered;

 int pushButton(String cmd) {
   Particle.publish("button-pushed", PRIVATE);
   if (myEyeSaver.isRunning || myEyeSaver.isAlarming) {
     myEyeSaver.stopAndReset(b);
     myTimer.end();
   } else {
     tickCounter = 0;
     myTimer.begin(incrementSaver, myEyeSaver.totalMin * 60, hmSec);
     myEyeSaver.increment(b);
   }
   return 1;
 }

 void incrementSaver() {
   tickCounter++;
   if (tickCounter >= EyeSaver::ticksPerLed) {
     myEyeSaver.increment(b);
     if (myEyeSaver.isAlarming) {
       alarmTriggered = true;
       myTimer.end();
     }
     tickCounter = 0;
   }
 }

 void setup() {
   b.begin();
   b.allLedsOff();
   Particle.function("pushButton", pushButton);
   alarmTriggered = false;
 }

 void loop() {
   if (!b.allButtonsOff()) {
     if (b.buttonOn(1)) {
       b.ledOn(1, 0, 50, 0);
       b.ledOn(11, 0, 50, 0);
     } else if (b.buttonOn(2)) {
       b.ledOn(3, 0, 50, 0);
     } else if (b.buttonOn(3)) {
       b.ledOn(6, 0, 50, 0);
     } else if (b.buttonOn(4)) {
       b.ledOn(9, 0, 50, 0);
     }
     delay(250);
     b.allLedsOff();
     pushButton("");
     delay(500);
   }

   if (alarmTriggered) {
     Particle.publish("alarm-triggered", PRIVATE);
     alarmTriggered = false;
     delay(500);
   }
 }
