#include "Particle.h"
#include "BetterPhotonButton.h"

BetterPhotonButton bb = BetterPhotonButton();

void setup() {
    bb.setup();
    bb.rainbow(250); // cycle rainbow every 250ms (indefinitely)
}

void loop() {
    bb.update(millis());
}
