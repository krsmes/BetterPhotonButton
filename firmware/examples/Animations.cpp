#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);


BetterPhotonButton bb = BetterPhotonButton();

// all the predefined animations
PixelAnimation *animations[] = {
        &animation_blink,
        &animation_fadeIn,
        &animation_fadeOut,
        &animation_glow,
        &animation_alternating,
        &animation_strobe,
        &animation_sparkle,
        &animation_cycle,
        &animation_fader,
        &animation_random,
        &animation_increment,
        &animation_decrement,
        &animation_bounce,
        &animation_scanner,
        &animation_comet,
        &animation_bars,
        &animation_gradient,
};

int currentAnimation = 0;


// some custom palettes
PixelColor colorsRainbowStripe[] = {
        0xFF0000, 0x000000, 0xAB5500, 0x000000, 0xABAB00, 0x000000, 0x00FF00, 0x000000,
        0x00AB55, 0x000000, 0x0000FF, 0x000000, 0x5500AB, 0x000000, 0xAB0055, 0x000000
};
PixelColor colorsParty[] = {
        0x5500AB, 0x84007C, 0xB5004B, 0xE5001B, 0xE81700, 0xB84700, 0xAB7700, 0xABAB00,
        0xAB5500, 0xDD2200, 0xF2000E, 0xC2003E, 0x8F0071, 0x5F00A1, 0x2F00D0, 0x0007F9
};
PixelColor colorsWRY[] = {
        0x000000, 0xFF0000, 0xFFFF00
};

// all the palettes (predefined and custom)
PixelPalette palettes[] = {
        paletteBW,
        paletteRGB,
        paletteRYGB,
        paletteRYGBStripe,
        paletteRainbow,
        { 16, colorsRainbowStripe },
        { 16, colorsParty },
        { 3, colorsWRY },
};

int currentPalette = 0;


// forward declarations
void buttonHandler(int button, bool state);
void startAnimation(int button, bool state);

void setup() {
    bb.setup();
    bb.setPressedHandler(&buttonHandler);
    bb.setReleasedHandler(&buttonHandler);
    bb.setReleasedHandler(0, &startAnimation); // start animations when top button released
    bb.rainbow(500, 1000);  // loop rainbow twice in one second at startup
}

void loop() {
    bb.update(millis());  // this call is where all the work is done
}

/*
 * if animations are active
 *      if buttons 2, 3, & 4 are pressed, stop the animations
 *      button 1 pressed: next color palette
 *      button 2 pressed: next animation
 *      button 3 pressed: previous color palette
 *      button 4 pressed: previous animation
 *      (going past the bounds of the arrays loops around)
 * if no animation is active
 *      all buttons: all pixels blue
 *      any button: all pixels green when pressed, all pixels red when released
 */
void buttonHandler(int button, bool pressed) {
    if (bb.isPixelAnimationActive()) {
        if (pressed) {
            if (button == 1) { currentAnimation++; }
            else if (button == 3) { currentAnimation--; }
            else if (button == 0) { currentPalette++; }
            else if (button == 2) { currentPalette--; }

            if (currentPalette < 0) { currentPalette = arraySize(palettes) - 1; }
            if (currentPalette == arraySize(palettes)) { currentPalette = 0; }

            if (currentAnimation < 0) { currentAnimation = arraySize(animations) - 1; }
            if (currentAnimation == arraySize(animations)) { currentAnimation = 0; }

            bb.startPixelAnimation(animations[currentAnimation], &palettes[currentPalette], 2000);
        }
        if (bb.isButtonPressed(1) && bb.isButtonPressed(2) & bb.isButtonPressed(3)) {
            bb.setPixels(0);
        }
    }
    else {
        if (bb.allButtonsPressed()) {
            bb.setPixels(0x0000FF);
        }
        else {
            bb.setPixels(pressed ? 0x00FF00 : 0xFF0000);
        }
    }
}

void startAnimation(int button, bool pressed) {
    bb.startPixelAnimation(animations[currentAnimation], &palettes[currentPalette], 2000);
}
