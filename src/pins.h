/*
config.h - Configuration for the SenseoWifi project.
Created by Thomas Dietrich, 2016-03-05.
Released under some license.
*/

#ifndef SenseoPins_h
#define SenseoPins_h

// circuit pin definitions      // PCB v1.6
#define ocPressLeftPin D2       // T1
#define ocPressRightPin D0      // T2
#define ocPressPowerPin D1      // TP
#define ocSenseLedPin D7        // LED
#define cupDetectorPin D6       // TCRT5000 D0
#define beeperPin D8            // BUZZER
#define resetButtonPin D4       //
#define cupDetectorAnalogPin A0 // TCRT5000 A0

// Reset: D3 doesn't work (why?), D0 okay, D4 needs LED off

#endif
