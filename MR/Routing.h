#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <queue>
#include <cstdlib>
#include "RoutePath.h"
#include "graphics.h"


using namespace std;

class Routing {
private:
	int GridSize;
	int Wv;	// vertical channel width
	int Wh;	// horizontal channel width
	vector<RoutePath*> allPath;	// all the connections between blocks that need to be routed
	vector< vector< vector<track*> > > RoutingGrid;	// the routing wires/tracks form a grid to route on

public:
	Routing();
	Routing(int wv, int wh);
	~Routing();

	void freeRoutePath();
	void freeGrid();

	int getGridSize() { return GridSize; }
	int getWv() { return Wv; }
	int getWh() { return Wh; }
	vector<RoutePath*>* getPaths();
	vector< vector< vector<track*> > >* getGrid();

	void inputFile(string filePath);
	void resetPaths();
	void setManhattanPriority();
	void sortPaths();
	void createGrid();
	void resetGrid();
	void labelGrid();
	int MazeRouting(int printOption);
	bool label0T(int p, queue<track*> &q, int print);
	void expand(track* tempTrack, vector<track*> &adjacentTracks);
	int getTotalSegments();

};

