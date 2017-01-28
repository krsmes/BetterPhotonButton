#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

BetterPhotonButton bb = BetterPhotonButton();

void buttonHandler(int button, bool state) {
    if (button == 0) { bb.playNotes("C:1"); }
    else if (button == 1) { bb.playNotes("A"); }
    else if (button == 2) { bb.playNotes("C#:8"); }
    else if (button == 3) { bb.playNotes(":8,C-,E-,G-,C,G:4"); }
}

void setup() {
    bb.setup();
    bb.setReleasedHandler(&buttonHandler);
    bb.startPixelAnimation(&animation_bounce); // the playing of notes does not interrupt pixel animations an vice-versa
}

void loop() {
    bb.update(millis());
}
