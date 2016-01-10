#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

BetterPhotonButton bb = BetterPhotonButton();
PhotonADXL362Accel* accel;

// keeps palete color 0 at the top
void animation_gradientAccel(PixelAnimationData* data) {
    float step = data->pixelCount - (accel->getAzimuth() / (360 / data->pixelCount));
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = data->paletteColor(((step + idx) * data->paletteCount()) / data->pixelCount);
    }
}

void setup() {
    bb.setup();
    accel = bb.startAccelerometer(1000/60);
    bb.startPixelAnimation(&animation_gradientAccel, &paletteRainbow);
}

void loop() {
    bb.update(millis());
}
