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

#### [AnimateAccel](examples/AnimateAccel/AnimateAccel.cpp)
See above.

#### [Animations](examples/Animations/Animations.cpp)
Use the buttons to cycle through a variety of sample animations.

#### [Incrementor](examples/Incrementor/Incrementor.cpp)
Use the buttons to increment/decrement/reset which LED is lit up.

#### [PlaySongs](examples/PlaySongs/PlaySongs.cpp)
Cycle through 11 'tunes' using the buttons.  One tune per LED.

#### [SimpleRainbow](examples/SimpleRainbow/SimpleRainbow.cpp)
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

#### [ToneTest](examples/ToneTest/ToneTest.cpp)
Example of playing notes while animating the LEDs

## Reference

### Buttons
(in progress)

### Pixels (LEDs)
(in progress)

#### Built in Animations

These animations are just functions implmented at the bottom of [BetterPhotonButton.cpp](src/BetterPhotonButton.cpp).
The most complex one in this set is animation_comet and it is only 9 lines of code, so these are not complex 
to implement.  Study these functions and the corresponding `PixelAnimationData` struct which has supporting functions
to help you compute things relative to the current step, palette, pixel, etc.

##### use only first color in color palette
* `animation_blink`
* `animation_alternating`
* `animation_fadeIn`
* `animation_fadeOut`
* `animation_glow`

##### use all colors in color palette
* `animation_strobe`
* `animation_sparkle`
* `animation_fader`
* `animation_cycle`
* `animation_random`
* `animation_increment`
* `animation_decrement`
* `animation_bounce`
* `animation_scanner`
* `animation_comet`
* `animation_bars`
* `animation_gradient`

You can write your own animation functions as seen above in `AnimateAccel` and set the pattern
of LEDs based on data (such as the accelerometer). 

Another example of a custom 'animation' function is in `PlaySongs` with the `animateCount()` 
function which lights a single LED based on the value of the variable indicating the current
selection.

#### Built in Color Palettes

All animations depend on a color palette to function.  These palettes are simply a reference to a 
`PixelPalette` struct which wraps a count and a pointer to an array of colors (`PixelColor`).  
There are 5 predefined palettes to help you get started.

* `paletteBW` -- 2: White/Black(off)
* `paletteRGB` -- 3: Red/Green/Blue
* `paletteRYGB` -- 4: Red/Yellow/Green/Blue
* `paletteRYGBStripe` -- 8: Red/Black/Yellow/Black/Green/Black/Blue/Black
* `paletteRainbow` 7: Red/Orange/Yellow/Green/Blue/Indego/Violet

Note that many animations compute the color between two colors in a palette. For example if you
use the `animation_gradient` with the `paletteBW` across 11 LEDs (pixels) you will end up with
with a gradient of the brightness of White to Black (off) across the 11 LEDs.  This is the difference
between using functions on `PixelAnimationData` that take and return `float` vs `int`.

### Musical Notes
(in progress)

### Accelerometer
(in progress)

## Etc.

* It is possible to use the `PhotonWS2812Pixel` and Animations classes directly to support any chain 
of WS8212 pixel LEDs.  Just use the `BetterPhotonButton` class as an example.  
  * See `BetterPhotonButton::updateAnimation` and the global `pixelRing` specifically


## License
Copyright 2017 The Brynwood Team, LLC.