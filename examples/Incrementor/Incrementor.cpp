#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

int count = 0;

/* light up only the pixel indexed by the 'count' variable, use color[0] from the palette */
void animateCount(PixelAnimationData* data) {
    int step = count - 1;
    PixelColor color = data->paletteColor(0);
    for (int idx = 0; idx < data->pixelCount; idx++) { data->pixels[idx] = color.scale(idx == step ? 1 : 0); }
}

/* increment/decrement count by button (0 = top, 1 = right, 2 = bottom, 3 = left) */
void buttonHandler(int button, bool state) {
    if (button == 0) { count = PIXEL_COUNT; }
    if (button == 1) { count++; }
    if (button == 2) { count = 0; }
    if (button == 3) { count--; }
    count = count < 0 ? PIXEL_COUNT: count % (PIXEL_COUNT + 1);
}


/*
 * setup/loop
 */

BetterPhotonButton bb = BetterPhotonButton();

void setup() {
    bb.setup();
    bb.setReleasedHandler(&buttonHandler);
    bb.startPixelAnimation(&animateCount);
}

void loop() {
    bb.update(millis());
}