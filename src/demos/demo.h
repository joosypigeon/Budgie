#ifndef DEMO_H
#define DEMO_H

#include <stddef.h>
#include "raylib.h"
#include "../budgie/precision.h"

void initDemo(); // initalise the demo
void deinitDemo(); // clean up and end demo
const char* getTitle();/** Returns the window title for the demo. */
void keyboard(KeyboardKey key);
void mouseButtonPressed(MouseButton mouseButton);
void updateDemo(buReal duration);/** Update the particles. */
void displayDemo();
void displayInfo(size_t Y, size_t d);

#endif // DEMO_H