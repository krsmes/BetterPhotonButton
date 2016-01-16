/*
Copyright 2016 Kevin R. Smith

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef BETTERPHOTONBUTTON_H
#define BETTERPHOTONBUTTON_H

#include <application.h>

#define BUTTON_COUNT 4
#define BUTTON_1_PHOTON_PIN 4
#define BUTTON_2_PHOTON_PIN 5
#define BUTTON_3_PHOTON_PIN 6
#define BUTTON_4_PHOTON_PIN 7
#define BUTTON_DEBOUNCE_DELAY 50

#define BUZZER_PHOTON_PIN D0

#define PIXEL_COUNT 11
#define PIXEL_PHOTON_PIN 3

#define ADXL_PHOTON_PIN A2
#define ADXL_TOLERANCE 10


struct PixelColor {
    byte r;
    byte g;
    byte b;

    inline PixelColor() __attribute__((always_inline)) { }

    /* create a color with the given red, green, and blue values */
    inline PixelColor(byte red, byte green, byte blue)  __attribute__((always_inline))
            : r(red), g(green), b(blue) { }

    /* create a color with the given 0xRRGGBB value */
    inline PixelColor(uint32_t rgb)  __attribute__((always_inline))
            : r((byte) ((rgb >> 16) & 0xFF)), g((byte) ((rgb >> 8) & 0xFF)), b((byte) ((rgb >> 0) & 0xFF)) { }

    bool operator == (const PixelColor &other) const {
        return (this->r == other.r and this->g == other.g and this->b == other.b);
    }

    bool operator != (const PixelColor &other) const {
        return (this->r != other.r or this->g != other.g or this->b != other.b);
    }

    /* return the current color as 0xRRGGBB */
    uint32_t rgb() { return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b; }

    /* compute a new color between the current one and given one using the given fractional value */
    PixelColor interpolate(PixelColor color, float value) {
        byte newR = (byte) (value * (color.r - r) + r);
        byte newG = (byte) (value * (color.g - g) + g);
        byte newB = (byte) (value * (color.b - b) + b);
        return PixelColor(newR, newG, newB);
    }

    /* multiply the current color by the given scale value */
    PixelColor scale(float value) {
        byte newR = (byte) min(r * value, 0xFF);
        byte newG = (byte) min(g * value, 0xFF);
        byte newB = (byte) min(b * value, 0xFF);
        return PixelColor(newR, newG, newB);
    }

    enum Colors: uint32_t {
        BLACK   = 0,
        WHITE   = 0xFFFFFF,
        RED     = 0xFF0000,
        GREEN   = 0x00FF00,
        BLUE    = 0x0000FF,
        YELLOW  = RED | GREEN,
        CYAN    = GREEN | BLUE,
        MAGENTA = RED | BLUE,
    };
};


struct PixelPalette {
    byte count;
    PixelColor *colors;

    /* return a color at the given index, if the index is fractional compute the proper color between the two indices */
    PixelColor computeColorAt(float index) {
        int first = (int) index % (count);
        int second = (int) (index + 1) % (count);
        float interpolationValue = index - (int) index;
        return colors[first].interpolate(colors[second], interpolationValue);
    }

    /* return one of the palette colors randomly */
    PixelColor randomColor() {
        return colors[random(count)];
    }

    bool operator == (const PixelPalette &other) const {
        if (this->count != other.count) return false;
        for (int i=0; i < count; i++) {
            if (this->colors[i] != other.colors[i]) return false;
        }
        return true;
    }
};

extern PixelPalette paletteBW;
extern PixelPalette paletteRGB;
extern PixelPalette paletteRYGB;
extern PixelPalette paletteRYGBStripe;
extern PixelPalette paletteRainbow;

struct PixelAnimationData {
    int pixelCount;
    PixelColor *pixels;
    PixelPalette *palette;
    long cycleMillis;
    unsigned long start;
    unsigned long stop;
    unsigned long updated;
    int temp;

    /* return the current step, given the number of steps, based on time and cycle time */
    int step(int steps) { return (int) (((updated - start) % cycleMillis) * steps / cycleMillis); }

    /* return the current fractional step, given the number of steps, based on time and cycle time */
    float step(float steps) { return ((updated - start) % cycleMillis) * steps / cycleMillis; }

    /* return the size of the color palatte */
    inline int paletteCount() { return palette->count; }

    /* return the current step as pixel index */
    inline int pixelStep() { return step(pixelCount); }  // cycle / pixels = 1 step

    /* return the current step as palatte index */
    inline int paletteStep() { return step(palette->count); }  // cycle / palette-colors = 1 step

    /* return the current step as fractional palatte index */
    inline float palettePartialStep() { return step((float)palette->count); }

    inline PixelColor paletteStepColor() { return palette->computeColorAt(paletteStep()); }

    inline PixelColor palettePartialStepColor() { return palette->computeColorAt(palettePartialStep()); }

    inline PixelColor paletteColor(float index) { return palette->computeColorAt(index); }

    inline PixelColor paletteColor(int index) { return palette->colors[index % palette->count]; }

    inline PixelColor randomColor() { return palette->randomColor(); }

    inline PixelColor pixelColor(int index) { return pixels[index % pixelCount]; }

    void setPixels(PixelColor color) { for (int i = 0; i < pixelCount; ++i) { pixels[i] = color; } }

    float mapFloat(float value, float minLeft, float maxLeft, float minRight, float maxRight) {
        return (value - minLeft) * (maxRight - minRight) / (maxLeft - minLeft) + minRight;
    }
};

typedef void (PixelAnimation)(PixelAnimationData* data);

/* palette color[0] only */
extern PixelAnimation animation_blink;
extern PixelAnimation animation_alternating;
extern PixelAnimation animation_fadeIn;
extern PixelAnimation animation_fadeOut;
extern PixelAnimation animation_glow;

/* all palette colors */
extern PixelAnimation animation_strobe;
extern PixelAnimation animation_sparkle;
extern PixelAnimation animation_fader;
extern PixelAnimation animation_cycle;
extern PixelAnimation animation_random;
extern PixelAnimation animation_increment;
extern PixelAnimation animation_decrement;
extern PixelAnimation animation_bounce;
extern PixelAnimation animation_scanner;
extern PixelAnimation animation_comet;
extern PixelAnimation animation_bars;
extern PixelAnimation animation_gradient;



/**********************************************************************************************************************/

class PhotonADXL362Accel;

typedef void (ButtonHandler)(int button, bool pressed);
extern int noteToFrequency(const char *note_cstr);


class BetterPhotonButton {
public:
    BetterPhotonButton();

    // initialize all the things, must be called in application's setup()
    void setup(void);

    // update all the things (pixels, buttons, accelermeter, buzzer), call this from the application's loop()
    void update(system_tick_t millis);

    /* buttons */

    // true if given button (0 based) is pressed
    bool isButtonPressed(byte button);

    // true if all buttons are currently pressed
    bool allButtonsPressed();

    // set the callback function for when any button is pressed
    void setPressedHandler(ButtonHandler *handler);

    // set the callback function for when the given button (0 based) is pressed
    void setPressedHandler(byte button, ButtonHandler *handler);

    // set the callback function for when any button is released
    void setReleasedHandler(ButtonHandler *handler);

    // set the callback function for when the given button (0 based) is pressed
    void setReleasedHandler(byte button, ButtonHandler *handler);

    /* pixels */

    // set given pixel (0 based) to given rgb colors, refreshes pixels on next update()
    void setPixel(int pixel, byte r, byte g, byte b);

    // set given pixel (0 based) to given color (0xRRGGBB or PixelColor::X), refreshes pixels on next update()
    void setPixel(int pixel, PixelColor color);

    // set all pixels to given color, refreshes pixels on next update()
    void setPixels(byte r, byte g, byte b);

    // set all pixels to given color, refreshes pixels on next update()
    void setPixels(PixelColor color);

    // set all pixels to given colors array, refreshes pixels on next update()
    void setPixels(PixelColor* colors, int count);

    // like set but forces immediate refresh, does not disable animation
    void updatePixel(int pixel, PixelColor color);

    // like set but forces immediate refresh, does not disable animation
    void updatePixels(PixelColor color);

    // like set but forces immediate refresh, does not disable animation
    void updatePixels(PixelColor* colors, int count);

    // retrieve the given pixel's color
    PixelColor getPixel(int pixel);

    /* animation */

    // start a pixel animation using the given animation function
    PixelAnimationData* startPixelAnimation(PixelAnimation *animation, PixelPalette *palette = &paletteRainbow,
                                            long cycle = 1000, long duration = -1, int refresh = 1000/60);

    // true if an animation is currently running
    bool isPixelAnimationActive();

    // display the &animation_gradient and &palette_rainbow with the given cycle times and duration, -1=indefinite
    void rainbow(long cycle = 1000, long duration = -1);

    /* accelerometer */

    /* initiate the accelerometer, set it update at the given refresh rate, and return pointer to it */
    PhotonADXL362Accel* startAccelerometer(unsigned int refreshRate = 1000/10);

    /* buzzer */

    /* play the given note for the given duration (returns immediately)
     * examples: "G" or "G5" = 784Hz, "G4" or "G-" = 392Hz, "G6" or "G+" = 1568Hz , "G#" = 831Hz, "Gb" = 740Hz */
    int playNote(const char* current, int duration = 1000/4);

    /* start the given sequence of notes (returns immediately)
     * example: "C-:8,E-,G-,C,G:4" plays 1/8th notes C4, E4, G4, C5 and 1/4 note G5 */
    void playNotes(String notes, int defaultDuration = 1000/4);

private:
    void updateButtonsState(system_tick_t millis);

    void updateButtonState(byte button, system_tick_t millis);

    void updateAnimation(system_tick_t millis);

    void updatePlayNotes(system_tick_t millis);

    PixelAnimation *animationFunction;
    PixelAnimationData animationData = PixelAnimationData();
    int animationRefresh;
    char* notesToPlay;
    int noteDuration;
    system_tick_t notesNextUpdate;
};

/**********************************************************************************************************************/





// Photon Only, WS2812B Only
class PhotonWS2812Pixel {
public:
    PhotonWS2812Pixel(PixelColor *pixels, int pixelCount, byte pin);

    ~PhotonWS2812Pixel();

    void setup(void);

    void update(bool refresh = false) __attribute__((optimize("Ofast")));

    void triggerRefresh();

    void setPixelColor(int pixel, PixelColor pixelColor);

private:
    byte pin;
    PixelColor *pixels;
    int pixelCount;
    unsigned long endTime;
    bool refresh;
};



// Accelerometer classes
typedef void (MotionHandler)(bool motion, unsigned long after);

class PhotonADXL362Accel {
public:
    PhotonADXL362Accel(byte pin);

    void setup(unsigned int refreshRate = 1000/10);

    void update(system_tick_t millis);

    /* set the callback function for when motion changes between in-motion and not-in-motion */
    void setMotionHandler(MotionHandler *handler);

    /* return true if not within the initial calibration values */
    unsigned long inMotion();

    /* return true if within the initial calibration values */
    unsigned long notInMotion();

    /* compute the pitch: ( ( arctan( x / squareroot( y^2 + z^2 ) ) * 180 ) / PI )  */
    double getPitch();

    /* compute the roll: ( ( arctan( -y / z ) * 180 ) / PI )  */
    double getRoll();

    /* compute the current angle (0..359), 0 = right, 90 = top, 180 = left, 270 = bottom */
    int getAngle();

    /* conver the angle to azimuth (0..360), 0 = top, 90 = right, 180 = bottom, 270 = left */
    int getAzimuth();

    /* direct access to current state */
    enum State: int { WAITING, STARTUP, RESET, POWER, CALIBRATING, RUNNING } state;

    /* direct access to x, y, z, and t (internal raw temperature value) */
    int16_t x, y, z, t;

    /* direct access to fx, fy, fz (weighted 50/50 between the last two updates, less 'jumpy') */
    double fx, fy, fz;

private:
    void stateCalibrating();

    void stateRunning();

    byte spiRead8(byte regAddress);

    void spiWrite8(byte regAddress, byte regValue);

    void spiReadXYZT();

    int stateWait[6] = { 100, 100, 10, 10, 10, 0 };

    byte pin;
    int16_t xMin, xMax;
    int16_t yMin, yMax;
    int16_t zMin, zMax;

    int calibrationCount;
    unsigned long motionMillis;
    unsigned long noMotionMillis;
    unsigned long nextUpdate;
    MotionHandler *motionHandler;
};


#define SPI_READ_INSTRUCTION 0x0B
#define SPI_WRITE_INSTRUCTION 0x0A

// ADXL Registers
#define XL362_XDATA_L       0x0E
#define XL362_SOFT_RESET    0x1F
#define XL362_POWER_CTL     0x2D

#define XL362_POWER_FLAG_MEASURE_RUNING  0b10



#endif //BETTERPHOTONBUTTON_H
