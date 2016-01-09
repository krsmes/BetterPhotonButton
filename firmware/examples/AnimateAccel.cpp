#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

BetterPhotonButton bb = BetterPhotonButton();
PhotonADXL362Accel* accel;

// keeps palete color 0 at the top
void animation_gradientAccel(PixelAnimationData data) {
    float step = data.count - (accel->getAzimuth() / (360 / data.count));
    for (int idx = 0; idx < data.count; idx++) {
        data.pixels[idx] = data.color(((step + idx) * (*data.palette).count) / data.count);
    }
}

void setup() {
    bb.setup();
    accel = bb.startAccelerometer();
    bb.startPixelAnimation(&animation_gradientAccel, &paletteRainbow);
}

void loop() {
    bb.update(millis());
}
