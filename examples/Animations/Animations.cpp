#include "Particle.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);


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

// some custom palettes
PixelColor colorsRainbowStripe[] = {
        0xFF0000, 0x000000, 0xAB5500, 0x000000, 0xABAB00, 0x000000, 0x00FF00, 0x000000,
        0x00AB55, 0x000000, 0x0000FF, 0x000000, 0x5500AB, 0x000000, 0xAB0055, 0x000000
};
PixelColor colorsMix[] = {
        0x5500AB, 0x84007C, 0xB5004B, 0xE5001B, 0xE81700, 0xB84700, 0xAB7700, 0xABAB00,
        0xAB5500, 0xDD2200, 0xF2000E, 0xC2003E, 0x8F0071, 0x5F00A1, 0x2F00D0, 0x0007F9
};
PixelColor colorsYCM[] = {
        PixelColor::YELLOW, PixelColor::CYAN, PixelColor::MAGENTA
};
PixelColor colors80s[] = {
        PixelColor::MAROON, PixelColor::OLIVE, PixelColor::GREEN, PixelColor::NAVY,
        PixelColor::TEAL, PixelColor::PURPLE, PixelColor::GRAY, PixelColor::SILVER
};
PixelColor colorsOther[] = {
        PixelColor::BROWN, PixelColor::SIENNA, PixelColor::CORAL, PixelColor::ORANGE,
        PixelColor::GOLD, PixelColor::INDIGO, PixelColor::VIOLET, PixelColor::PINK
};

// all the palettes (predefined and custom)
PixelPalette palettes[] = {
        paletteBW,
        paletteRGB,
        paletteRYGB,
        paletteRYGBStripe,
        paletteRainbow,
        { 16, colorsRainbowStripe },
        { 16, colorsMix},
        { 3, colorsYCM },
        { 8, colors80s },
        { 8, colorsOther },
};


/*
 * setup/loop
 */

BetterPhotonButton bb = BetterPhotonButton();
int currentAnimation = 0;
int currentPalette = 0;

// forward declarations (these are not needed if you put setup/loop at the bottom of the file)
void buttonHandler(int button, bool state);
void startAnimation(int button, bool state);

void setup() {
    bb.setup();
    bb.setPressedHandler(&buttonHandler);  // set buttonHandler() as handler for all buttons when pressed
    bb.setReleasedHandler(&buttonHandler);  // set buttonHandler() as handler for all buttons when released
    bb.setReleasedHandler(0, &startAnimation);  // start animations when top button (0) released
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

            // cycle around if either palette or animation go below zero or above array size
            currentPalette = (int) ((currentPalette < 0) ? arraySize(palettes) - 1 : currentPalette % arraySize(palettes));
            currentAnimation = (int) ((currentAnimation < 0) ? arraySize(animations) - 1 : currentAnimation % arraySize(animations));

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

/* simple start the current animation with the current palette with a 2 second cycle time */
void startAnimation(int button, bool pressed) {
    bb.startPixelAnimation(animations[currentAnimation], &palettes[currentPalette], 2000);
}
