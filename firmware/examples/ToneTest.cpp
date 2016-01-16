#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);


BetterPhotonButton bb = BetterPhotonButton();

void buttonHandler(int button, bool state) {
    String note;
    if (button == 0) { note = "C:1"; }
    if (button == 1) { note = "A"; }
    if (button == 2) { note = "C#:8"; }
    if (button == 3) { note = ":8,C-,E-,G-,C,G:4"; }
    bb.playNotes(note);
}

void setup() {
    Serial.begin(9600);
    bb.setup();
    bb.setReleasedHandler(&buttonHandler);
    bb.startPixelAnimation(&animation_bounce); // to show the playing of notes does not interrupt pixel animations an vice-versa
}

void loop() {
    bb.update(millis());
}
