#include "RoutePath.h"


using namespace std;

RoutePath::RoutePath(int num0, int num1, int num2, int num3, int num4, int num5) {
	sourceX = num0;
	sourceY = num1;
	sourceP = num2;
	sinkX = num3;
	sinkY = num4;
	sinkP = num5;

	priority = 0;
	state = 0;
}

RoutePath::~RoutePath() {
	// free used tracks(tracks already freed from grid level, double free caused problem)
	/*for(int i = 0; i < usedTracks.size(); i++){
		delete usedTracks[i];
		usedTracks[i] = NULL;
	}*/
}

void RoutePath::resetUsedTracks() {
	usedTracks.clear();
}

void RoutePath::setPriority(int p) {
	priority = p;
}

void RoutePath::setState(int s) {
	state = s;
}

void RoutePath::useTrack(track* thisTrack) {
	usedTracks.push_back(thisTrack);
}

int RoutePath::getXYP(int index) {
	switch (index) {
	case 0: return sourceX; break;
	case 1: return sourceY; break;
	case 2: return sourceP; break;
	case 3: return sinkX; break;
	case 4: return sinkY; break;
	case 5: return sinkP; break;
	default: std::cout << "Invalid input index\n"; return -1;
	}
}

vector<track*>* RoutePath::getTracks() {
	return &usedTracks;
}
