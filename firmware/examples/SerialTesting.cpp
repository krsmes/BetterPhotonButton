#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

BetterPhotonButton bb = BetterPhotonButton();
PhotonADXL362Accel *accel = NULL;

void buttonHandler(int button, bool state) {
    Serial.printlnf("Button %d %s", button, state?"pressed":"released");
}

void motionHandler(bool motion, unsigned long after) {
    Serial.printlnf("Became %s after %d millis", motion?"in motion":"still", after);
}


void setup() {
    Serial.begin(9600);
    bb.setup();
    bb.setPressedHandler(&buttonHandler);
    bb.setReleasedHandler(&buttonHandler);
    accel = bb.startAccelerometer();
    accel->setMotionHandler(&motionHandler);
    Serial.println("Serial Testing of BetterPhotonButton started.");
}

int lastRefresh;
int refreshRate = 1000/10;

void loop() {
    int current = millis();
    bb.update(current);


    if (current - lastRefresh > refreshRate) {
        bb.setPixels(0);
        lastRefresh = current;

        if (accel->state == accel->CALIBRATING) {
            Serial.printlnf("CALIBRATING x=%5d, y=%5d, z=%5d", accel->x, accel->y, accel->z);
        }

        if (accel->inMotion()) {
            int az = accel->getAzimuth();
            Serial.printlnf("x=%5d, y=%5d, z=%5d, pitch=%f, roll=%f, angle=%d, azimuth=%d",
                            accel->x, accel->y, accel->z,
                            accel->getPitch(), accel->getRoll(),
                            accel->getAngle(), az);

            int top = (az - 6)/ 30;
            bb.setPixel(top, 0xFF00FF);
        }
//        Serial.printlnf("temperatureRaw=%d", bb.getTemperatureRaw());
    }
}
