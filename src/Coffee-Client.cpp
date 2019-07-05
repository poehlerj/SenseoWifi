//
// Created by jpohler on 27/04/19.
//

#include <ESP8266WiFi.h>
#include "Coffee-Client.h"
#include "pins.h"

#include "Arduino.h"
#include "SenseoLed.h"
#include "SenseoSM.h"
#include "SenseoControl.h"

SenseoLed mySenseoLed(ocSenseLedPin);
SenseoSM mySenseoSM;
SenseoControl myControl(ocPressPowerPin, ocPressLeftPin, ocPressRightPin);

bool statusUpdated = false;

void updateAllStates() {
    mySenseoLed.updateState();
    if (mySenseoLed.hasChanged()) {
        mySenseoSM.updateState(mySenseoLed.getState());
        if (mySenseoSM.stateHasChanged()) {
            statusUpdated = true;
        }
    }
}

/**
* Called by the LED changed interrupt
*/
void ledChangedHandler() {
    mySenseoLed.pinStateToggled();
    updateAllStates();
}

void setup(void) {

    /**
    * pin initializations
    */
    pinMode(ocPressLeftPin, OUTPUT);
    pinMode(ocPressRightPin, OUTPUT);
    pinMode(ocPressPowerPin, OUTPUT);
    pinMode(ocSenseLedPin, INPUT_PULLUP);

    digitalWrite(ocPressPowerPin, LOW);
    digitalWrite(ocPressLeftPin, LOW);
    digitalWrite(ocPressRightPin, LOW);

    pinMode(beeperPin, OUTPUT);

    attachInterrupt(ocSenseLedPin, ledChangedHandler, CHANGE);

    Serial.begin(115200);
}

void loopConnection(WiFiClient &client) {
    updateAllStates();

    if (statusUpdated) {
        client.write(mySenseoSM.getState() + '0');
        statusUpdated = false;
    }

    myControl.releaseIfPressed();
    if (client.available()) {
        int state = client.read() - '0';
        Serial.printf("State %d\n", mySenseoSM.getState());
        switch (state) {
            case POWER_ON:
                Serial.println("POWER_ON");
                if (mySenseoSM.getState() == SENSEO_OFF) {
                    myControl.pressPowerButton();
                }
                break;
            case POWER_OFF:
                Serial.println("POWER_OFF");
                if (mySenseoSM.getState() != SENSEO_OFF && mySenseoSM.getState() != SENSEO_unknown) {
                    myControl.pressPowerButton();
                }
                break;
            case BREW_1_CUP:
                Serial.println("BREW_1_CUP");
                if (mySenseoSM.getState() == SENSEO_READY) {
                    myControl.pressLeftButton();
                }
                break;
            case BREW_2_CUPS:
                Serial.println("BREW_2_CUPS");
                if (mySenseoSM.getState() == SENSEO_READY) {
                    myControl.pressRightButton();
                }
                break;

        }
        Serial.println(state);
    }
}

void loop(void) {
    WiFi.disconnect(); // remove old credentials they say
    delay(500);
    WiFi.mode(WIFI_OFF);
    delay(500);
    WiFi.mode(WIFI_STA);
    WiFi.begin("WiFi-AP", "WiFi-PW");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    tone(beeperPin, 1024, 1000);

    while (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;

        IPAddress address;
        address.fromString("Insert IP-Address");

        client.setNoDelay(true);
        client.connect(address, 8080);

        if (client.connected()) {
            tone(beeperPin, 1024, 500);
            delay(500);
            tone(beeperPin, 1024, 500);
        }

        while (client.connected()) {
            loopConnection(client);
        }
        delay(500);
    }

}