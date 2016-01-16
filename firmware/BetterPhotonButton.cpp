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

#include "BetterPhotonButton.h"
#include <math.h>
#include "pinmap_impl.h"

/*************************
 * BetterPhotonButton
 */

byte buttonPins[BUTTON_COUNT] = {BUTTON_1_PHOTON_PIN, BUTTON_2_PHOTON_PIN, BUTTON_3_PHOTON_PIN, BUTTON_4_PHOTON_PIN};
bool buttonState[BUTTON_COUNT] = {0};
bool buttonPrevState[BUTTON_COUNT] = {0};
int buttonPrevUpdate[BUTTON_COUNT] = {0};

ButtonHandler *buttonPressed[BUTTON_COUNT] = {0};
ButtonHandler *buttonReleased[BUTTON_COUNT] = {0};

PixelColor pixels[PIXEL_COUNT] = {0};
PhotonWS2812Pixel pixelRing = PhotonWS2812Pixel(pixels, PIXEL_COUNT, PIXEL_PHOTON_PIN);
PhotonADXL362Accel accelerometer = PhotonADXL362Accel(ADXL_PHOTON_PIN);

/*
 * constructors/destructors
 */

BetterPhotonButton::BetterPhotonButton() {
    animationData.pixels = pixels;
    animationData.pixelCount = PIXEL_COUNT;
}


/*
 * public api
 */

void BetterPhotonButton::setup(void) {
    pixelRing.setup();
    for (int idx = 0; idx < BUTTON_COUNT; idx++) {
        pinMode(buttonPins[idx], INPUT_PULLUP);
    }
}

void BetterPhotonButton::update(system_tick_t millis) {
    updatePlayNotes(millis);
    updateAnimation(millis);
    pixelRing.update();
    accelerometer.update(millis);
    updateButtonsState(millis);
}

bool BetterPhotonButton::isButtonPressed(byte button) {
    return button < BUTTON_COUNT ? buttonState[button] : false;
}

bool BetterPhotonButton::allButtonsPressed() {
    for (int idx = 0; idx < BUTTON_COUNT; idx++) { if (!buttonState[idx]) return false; }
    return true;
}

void BetterPhotonButton::setPressedHandler(ButtonHandler *handler) {
    for (byte idx = 0; idx < BUTTON_COUNT; idx++) {
        setPressedHandler(idx, handler);
    }
}

void BetterPhotonButton::setPressedHandler(byte button, ButtonHandler *handler) {
    if (button < BUTTON_COUNT) { buttonPressed[button] = handler; }
}

void BetterPhotonButton::setReleasedHandler(ButtonHandler *handler){
    for (byte idx = 0; idx < BUTTON_COUNT; idx++) {
        setReleasedHandler(idx, handler);
    }
}

void BetterPhotonButton::setReleasedHandler(byte button, ButtonHandler *handler) {
    if (button < BUTTON_COUNT) { buttonReleased[button] = handler; }
}

void BetterPhotonButton::setPixel(int pixel, byte r, byte g, byte b) {
    setPixel(pixel, PixelColor(r, g, b));
}

void BetterPhotonButton::setPixel(int pixel, PixelColor color) {
    animationFunction = NULL;
    pixelRing.setPixelColor(pixel, color);
};

void BetterPhotonButton::setPixels(byte r, byte g, byte b) {
    setPixels(PixelColor(r, g, b));
}

void BetterPhotonButton::setPixels(PixelColor color) {
    animationFunction = NULL;
    for (int idx = 0; idx < PIXEL_COUNT; idx++) { pixelRing.setPixelColor(idx, color); }
};

void BetterPhotonButton::setPixels(PixelColor* colors, int count) {
    animationFunction = NULL;
    for (int idx = 0; idx < PIXEL_COUNT; idx++) { pixelRing.setPixelColor(idx, idx < count ? colors[idx] : 0); }
}

void BetterPhotonButton::updatePixel(int pixel, PixelColor color) {
    pixelRing.setPixelColor(pixel, color);
    pixelRing.update(true);
};

void BetterPhotonButton::updatePixels(PixelColor color) {
    for (int idx = 0; idx < PIXEL_COUNT; idx++) { pixelRing.setPixelColor(idx, color); }
    pixelRing.update(true);
};

void BetterPhotonButton::updatePixels(PixelColor* colors, int count) {
    for (int idx = 0; idx < PIXEL_COUNT; idx++) { pixelRing.setPixelColor(idx, idx < count ? colors[idx] : 0); }
    pixelRing.update(true);
}

PixelColor BetterPhotonButton::getPixel(int pixel) {
    return pixels[pixel];
}

PixelAnimationData* BetterPhotonButton::startPixelAnimation(PixelAnimation *animation, PixelPalette *palette,
                                                            long cycle, long duration, int refresh) {
    animationFunction = animation;
    animationData.palette = palette;
    animationData.cycleMillis = cycle;
    animationData.start = millis();
    animationData.stop = animationData.start + duration;
    animationData.temp = 0;
    animationRefresh = refresh;
    return &animationData;
}

bool BetterPhotonButton::isPixelAnimationActive() {
    return (bool) (*animationFunction);
}

void BetterPhotonButton::rainbow(long cycle, long duration) {
    startPixelAnimation(&animation_gradient, &paletteRainbow, cycle, duration);
}

PhotonADXL362Accel* BetterPhotonButton::startAccelerometer(unsigned int refreshRate) {
    accelerometer.setup(refreshRate);
    return &accelerometer;
}

int BetterPhotonButton::playNote(char* current, int duration) {
    int freq = noteToFrequency(current);

    // compute the note time up to two digits between the colon and the comma or end-of-string
    // e.g. "C:8," = 1/8 note; "G#+:16" = 16th note
    char* comma = strchr(current, ',');
    char* colon = strchr(current, ':');
    int time = 0;
    if (comma || colon) {
        if (colon && (!comma || colon < comma)) {
            time = *++colon - '0';
            if (*++colon != 0 && colon != comma) { time = time * 10 + (*colon - '0'); }
        }
    }
    // turn the time into a duration
    if (time) { duration = (time == 1) ? 1000 : 1000/time; }

    tone(BUZZER_PHOTON_PIN, (unsigned int) freq, (unsigned long) duration);
    return duration;
}

void BetterPhotonButton::playNotes(String notes, int defaultDuration) {
    notesToPlay = (char *) notes.c_str();
    noteDuration = defaultDuration;
    notesNextUpdate = 0;
}


/*
 * private helpers
 */

void BetterPhotonButton::updateButtonsState(system_tick_t millis) {
    for (byte idx = 0; idx < BUTTON_COUNT; idx++) { updateButtonState(idx, millis); }
}

void BetterPhotonButton::updateButtonState(byte button, system_tick_t millis) {
    bool currentState = (bool) !digitalRead(buttonPins[button]);
    if (currentState != buttonPrevState[button]) {
        buttonPrevState[button] = currentState;
        buttonPrevUpdate[button] = millis;
    }
    if ((millis - buttonPrevUpdate[button] > BUTTON_DEBOUNCE_DELAY) &&
        (currentState == buttonPrevState[button]) &&
        (currentState != buttonState[button])) {
        buttonState[button] = currentState;
        // call the pressed or released handler function if one has been set for this button
        ButtonHandler *handler = currentState ? buttonPressed[button] : buttonReleased[button];
        if (handler) { (*handler)((int)button, currentState); }
    }
}

void BetterPhotonButton::updateAnimation(system_tick_t millis) {
    if ((*animationFunction) && (millis > animationData.updated + animationRefresh)) {
        if (animationData.stop > animationData.start && millis > animationData.stop) {
            setPixels(0);
        }
        else {
            animationData.updated = millis;
            animationFunction(&animationData);
            pixelRing.triggerRefresh();
        }
    }
}

void BetterPhotonButton::updatePlayNotes(system_tick_t millis) {
    if (notesToPlay && (millis >= notesNextUpdate)) {
        noteDuration = playNote(notesToPlay, noteDuration);
        notesNextUpdate = millis + noteDuration;
        notesToPlay = strchr(notesToPlay, ',');  // move ahead to next comma
        if (notesToPlay) { notesToPlay++; }
    }
}



/*************************
 * PhotonWS2812Pixel
 */

STM32_Pin_Info* BB_PIN_MAP = HAL_Pin_Map(); // Pointer required for highest access speed
#define bbPinLO(_pin) (BB_PIN_MAP[_pin].gpio_peripheral->BSRRH = BB_PIN_MAP[_pin].gpio_pin)
#define bbPinHI(_pin) (BB_PIN_MAP[_pin].gpio_peripheral->BSRRL = BB_PIN_MAP[_pin].gpio_pin)
#define PIXEL_WAIT_TIME 50L

/*
 * constructors/destructors
 */

PhotonWS2812Pixel::PhotonWS2812Pixel(PixelColor *pixels, int pixelCount, byte pin) {
    this->pixelCount = pixelCount;
    this->pixels = pixels;
    this->pin = pin;
    this->refresh = true;
}

PhotonWS2812Pixel::~PhotonWS2812Pixel() {
    pinMode(pin, INPUT);
}

/*
 * public api
 */

void PhotonWS2812Pixel::setup() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "CannotResolve"
void PhotonWS2812Pixel::update(bool refresh) {
    if (!this->refresh && !refresh) return;
    // Data latch = 50 microsecond pause in the output stream.  Rather than
    // put a delay at the end of the function, the ending time is noted and
    // the function will simply hold off (if needed) on issuing the
    // subsequent round of data until the latch time has elapsed.  This
    // allows the mainline code to start generating the next frame of data
    // rather than stalling for the latch.
    while((micros() - endTime) < PIXEL_WAIT_TIME);
    // endTime is a private member (rather than global var) so that multiple
    // instances on different pins can be quickly issued in succession (each
    // instance doesn't delay the next).

    __disable_irq(); // Need 100% focus on instruction timing

    volatile uint32_t
            c,    // 24-bit pixel color
            mask; // 8-bit mask
    volatile int i = pixelCount; // Output loop counter
    volatile uint8_t
            j,              // 8-bit inner loop counter
            g,              // Current green byte value
            r,              // Current red byte value
            b;              // Current blue byte value
    volatile PixelColor *pixptr = pixels;

    while(i) { // While pixels left...
        i--;      // decrement bytes remaining
        r = (*pixptr).r;   // Next red byte value
        g = (*pixptr).g;   // Next green byte value
        b = (*pixptr++).b;   // Next blue byte value
        c = ((uint32_t)g << 16) | ((uint32_t)r <<  8) | b; // Pack the next 3 bytes to keep timing tight

        mask = 0x800000; // reset the mask
        j = 0; // reset the 24-bit counter
        do {
            bbPinHI(pin);
            if (c & mask) { // if masked bit is high
                // WS2812 spec             700ns HIGH
                // Adafruit on Arduino    (meas. 812ns)
                // This lib on Photon     (meas. 792ns)
                asm volatile(
                    "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
                // WS2812 spec             600ns LOW
                // Adafruit on Arduino    (meas. 436ns)
                // This lib on Photon     (meas. 434ns)
                bbPinLO(pin);
                asm volatile(
                    "mov r0, r0" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
            }
            else { // else masked bit is low
                // WS2812 spec             350ns HIGH
                // Adafruit on Arduino    (meas. 312ns)
                // This lib on Photon     (meas. 308ns)
                asm volatile(
                    "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
                // WS2812 spec             800ns LOW
                // Adafruit on Arduino    (meas. 938ns)
                // This lib on Photon     (meas. 934ns)
                bbPinLO(pin);
                asm volatile(
                    "mov r0, r0" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t" "nop" "\n\t"
                    "nop" "\n\t" "nop" "\n\t"
                ::: "r0", "cc", "memory");
            }
            mask >>= 1;
        } while ( ++j < 24 ); // ... pixel done
    } // end while(i) ... no more pixels

    __enable_irq();

    endTime = micros(); // Save EOD time for latch on next call
    this->refresh = false;
}
#pragma clang diagnostic pop

void PhotonWS2812Pixel::setPixelColor(int pixel, PixelColor pixelColor) {
    if (pixel < pixelCount) {
        pixels[pixel] = pixelColor;
        refresh = true;
    }
}

void PhotonWS2812Pixel::triggerRefresh() {
    this->refresh = true;
}



/*************************
 * PhotonADXL362Accel
 */

/*
 * constructors/destructors
 */

PhotonADXL362Accel::PhotonADXL362Accel(byte pin) : pin(pin) {
    state = WAITING;
}

void PhotonADXL362Accel::setup(unsigned int refreshRate) {
    stateWait[RUNNING] = refreshRate;
    state = STARTUP;
}

void PhotonADXL362Accel::update(system_tick_t millis) {
    if (millis < nextUpdate) return;
    nextUpdate = millis + stateWait[state];

    switch (state) {
        case WAITING:
            break;
        case STARTUP:
            pinMode(pin, OUTPUT);
            SPI.begin();
            SPI.setDataMode(SPI_MODE0);
            state = RESET;
            break;
        case RESET:
            spiWrite8(XL362_SOFT_RESET, 0x52);
            state = POWER;
            break;
        case POWER:
            spiWrite8(XL362_POWER_CTL,
                      (byte) ((spiRead8(XL362_POWER_CTL) & 0b11111100) | XL362_POWER_FLAG_MEASURE_RUNING) );
            state = CALIBRATING;
            calibrationCount = 10;
            break;
        case CALIBRATING:
            stateCalibrating();
            break;
        case RUNNING:
            stateRunning();
            break;
    }
}

void PhotonADXL362Accel::setMotionHandler(MotionHandler *handler) {
    motionHandler = handler;
}

unsigned long PhotonADXL362Accel::inMotion() { return motionMillis; }

unsigned long PhotonADXL362Accel::notInMotion() { return noMotionMillis; }

double PhotonADXL362Accel::getPitch() {
    return (atan2(fx, sqrt(pow(fy, 2) + pow(fz, 2))) * 180.0) / M_PI;
}

double PhotonADXL362Accel::getRoll() {
    return (atan2(-fy, fz) * 180.0) / M_PI;
}

int PhotonADXL362Accel::getAngle() {
    return (int) ((atan2(fy, fx) * 180.0) / M_PI) + 180;
}

int PhotonADXL362Accel::getAzimuth(){
    return (90 - getAngle() + 360) % 360;
}


/*
 * private helpers
 */

void PhotonADXL362Accel::stateCalibrating() {
    spiReadXYZT();
    if (x < xMin) xMin = x; else if (x > xMax) xMax = x;
    if (y < yMin) yMin = y; else if (y > yMax) yMax = y;
    if (z < zMin) zMin = z; else if (z > zMax) zMax = z;
    if (calibrationCount-- == 0) {
        xMin -= ADXL_TOLERANCE; xMax += ADXL_TOLERANCE; xZero = (xMin + xMax) / 2;
        yMin -= ADXL_TOLERANCE; yMax += ADXL_TOLERANCE; yZero = (yMin + yMax) / 2;
        zMin -= ADXL_TOLERANCE; zMax += ADXL_TOLERANCE; zZero = (zMin + zMax) / 2;
        state = RUNNING;
    }
}

void PhotonADXL362Accel::stateRunning(){
    spiReadXYZT();
    fx = (x * 0.5) + (fx * 0.5);
    fy = (y * 0.5) + (fy * 0.5);
    fz = (z * 0.5) + (fz * 0.5);
    if (x < xMin || x > xMax || y < yMin || y > yMax || z < zMin || z > zMax) {
        // motion
        if (noMotionMillis) {
            if (motionHandler) { (*motionHandler)(true, noMotionMillis); }
            motionMillis = 0;
            noMotionMillis = 0;
        }
        motionMillis += stateWait[state];
    } else {
        // stationary
        if (motionMillis) {
            if (motionHandler) { (*motionHandler)(false, motionMillis); }
            motionMillis = 0;
            noMotionMillis = 0;
        }
        noMotionMillis += stateWait[state];
    }
}

byte PhotonADXL362Accel::spiRead8(byte regAddress) {
    byte regValue = 0;
    digitalWrite(pin, LOW);
    SPI.transfer(SPI_READ_INSTRUCTION);
    SPI.transfer(regAddress);
    regValue = SPI.transfer(0x00);
    digitalWrite(pin, HIGH);
    return regValue;
}

void PhotonADXL362Accel::spiWrite8(byte regAddress, byte regValue) {
    digitalWrite(pin, LOW);
    SPI.transfer(SPI_WRITE_INSTRUCTION);
    SPI.transfer(regAddress);
    SPI.transfer(regValue);
    digitalWrite(pin, HIGH);
}

void PhotonADXL362Accel::spiReadXYZT() {
    digitalWrite(pin, LOW);
    SPI.transfer(SPI_READ_INSTRUCTION);
    SPI.transfer(XL362_XDATA_L);
    x = SPI.transfer(0x00) + (SPI.transfer(0x00) << 8);
    y = SPI.transfer(0x00) + (SPI.transfer(0x00) << 8);
    z = SPI.transfer(0x00) + (SPI.transfer(0x00) << 8);
    t = SPI.transfer(0x00) + (SPI.transfer(0x00) << 8);
    digitalWrite(pin, HIGH);
}


/*************************
 * notes
 */

int bpb_noteToFrequency(const int note, const int octave = 5) {
    // see http://www.phy.mtu.edu/~suits/notefreqs.html
    return (int) lround(2093 * pow(1.059463094359, (12 * (octave - 7) + note))); // C7=2093.0
}

int bpb_noteIndex(const char note) {
    switch (note) {
        case 'C': return 0;
        case 'D': return 2;
        case 'E': return 4;
        case 'F': return 5;
        case 'G': return 7;
        case 'A': return 9;
        case 'B': return 11;
        default: return -1000; // rest
    }
}

int noteToFrequency(const char *note_cstr) {
    int note = -1000;
    int octave = 5;
    int idx = 0;
    // examples: "C" & "C5" are equivalent, "C4" & "C-" are equivalent, "F#" & "Gb" are equivalent
    while (note_cstr[idx] != 0 && note_cstr[idx] != ':' && note_cstr[idx] != ',') {
        char next = note_cstr[idx++];
        if (next >= 'A' && next <= 'G') { note = bpb_noteIndex(next); }
        else if (next >= '0' && next <= '8') { octave = next - '0'; }
        else if (next == '+') { octave++; }
        else if (next == '-') { octave--; }
        else if (next == '#') { note++; }
        else if (next == 'b') { note--; }
    }
    return bpb_noteToFrequency(note, octave);
}



/*************************
 * animations
 */


PixelColor colorsBW[] = { PixelColor::WHITE, PixelColor::BLACK };  // white is first so that blink/fade animations work
PixelColor colorsRGB[] = { PixelColor::RED, PixelColor::GREEN, PixelColor::BLUE };
PixelColor colorsRYGB[] = { PixelColor::RED, PixelColor::YELLOW, PixelColor::GREEN, PixelColor::BLUE };
PixelColor colorsRYGBStripe[] = { PixelColor::RED, 0, PixelColor::YELLOW, 0, PixelColor::GREEN, 0,  PixelColor::BLUE, 0 };
PixelColor colorsRainbow[] = { 0xFF0000, 0xAB5500, 0xABAB00, 0x00FF00, 0x00AB55, 0x0000FF, 0x5500AB, 0xAB0055 };

PixelPalette paletteBW = { 2, colorsBW };
PixelPalette paletteRGB = { 3, colorsRGB };
PixelPalette paletteRYGB = { 4, colorsRYGB };
PixelPalette paletteRYGBStripe = { 8, colorsRYGBStripe };
PixelPalette paletteRainbow = { 8, colorsRainbow };

void animation_blink(PixelAnimationData* data) {
    data->setPixels(data->paletteColor(0).scale((data->step(2) + 1) % 2));
}

void animation_alternating(PixelAnimationData* data) {
    int step = data->step(2);
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = data->paletteColor(0).scale((step + idx) % 2);
    }
}

void animation_fadeIn(PixelAnimationData* data) {
    data->setPixels(data->paletteColor(0).scale(data->step(1.0f)));
}

void animation_fadeOut(PixelAnimationData* data) {
    data->setPixels(data->paletteColor(0).scale(1.0f - data->step(1.0f)));
}

void animation_glow(PixelAnimationData* data) {
    float scale = (-cosf(data->step((float)M_PI*2)) + 1.0f) / 2.0f;
    data->setPixels(data->paletteColor(0).scale(scale));
}

void animation_strobe(PixelAnimationData* data) {
    int step = data->step(10);  // 1/10th of the cycle
    if (step != data->temp) {
        data->temp = step;
        data->setPixels(data->randomColor().scale(step==0));
    }
}

void animation_sparkle(PixelAnimationData* data) {
    int step = (int) (data->cycleMillis / 10);
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = (random(step) == 0) ? data->randomColor() : data->pixelColor(idx).scale(0.75);
    }
}

void animation_fader(PixelAnimationData* data) {
    data->setPixels(data->palettePartialStepColor());
}

void animation_cycle(PixelAnimationData* data) {
    data->setPixels(data->paletteStepColor());
}

void animation_random(PixelAnimationData* data) {
    int step = data->paletteStep();
    data->setPixels((step != data->temp) ? data->randomColor() : data->pixelColor(0));
    data->temp = step;
}

void animation_increment(PixelAnimationData* data) {
    int pixStep = data->pixelStep();  
    PixelColor color = data->palettePartialStepColor();
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = color.scale(idx == pixStep ? 1 : 0);
    }
}

void animation_decrement(PixelAnimationData* data) {
    int pixStep = data->pixelStep();
    PixelColor color = data->palettePartialStepColor();
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = color.scale(idx == (data->pixelCount - 1 - pixStep) ? 1 : 0);
    }
}

void animation_bounce(PixelAnimationData* data) {
    int pixStep = data->step(2 * data->pixelCount - 2);
    PixelColor color = data->palettePartialStepColor();
    for (int idx = 0; idx < data->pixelCount; idx++) {
        int scale = (idx == (-abs(pixStep - data->pixelCount + 1) + data->pixelCount - 1)) ? 1 : 0;
        data->pixels[idx] = color.scale(scale);
    }
}

void animation_scanner(PixelAnimationData* data) {
    float tail = data->pixelCount / 4;
    float step = data->step(2 * data->pixelCount + ((tail * 4) - 1));
    PixelColor color = data->palettePartialStepColor();
    for (int idx = 0; idx < data->pixelCount; idx++) {
        float scale = constrain((-abs((int) (abs((int) (step - data->pixelCount - 2 * tail)) - idx - tail)) + tail), 0, 1);
        data->pixels[idx] = color.scale(scale);
    }
}

void animation_comet(PixelAnimationData* data) {
    float tail = data->pixelCount / 2;
    float pixStep = data->step(2 * data->pixelCount - tail);
    PixelColor color;
    for (int idx = 0; idx < data->pixelCount; idx++) {
        float palStep = data->mapFloat(max(pixStep - idx, 0), 0, (data->pixelCount / 1.75f), 0, data->paletteCount() - 1);
        color = data->paletteColor(palStep);
        float scale = constrain((((idx - pixStep) / tail + 1.25f)) * (((idx - pixStep) < 0) ? 1 : 0), 0, 1);
        data->pixels[idx] = color.scale(scale);
    }
}

void animation_bars(PixelAnimationData* data) {
    int step = data->pixelStep();
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = data->paletteColor((step + idx) * data->paletteCount() / data->pixelCount);
    }
}

void animation_gradient(PixelAnimationData* data) {
    float step = data->pixelStep();
    for (int idx = 0; idx < data->pixelCount; idx++) {
        data->pixels[idx] = data->paletteColor((step + idx) * data->paletteCount() / data->pixelCount);
    }
}

