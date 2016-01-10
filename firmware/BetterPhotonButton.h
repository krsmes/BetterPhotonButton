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

#define PIXEL_COUNT 11
#define PIXEL_PHOTON_PIN 3

#define ADXL_PHOTON_PIN A2
#define ADXL_TOLERANCE 10


struct PixelColor {
    byte r;
    byte g;
    byte b;

    inline PixelColor() __attribute__((always_inline)) { }

    inline PixelColor(byte red, byte green, byte blue)  __attribute__((always_inline))
            : r(red), g(green), b(blue) { }

    inline PixelColor(uint32_t rgb)  __attribute__((always_inline))
            : r((byte) ((rgb >> 16) & 0xFF)), g((byte) ((rgb >> 8) & 0xFF)), b((byte) ((rgb >> 0) & 0xFF)) { }

    bool operator == (const PixelColor &other) const {
        return (this->r == other.r and this->g == other.g and this->b == other.b);
    }

    bool operator != (const PixelColor &other) const {
        return (this->r != other.r or this->g != other.g or this->b != other.b);
    }

    uint32_t rgb() { return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b; }

    PixelColor interpolate(PixelColor pixel, float value) {
        byte newR = (byte) (value * (pixel.r - r) + r);
        byte newG = (byte) (value * (pixel.g - g) + g);
        byte newB = (byte) (value * (pixel.b - b) + b);
        return PixelColor(newR, newG, newB);
    }

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

    PixelColor computeColorAt(float index) {
        int first = (int) index % (count);
        int second = (int) (index + 1) % (count);
        float interpolationValue = index - (int) index;
        return colors[first].interpolate(colors[second], interpolationValue);
    }

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

    int step(int steps) { return (int) (((updated - start) % cycleMillis) * steps / cycleMillis); }

    float step(float steps) { return ((updated - start) % cycleMillis) * steps / cycleMillis; }

    int pixelStep() { return step(pixelCount); }  // cycle / pixels = 1 step

    int paletteStep() { return step(palette->count); }  // cycle / palette-colors = 1 step

    int paletteCount() { return palette->count; }

    float palettePartialStep() { return step((float)palette->count); }

    PixelColor paletteStepColor() { return palette->computeColorAt(paletteStep()); }

    PixelColor palettePartialStepColor() { return palette->computeColorAt(palettePartialStep()); }

    PixelColor paletteColor(float index) { return palette->computeColorAt(index); }

    PixelColor paletteColor(int index) { return palette->colors[index % palette->count]; }

    PixelColor randomColor() { return palette->randomColor(); }

    PixelColor pixelColor(int index) { return pixels[index % pixelCount]; }

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


class BetterPhotonButton {
public:
    BetterPhotonButton();

    void setup(void);

    void update(system_tick_t millis);

    /* buttons */

    bool isButtonPressed(byte button);

    bool allButtonsPressed();

    void setPressedHandler(ButtonHandler *handler);

    void setPressedHandler(byte button, ButtonHandler *handler);

    void setReleasedHandler(ButtonHandler *handler);

    void setReleasedHandler(byte button, ButtonHandler *handler);

    /* pixels */

    void setPixel(int pixel, byte r, byte g, byte b);

    void setPixel(int pixel, PixelColor color);

    void setPixels(byte r, byte g, byte b);

    void setPixels(PixelColor color);

    void setPixels(PixelColor* colors, int count);

    PixelColor getPixel(int pixel);

    /* animation */

    PixelAnimationData* startPixelAnimation(PixelAnimation *animation, PixelPalette *palette = &paletteRainbow,
                                            long cycle = 1000, long duration = -1, int refresh = 1000/60);

    bool isPixelAnimationActive();

    void rainbow(long cycle = 1000, long duration = -1);

    /* accelerometer */

    PhotonADXL362Accel* startAccelerometer(unsigned int refreshRate = 1000/10);

private:
    void updateButtonsState(system_tick_t millis);

    void updateButtonState(byte button, system_tick_t millis);

    void updateAnimation(system_tick_t millis);

    PixelAnimation *animationFunction;
    PixelAnimationData animationData = PixelAnimationData();
    int animationRefresh;
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

    void setMotionHandler(MotionHandler *handler);

    unsigned long inMotion();

    unsigned long notInMotion();

    double getPitch();

    double getRoll();

    int getAngle();

    int getAzimuth();

    enum State: int { WAITING, STARTUP, RESET, POWER, CALIBRATING, RUNNING } state;
    int16_t x, y, z, t;
    double fx, fy, fz;

private:
    void stateCalibrating();

    void stateRunning();

    byte spiRead8(byte regAddress);

    void spiWrite8(byte regAddress, byte regValue);

    void spiReadXYZT();

    int stateWait[6] = { 100, 100, 10, 10, 10, 0 };

    byte pin;
    int16_t xMin, xMax, xZero;
    int16_t yMin, yMax, yZero;
    int16_t zMin, zMax, zZero;

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
