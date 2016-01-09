# BetterPhotonButton
Replacement for Particle's InternetButton library specific to the Photon

After reading through the code to InternetButton from Particle I wanted to fix a few things.  
Instead I ended up creating a replacement.

BetterPhotonButton only works with the Photon (not the Core).  The LED driver class only works with WS2812 LEDs 
(the ones on the InternetButton).

There is no use of the delay() function in BetterPhotonButton.  

Button presses and releases have debounce handling built in and can trigger a callback function.

The LED/Pixel classes support animations.  Write your own or use one of the 17 built in animations.
The animations support custom color palettes.

LEDs can be set with separate r, g, b values or with a combined value (like html colors).

The Accelerometer class supports a callback method which is called when the InternetButton starts
or stops moving.  The class also provides angle, azimuth, pitch, and roll calculations.  It uses
a state machine when starting up to also avoid using delay().

All of the above is accomplished by an update() method on BetterPhotonButton.  
Just add update(millis()) to the loop() method and BetterPhotonButton classes take care of the rest.

