#include "application.h"
#include "BetterPhotonButton.h"

BetterPhotonButton bb = BetterPhotonButton();

void setup() {
    bb.setup();
    bb.rainbow(250, 2000);
}

void loop() {
    bb.update(millis());
}
