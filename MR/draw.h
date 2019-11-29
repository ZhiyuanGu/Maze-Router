#include "graphics.h"
#include <iostream>
#include <stdio.h>
#include "Routing.h"
#include <math.h>

using namespace std;


void openGraphics(Routing* MR);
void drawGrid(void);
void drawNextPath ();
void drawAllPath ();
void NextPath (void (*drawscreen_ptr) (void));
void AllPath (void (*drawscreen_ptr) (void));
void act_on_button_press (float x, float y);
void Rewind (void (*drawscreen_ptr) (void));
void PreviousPath(void (*drawscreen_ptr) (void));
