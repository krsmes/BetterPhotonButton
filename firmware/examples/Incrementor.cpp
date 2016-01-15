#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

int count = 0;

void animateCount(PixelAnimationData* data) {
    int step = count - 1;
    PixelColor color = data->paletteColor(0);
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = color.scale(idx == step ? 1 : 0);
    }
}

void buttonHandler(int button, bool state) {
    if (button == 0) { count++; }
    if (button == 1) { count = PIXEL_COUNT; }
    if (button == 2) { count--; }
    if (button == 3) { count = 0; }
    count %= PIXEL_COUNT;
}

BetterPhotonButton bpb = BetterPhotonButton();


void setup() {
    bpb.setup();
    bpb.setReleasedHandler(&buttonHandler);
    bpb.startPixelAnimation(&animateCount);
}

void loop() {
    bpb.update(millis());
}