#include "draw.h"

#define windowSize 1000.0
Routing* MR1;   // set as global variable so other functions can access easily
int num_next_path_clicks = 0;
bool isAllPath = false;

void openGraphics(Routing* MR) {
	int i;
	/* initialize display with WHITE background */
	MR1 = MR;
	cout << "Starting graphics...\n";
	init_graphics("Maze Router (by Zhiyuan Gu)", WHITE);               // text on the top of the window

	/* still picture drawing allows user to zoom, etc. */
	init_world(0., 0., windowSize, windowSize);                              // Set-up drawing coordinates.
	update_message("Press PROCEED to start routing");                              // text down below

	// responds to panning and zooming button pushes from the user
	// drawGrid is a graphics routine called by event_loop
	event_loop(act_on_button_press, NULL, NULL, drawGrid);

	cout << "You pressed PROCEED!\n" << "Press NEXT PATH so the paths will be shown!\n";

	clearscreen();
	init_world(0., 0., windowSize, windowSize);
	update_message("Press NEXT PATH to route the next path");
	create_button("Window", "Next Path", NextPath);     // create new button and a function called everytime
	create_button("Window", "PreviousPath", PreviousPath);         // create new button and a function called everytime
	create_button("Window", "All Paths", AllPath);      // create new button and a function called everytime
	create_button("Window", "Rewind", Rewind);              // create new button and a function called everytime

	drawGrid();
	event_loop(act_on_button_press, NULL, NULL, drawGrid);

	close_graphics();                                             // close graphics tool
	cout << "Graphics closed down.\n";
}

void drawGrid(void) {                                          // redrawing routine for still pictures.

	float gridSize = MR1->getGridSize();
	float Wv = MR1->getWv();
	float Wh = MR1->getWh();
	float bound = 60;
	float bSize = (windowSize - 2 * bound) / (gridSize * 2 + 1);
	float sSize = (windowSize - 2 * bound) / (gridSize * 2 + 1) * 0.8;
	float BSize = (windowSize - 2 * bound) / (gridSize * 2 + 1) * 1.2;
	float vspace = sSize / (Wv + 1);                                // space between vertical tracks
	float hspace = sSize / (Wh + 1);                                // space between horizontal tracks
	float pspace = sSize / 5;                                     // space between pins

	set_draw_mode(DRAW_NORMAL);                                   // Should set this if your program does any XOR drawing in callbacks.
	clearscreen();                                                 // Should precede drawing for all drawscreens

	setfontsize(10);
	setlinestyle(SOLID);
	setlinewidth(1);
	setcolor(BLACK);

	// draw switch blocks and others

	for (int i = 0; i < 2 * gridSize + 1; i++) {//row
		for (int j = 0; j < 2 * gridSize + 1; j++) {//column
			if ((i + j) % 2 == 0) {    // for blocks
				setlinestyle(SOLID);
				setlinewidth(2);
				setcolor(BLACK);
				drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize),
					(bound + i * bSize + 0.5*bSize - 0.5*sSize),
					(bound + j * bSize + 0.5*bSize + 0.5*sSize),
					(bound + i * bSize + 0.5*bSize - 0.5*sSize));
				drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize),
					(bound + i * bSize + 0.5*bSize - 0.5*sSize),
					(bound + j * bSize + 0.5*bSize - 0.5*sSize),
					(bound + i * bSize + 0.5*bSize + 0.5*sSize));
				drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize),
					(bound + i * bSize + 0.5*bSize + 0.5*sSize),
					(bound + j * bSize + 0.5*bSize + 0.5*sSize),
					(bound + i * bSize + 0.5*bSize + 0.5*sSize));
				drawline((bound + j * bSize + 0.5*bSize + 0.5*sSize),
					(bound + i * bSize + 0.5*bSize + 0.5*sSize),
					(bound + j * bSize + 0.5*bSize + 0.5*sSize),
					(bound + i * bSize + 0.5*bSize - 0.5*sSize));
			}
			else {  // for track channels
				if (i % 2 == 1) { // for vertical tracks
					for (int k = 0; k < Wv; k++) {
						setlinestyle(DASHED);
						setlinewidth(2);
						setcolor(BLACK);
						drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize + vspace * (k + 1)),
							(bound + i * bSize + 0.5*bSize + 0.5*BSize),
							(bound + j * bSize + 0.5*bSize - 0.5*sSize + vspace * (k + 1)),
							(bound + i * bSize + 0.5*bSize - 0.5*BSize));
					}
				}
				else {  // for horizontal tracks
					for (int k = 0; k < Wh; k++) {
						setlinestyle(DASHED);
						setlinewidth(2);
						setcolor(BLACK);
						drawline((bound + j * bSize + 0.5*bSize - 0.5*BSize),
							(bound + i * bSize + 0.5*bSize - 0.5*sSize + hspace * (k + 1)),
							(bound + j * bSize + 0.5*bSize + 0.5*BSize),
							(bound + i * bSize + 0.5*bSize - 0.5*sSize + hspace * (k + 1)));
					}

				}
			}
			// for logic blocks
			if ((i % 2 == 1) && (j % 2 == 1)) {
				// fill color
				setcolor(YELLOW);
				fillrect((bound + j * bSize + 0.5*bSize + 0.5*sSize),
					(bound + i * bSize + 0.5*bSize + 0.5*sSize),
					(bound + j * bSize + 0.5*bSize - 0.5*sSize),
					(bound + i * bSize + 0.5*bSize - 0.5*sSize));
				// label coordinate
				int x = (j - 1) / 2;
				int y = (gridSize - 1) - (i - 1) / 2; // upside down

				int printSize = 4;
				if (x > 9)
					printSize++;
				if (y > 9)
					printSize++;
				char* s = new char[printSize];
				if (x <= 9 && y <= 9) {
					s[0] = x + 48;
					s[1] = ',';
					s[2] = y + 48;
					s[3] = '\0';
				}
				else if (x <= 9 && y > 9) {
					s[0] = x + 48;
					s[1] = ',';
					s[2] = y / 10 + 48;
					s[3] = y % 10 + 48;
					s[4] = '\0';
				}
				else if (x > 9 && y <= 9) {
					s[0] = x / 10 + 48;
					s[1] = x % 10 + 48;
					s[2] = ',';
					s[3] = y + 48;
					s[4] = '\0';
				}
				else {
					s[0] = x / 10 + 48;
					s[1] = x % 10 + 48;
					s[2] = ',';
					s[3] = y / 10 + 48;
					s[4] = y % 10 + 48;
					s[5] = '\0';
				}
				setfontsize(18);
				setcolor(BLACK);
				drawtext((bound + j * bSize + 0.5*bSize),
					bound + i * bSize + 0.5*bSize,
					s, 150.);
				delete[] s;
				// draw pin wire
				for (int p = 0; p < 4; p++) {
					setlinestyle(DASHED);
					setlinewidth(2);
					setcolor(DARKGREEN);
					drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (p + 1)),
						(bound + i * bSize + 0.5*bSize - 0.5*sSize),
						(bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (p + 1)),
						(bound + i * bSize - 0.9*bSize + hspace));
					char s2[2];
					s2[0] = p + 1 + 48;
					s2[1] = '\0';
					setfontsize(12);
					setcolor(BLACK);
					drawtext((bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (p + 1)),
						bound + i * bSize + 0.2*bSize,
						s2, 150.);

				}
			}

		}
	}

	drawNextPath();
	if (isAllPath) drawAllPath();
}

void act_on_button_press(float x, float y) {

	/* Called whenever event_loop gets a button press in the graphics *
	 * area.  Allows the user to do whatever he/she wants with button *
	 * clicks.                                                        */

	printf("User clicked a button at coordinates (%f, %f)\n", x, y);


}

void NextPath(void(*drawscreen_ptr) (void)) {
	num_next_path_clicks++;
	drawGrid();
	printf("number of paths printed: %d\n", num_next_path_clicks);
}

void AllPath(void(*drawscreen_ptr) (void)) {
	isAllPath = true;
	drawGrid();
	printf("All paths print complete!\n");
	printf("Press PROCEED or EXIT to close window\nPress PREVIOUSPATH to show previous path\nPress REWIND to go to the beginning\n");
	update_message("All paths print complete!");
}

void Rewind(void(*drawscreen_ptr) (void)) {
	num_next_path_clicks = 0;
	isAllPath = false;
	drawGrid();
	update_message("Press NEXT PATH to route the next path");

	printf("Rewinded to the beginning!\n");
	printf("Press NEXT PATH so the paths will be shown!\n");
}

void PreviousPath(void(*drawscreen_ptr) (void)) {
	if (isAllPath)
		isAllPath = false;
	else if (num_next_path_clicks > 0) {
		num_next_path_clicks--;
		update_message("Press NEXT PATH to route the next path");
	}
	drawGrid();
	printf("Returned to previous path!\n");
}


void drawNextPath() {
	setlinestyle(SOLID);
	setlinewidth(1);
	setcolor(BLACK);

	float gridSize = MR1->getGridSize();
	float Wv = MR1->getWv();
	float Wh = MR1->getWh();
	float bound = 60;
	float bSize = (windowSize - 2 * bound) / (gridSize * 2 + 1);
	float sSize = (windowSize - 2 * bound) / (gridSize * 2 + 1) * 0.8;
	float BSize = (windowSize - 2 * bound) / (gridSize * 2 + 1) * 1.2;
	float vspace = sSize / (Wv + 1);                                // space between vertical tracks
	float hspace = sSize / (Wh + 1);                                // space between horizontal tracks
	float pspace = sSize / 5;                                     // space between pins

	if (num_next_path_clicks >= MR1->getPaths()->size() + 1) {
		isAllPath = true;
		printf("All paths print complete!\n");
		printf("Press PROCEED or EXIT to close window\nPress PREVIOUSPATH to show previous path\nPress REWIND to go to the beginning\n");
		update_message("All paths print complete!");
	}
	else if (num_next_path_clicks > 0) {
		if (num_next_path_clicks == MR1->getPaths()->size()) {
			printf("All paths print complete!\n");
			printf("Press PROCEED or EXIT to close window\nPress PREVIOUSPATH to show previous path\nPress REWIND to go to the beginning\n");
			update_message("All paths print complete!");
		}
		// draw source pin wire
		int j = 2 * MR1->getPaths()->at(num_next_path_clicks - 1)->getXYP(0) + 1;
		int i = 2 * (gridSize - 1 - MR1->getPaths()->at(num_next_path_clicks - 1)->getXYP(1)) + 1;
		int pinS = MR1->getPaths()->at(num_next_path_clicks - 1)->getXYP(2);  // get source pin index
		setlinestyle(SOLID);
		setlinewidth(3);
		setcolor(BLUE);
		drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinS)),
			(bound + i * bSize + 0.5*bSize - 0.5*sSize),
			(bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinS)),
			(bound + i * bSize - 0.9*bSize + hspace));
		// draw paths used
		vector<track*>* paths = MR1->getPaths()->at(num_next_path_clicks - 1)->getTracks();
		for (int ti = 0; ti < paths->size(); ti++) {
			track* track = paths->at(ti);
			int gx = track->getGridX();
			int gy = 2 * (gridSize - track->getGridY());
			int ix = track->getIndex();
			// for track channels
			if (gx % 2 == 0) { // for vertical tracks

				setlinestyle(SOLID);
				setlinewidth(3);
				setcolor(BLUE);
				drawline((bound + gx * bSize + 0.5*bSize - 0.5*sSize + vspace * (ix + 1)),
					(bound + (gy - 1)*bSize + 0.5*bSize + 0.5*BSize),
					(bound + gx * bSize + 0.5*bSize - 0.5*sSize + vspace * (ix + 1)),
					(bound + (gy - 1)*bSize + 0.5*bSize - 0.5*BSize));

			}
			else {  // for horizontal tracks

				setlinestyle(SOLID);
				setlinewidth(3);
				setcolor(BLUE);
				drawline((bound + gx * bSize + 0.5*bSize - 0.5*BSize),
					(bound + gy * bSize + 0.5*bSize - 0.5*sSize + hspace * (ix + 1)),
					(bound + gx * bSize + 0.5*bSize + 0.5*BSize),
					(bound + gy * bSize + 0.5*bSize - 0.5*sSize + hspace * (ix + 1)));


			}
		}
		// draw load pin wire
		j = 2 * MR1->getPaths()->at(num_next_path_clicks - 1)->getXYP(3) + 1;
		i = 2 * (gridSize - 1 - MR1->getPaths()->at(num_next_path_clicks - 1)->getXYP(4)) + 1;
		int pinL = MR1->getPaths()->at(num_next_path_clicks - 1)->getXYP(5);  // get load pin index
		setlinestyle(SOLID);
		setlinewidth(3);
		setcolor(BLUE);
		drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinL)),
			(bound + i * bSize + 0.5*bSize - 0.5*sSize),
			(bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinL)),
			(bound + i * bSize - 0.9*bSize + hspace));
	}
}

void drawAllPath() {
	setlinestyle(SOLID);
	setlinewidth(1);
	setcolor(BLACK);

	float gridSize = MR1->getGridSize();
	float Wv = MR1->getWv();
	float Wh = MR1->getWh();
	float bound = 60;
	float bSize = (windowSize - 2 * bound) / (gridSize * 2 + 1);
	float sSize = (windowSize - 2 * bound) / (gridSize * 2 + 1) * 0.8;
	float BSize = (windowSize - 2 * bound) / (gridSize * 2 + 1) * 1.2;
	float vspace = sSize / (Wv + 1);                                // space between vertical tracks
	float hspace = sSize / (Wh + 1);                                // space between horizontal tracks
	float pspace = sSize / 5;                                     // space between pins

	num_next_path_clicks = 0;
	for (int n = 0; n < MR1->getPaths()->size(); n++) {
		// draw source pin wire
		int j = 2 * MR1->getPaths()->at(n)->getXYP(0) + 1;
		int i = 2 * (gridSize - 1 - MR1->getPaths()->at(n)->getXYP(1)) + 1;
		int pinS = MR1->getPaths()->at(n)->getXYP(2);  // get source pin index
		setlinestyle(SOLID);
		setlinewidth(3);
		setcolor(BLUE);
		drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinS)),
			(bound + i * bSize + 0.5*bSize - 0.5*sSize),
			(bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinS)),
			(bound + i * bSize - 0.9*bSize + hspace));
		// draw paths used
		vector<track*>* paths = MR1->getPaths()->at(n)->getTracks();
		for (int ti = 0; ti < paths->size(); ti++) {
			track* track = paths->at(ti);
			int gx = track->getGridX();
			int gy = 2 * (gridSize - track->getGridY());
			int ix = track->getIndex();
			// for track channels
			if (gx % 2 == 0) { // for vertical tracks

				setlinestyle(SOLID);
				setlinewidth(3);
				setcolor(BLUE);
				drawline((bound + gx * bSize + 0.5*bSize - 0.5*sSize + vspace * (ix + 1)),
					(bound + (gy - 1)*bSize + 0.5*bSize + 0.5*BSize),
					(bound + gx * bSize + 0.5*bSize - 0.5*sSize + vspace * (ix + 1)),
					(bound + (gy - 1)*bSize + 0.5*bSize - 0.5*BSize));

			}
			else {  // for horizontal tracks
				setlinestyle(SOLID);
				setlinewidth(3);
				setcolor(BLUE);
				drawline((bound + gx * bSize + 0.5*bSize - 0.5*BSize),
					(bound + gy * bSize + 0.5*bSize - 0.5*sSize + hspace * (ix + 1)),
					(bound + gx * bSize + 0.5*bSize + 0.5*BSize),
					(bound + gy * bSize + 0.5*bSize - 0.5*sSize + hspace * (ix + 1)));
			}
		}
		// draw load pin wire
		j = 2 * MR1->getPaths()->at(n)->getXYP(3) + 1;
		i = 2 * (gridSize - 1 - MR1->getPaths()->at(n)->getXYP(4)) + 1;
		int pinL = MR1->getPaths()->at(n)->getXYP(5);  // get load pin index
		setlinestyle(SOLID);
		setlinewidth(3);
		setcolor(BLUE);
		drawline((bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinL)),
			(bound + i * bSize + 0.5*bSize - 0.5*sSize),
			(bound + j * bSize + 0.5*bSize - 0.5*sSize + pspace * (pinL)),
			(bound + i * bSize - 0.9*bSize + hspace));

		num_next_path_clicks++;
	}

}


