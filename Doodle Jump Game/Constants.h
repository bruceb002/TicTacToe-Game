#pragma once

 const int SCALE= 175; //percent
 const int GRAVITY = 1000 * SCALE / 100; // acceleration constant (UNITS: pixels/s^2)
 const int REBOUND_VELOCITY = -480 * SCALE / 100; // initial jump velocity (UNITS: pixels/s)
 const int VELOCITY = 1 * SCALE / 100; // initial jump velocity (UNITS: pixels/s)
 const float DURATION = (float)1.0/60; // KeyFrame(60 frames/s) duration (UNITS: s)

 const int PLAYBUTTON_WIDTH = 250* SCALE / 100; // (UNITS: pixels)
 const int PLAYBUTTON_HEIGHT = 250 * SCALE / 100; // (UNITS: pixels)

 const int GAMEOVERBUTTON_WIDTH = 196 * SCALE / 100; // (UNITS: pixels)
 const int GAMEOVERBUTTON_HEIGHT = 110 * SCALE / 100; // (UNITS: pixels)

 const int PLAYAGAINBUTTON_WIDTH = 344; // (UNITS: pixels)
 const int PLAYAGAINBUTTON_HEIGHT = 109; // (UNITS: pixels)

 const int PLATFORM_WIDTH = 50 * SCALE / 100; // (UNITS: pixels)
 const int PLATFORM_HEIGHT = 10 * SCALE / 100; // (UNITS: pixels)


 const int DOODLER_WIDTH = 45 * SCALE / 100; // (UNITS: pixels)
 const int DOODLER_HEIGHT = 45 * SCALE / 100; // (UNITS: pixels)

 const int FRAME_WIDTH = 350 * SCALE / 100; // (UNITS: pixels)
 const int FRAME_HEIGHT = 500 * SCALE / 100; // (UNITS: pixels)
 const int FRAME_MIDPOINT = 250 * SCALE / 100; // (UNITS: pixels)

 const int HORIZONTAL_MOVEMENT = 100 * SCALE / 100;// (UNITS: pixels)
 const int VERTICAL_MIN = 60 * SCALE / 100;// (UNITS: pixels)
 const int VERTICAL_MAX = 100 * SCALE / 100;// (UNITS: pixels)
 const int DOODLE_XMOVE = 5 * SCALE/ 100;// (UNITS: pixels)

 const int DOODLER_IMG_WIDTH = 225; // (UNITS: pixels)
 const int DOODLER_IMG_HEIGHT = 225; // (UNITS: pixels)
 const int BACKGROUND_IMG_WIDTH = 532; // (UNITS: pixels)
 const int BACKGROUND_IMG_HEIGHT = 850; // (UNITS: pixels)
 const int PLATFORM_IMG_WIDTH = 368; // (UNITS: pixels)
 const int PLATFORM_IMG_HEIGHT = 110; // (UNITS: pixels)

 const int PLAYBUTTON_IMG_WIDTH = 3000; // (UNITS: pixels)
 const int PLAYBUTTON_IMG_HEIGHT = 3000; // (UNITS: pixels)
  
 const int GAMEOVERBUTTON_IMG_WIDTH = 1370; // (UNITS: pixels)
 const int GAMEOVERBUTTON_IMG_HEIGHT = 768 ; // (UNITS: pixels)

 const int PLAYAGAINBUTTON_IMG_WIDTH = 344; // (UNITS: pixels)
 const int PLAYAGAINBUTTON_IMG_HEIGHT = 109; // (UNITS: pixels)