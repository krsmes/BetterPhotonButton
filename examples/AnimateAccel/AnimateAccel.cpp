#include "Particle.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

BetterPhotonButton bb = BetterPhotonButton();
PhotonADXL362Accel *accel;

// keeps palette color 0 at the top
void gradientTop(PixelAnimationData *data) {
    float step = data->pixelCount - (accel->getAzimuth() / (360 / data->pixelCount));
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = data->paletteColor(((step + idx) * data->paletteCount()) / data->pixelCount);
    }
}


/*
 * setup/loop
 */

void setup() {
    bb.setup();
    accel = bb.startAccelerometer(1000/60);  // start accelerometer with a update rate of 60x per second
    bb.startPixelAnimation(&gradientTop, &paletteRainbow);
}

void loop() {
    bb.update(millis());
}
