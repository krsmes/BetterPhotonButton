#pragma SPARK_NO_PREPROCESSOR
#include "application.h"
#include "BetterPhotonButton.h"

SYSTEM_THREAD(ENABLED);

String songs[] = {
        // twinkle
        "C,C,G,G,A,A,G,R,F,F,E,E,D,D,C",
        // mary had a little lamb
        ":o=4,B,A,G,A,B,B,B:2,A,A,A:2,B,D+,D+:2,B,A,G,A,B,B,B,B,A,A,B,A,G",
        // happy birthday
        ":o=4,G:8,G:8,A,G,C+,B:2,G:8,G:8,A,G,D+,C+:2,G:8,G:8,G+,E+,C+,B,A:3,F+:8,F+:8,E+,C+,D+,C+:2",

        // macarena
        ":o=5,:b=180,f,8f,8f,f,8f,8f,8f,8f,8f,8f,8f,8a,8c,8c,f,8f,8f,f,8f,8f,8f,8f,8f,8f,8d,8c,p,f,8f,8f,f,8f,8f,8f,8f,8f,8f,8f,8a,p,2c.6,a,8c6,8a,8f,p,2p",
        // entertainer
        ":o=5,:b=140,8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6",
        // the good the bad the ugly
        ":o=5,:b=56,32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#",

        // jeopardy
        ":o=6,:b=125,c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#",
        // deep blue something
        ":b=112, 8.#f+, 8#f+, 16#f+, 8.e+, 8#f+, 16#f+, 16g+, 16#f+, 16d+, 16a, 8.d+, 8d+, 16d+, 8.#c+, 8#c+, 16#c+, 8#c+, 16d+, 16d+, 8.#f+, 8#f+, 16#f+, 8.e+, 8#f+, 16#f+, 16g+, 16#f+, 16d+, 16a, 8.d+, 8d+, 16d+, 8.#c+, 8#c+, 16#c+, 16#c+, 16d+, 16d+, 8.#f+, 8#f+, 16#f+, 8.e+, 8#f+, 16#f+, 16g+, 16#f+, 16d+, 16a, 8.d+, 8d+, 16d+, 8.#c+, 8#c+, 16#c+, 8#c+, 16d+, 16d+, 8.#f+, 8#f+, 16#f+, 8.e+, 8#f+, 16#f+, 16g+, 16#f+, 16d+, 16a, 8.d+, 8d+, 16d+, 8.#c+, 8#c+, 16#c+, 16#c+, 16d+, 16d+",
        // starwars
        ":o=5,:b=45,32p,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#.6,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#6",

        // mission impossible
        ":o=6,:b=95,:d=16,32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d",
        // indiana jones
        ":o=5,:b=250,e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6",
};

// most of the above is from
//   https://www.reddit.com/r/arduino/comments/10l2pk/question_about_creating_a_melody_with_a_small/
//   with a few tweaks around the settings at the beginning of the string


int song = 0;

BetterPhotonButton bb = BetterPhotonButton();

void button0(int button, bool state) { song++; }

void button1(int button, bool state) { bb.playNotes(songs[song % arraySize(songs)]); }

void button2(int button, bool state) { song--; }

void button3(int button, bool state) { bb.stopPlayingNotes(); }

void animateCount(PixelAnimationData *data) {
    PixelColor color = data->paletteColor(song);
    for (int idx = 0; idx < data->pixelCount; idx++) { data->pixels[idx] = color.scale(idx == song ? 1 : 0); }
}

/*
 * setup/loop
 */

void setup() {
    Serial.begin(9600);
    bb.setup();
    bb.startPixelAnimation(animateCount);
    bb.setReleasedHandler(0, button0);
    bb.setReleasedHandler(1, button1);
    bb.setReleasedHandler(2, button2);
    bb.setReleasedHandler(3, button3);
}

void loop() {
    bb.update(millis());
}