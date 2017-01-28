# BetterPhotonButton

_Replacement for Particle's InternetButton library specific to the Photon_

### Why

After reading through the code to InternetButton from Particle I wanted to fix a few things.  
Instead I ended up creating a replacement.

BetterPhotonButton only works with the Photon (not the Core).  The LED driver class only works with WS2812 LEDs 
(the ones on the InternetButton).

## Features

* **There is no use of the delay() function in BetterPhotonButton.**  

* Button presses and releases have debounce handling built in and can trigger a callback function.

* The LED/Pixel classes support animations.  Write your own or use one of the 17 built in animations.

* The animations support custom color palettes.

* LEDs can be set with separate r, g, b values or with a combined value (like html colors).

* The Accelerometer class supports a callback method which is called when the InternetButton starts
or stops moving.  The class also provides angle, azimuth, pitch, and roll calculations.  It uses
a state machine when starting up to also avoid using delay().

All of the above is accomplished by an `update()` method on BetterPhotonButton.  
Just add `update(millis())` to the `loop()` method and BetterPhotonButton classes take care of the rest.

## Usage

The below is the example code from AnimateAccel, it simply rotates a gradient color palette so color 0
stays at the top.

```c
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

BetterPhotonButton bb = BetterPhotonButton();
PhotonADXL362Accel *accel;

void gradientTop(PixelAnimationData *data) {
    float step = data->pixelCount - (accel->getAzimuth() / (360 / data->pixelCount));
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = data->paletteColor(((step + idx) * data->paletteCount()) / data->pixelCount);
    }
}

void setup() {
    bb.setup();
    accel = bb.startAccelerometer(1000/60);  // start accelerometer with a update rate of 60x per second
    bb.startPixelAnimation(&gradientTop, &paletteRainbow);
}

void loop() {
    bb.update(millis());
}
```

The key to this code being `bb.setup()` and `bb.update(...)`. Every BetterPhotonButton needs to call 
these two for the library to function properly.

## Examples

#### AnimateAccel
See above.

#### Animations
Use the buttons to cycle through a variety of sample animations.

#### Incrementor
Use the buttons to increment/decrement/reset which LED is lit up.

#### PlaySongs
Cycle through 11 'tunes' using the buttons.  One tune per LED.

#### SimpleRainbow
The simplest BetterPhotonButton example.  Simple gradiant across all the LEDs.  

```c
#include "BetterPhotonButton.h"

BetterPhotonButton bb = BetterPhotonButton();

void setup() {
    bb.setup();
    bb.rainbow(250); // cycle rainbow every 250ms (indefinitely)
}

void loop() {
    bb.update(millis());
}
```

The `rainbow(...)` function exists as an carryover from Particle's InternetButton library.  All it 
does is call `startPixelAnimation(&animation_gradient, &paletteRainbow, cycle, duration);`

#### ToneTest
Example of playing notes while animating the LEDs

## Notes

* It is possible to use the `PhotonWS2812Pixel` and Animations classes directly to support any chain 
of WS8212 pixel LEDs.  Just use the `BetterPhotonButton` class as an example.  
  * See `BetterPhotonButton::updateAnimation` and the global `pixelRing` specifically


## License
Copyright 2017 The Brynwood Team, LLC.