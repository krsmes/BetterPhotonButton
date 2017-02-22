#include "Particle.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

BetterPhotonButton bb = BetterPhotonButton();

void pressed(int button, bool state) {
    if (button == 0) { bb.setPixels(PixelColor::IVORY); }
    else if (button == 1) { bb.setPixels(PixelColor::LINEN); }
    else if (button == 2) { bb.setPixels(PixelColor::BEIGE); }
    else if (button == 3) { bb.setPixels(PixelColor::KHAKI); }
}

void released(int button, bool state) { bb.setPixels(PixelColor::OFF); }

void setup() {
    bb.setup();
    bb.setPressedHandler(pressed);
    bb.setReleasedHandler(released);
}

void loop() {
    bb.update(millis());
}
