#include "Particle.h"
#include "BetterPhotonButton.h"

/* print out the button and its state */
void buttonHandler(int button, bool state) {
    Serial.printlnf("Button %d %s", button, state?"pressed":"released");
}

/* print out if motion started or stopped and after how many milliseconds */
void motionHandler(bool motion, unsigned long after) {
    Serial.printlnf("Became %s after %d millis", motion?"in motion":"still", after);
}


BetterPhotonButton bb = BetterPhotonButton();
PhotonADXL362Accel *accel = NULL;

void update() {
    bb.setPixels(0); // set all pixels to off

    if (accel->state == accel->CALIBRATING) {
        Serial.printlnf("CALIBRATING x=%5d, y=%5d, z=%5d", accel->x, accel->y, accel->z);
    }

    if (accel->inMotion()) {
        // azimuth provides a position of the button in 0..360 degrees where 0 is at the top and 90 is to the right
        int az = accel->getAzimuth();
        Serial.printlnf("x=%5d, y=%5d, z=%5d, pitch=%f, roll=%f, angle=%d, azimuth=%d",
                        accel->x, accel->y, accel->z,
                        accel->getPitch(), accel->getRoll(),
                        accel->getAngle(), az);

        // convert azimuth (0..359, 0=top, 90=right, etc) to a pixel location.
        // The arc of one pixel is 30 degrees (360 / 12 pixel locations = 30)
        int top = (az - 15) / 30; // subtract 1/2 an arc (15 degrees) for rounding, then divide by arc of one pixel
        // set the top-most pixel to magenta
        bb.setPixel(top, PixelColor::MAGENTA);
    }
}


/*
 * setup/loop
 */

void setup() {
    Serial.begin(9600);

    bb.setup();  // always call BetterPhotonButton::setup() in setup
    bb.setPressedHandler(&buttonHandler);  // all button presses go to the buttonHandler()
    bb.setReleasedHandler(&buttonHandler);  // all button releases go to the buttonHandler()
    accel = bb.startAccelerometer();  // accelerometer must be started (and retrieved) separately from setup()
    accel->setMotionHandler(&motionHandler);  // all motion activations/deactivations go to motionHandler()

    Serial.println("Serial Testing of BetterPhotonButton started.");
}

system_tick_t lastRefresh;
int refreshRate = 1000/10;

void loop() {
    system_tick_t current = millis();
    bb.update(current);  // always call BetterPhotonButton::update at/near the top of the loop

    // call update every 'refreshRate' milliseconds
    if (current - lastRefresh > refreshRate) {
        lastRefresh = current;
        update();
    }
}
