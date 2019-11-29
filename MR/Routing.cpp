#include "Routing.h"

using namespace std;

Routing::Routing() {
	GridSize = -1;
	Wv = -1;	// vertical channel width
	Wh = -1;	// horizontal channel width

}

Routing::Routing(int wv, int wh) {
	GridSize = -1;
	Wv = wv;	// vertical channel width
	Wh = wh;	// horizontal channel width

}

Routing::~Routing() {

	// free RoutePath
	freeRoutePath();
	// free RoutingGrid
	freeGrid();

}

void Routing::freeRoutePath() {
	for (int i = 0; i < allPath.size(); i++) {
		delete allPath[i];
		allPath[i] = NULL;
	}
}

void Routing::freeGrid() {
	for (int i = 0; i < RoutingGrid.size(); i++) {
		for (int j = 0; j < RoutingGrid[i].size(); j++) {
			for (int k = 0; k < RoutingGrid[i][j].size(); k++) {
				delete RoutingGrid[i][j][k];
				RoutingGrid[i][j][k] = NULL;
			}
		}
	}
}


void Routing::inputFile(string filePath) {
	// input routing parameters
	ifstream infile;
	infile.open(filePath.c_str(), std::ifstream::in);

	int num[6];
	infile >> num[0];
	GridSize = num[0];
	infile >> num[0];
	if (Wv == -1)
		Wv = num[0];
	infile >> num[0];
	if (Wh == -1)
		Wh = num[0];

	while ((infile >> num[0]) && (infile >> num[1]) && (infile >> num[2]) && (infile >> num[3]) && (infile >> num[4]) && (infile >> num[5])) {
		if (num[0] == -1) break;
		else {
			RoutePath* path = new RoutePath(num[0], num[1], num[2], num[3], num[4], num[5]);
			allPath.push_back(path);
		}
	}
	infile.close();
}

void Routing::resetPaths() {
	for (int i = 0; i < allPath.size(); i++) {
		allPath[i]->resetUsedTracks();
		allPath[i]->setState(0);
	}
}

void Routing::setManhattanPriority() {
	for (int i = 0; i < allPath.size(); i++) {
		allPath[i]->setPriority((int)(abs(allPath[i]->getXYP(0) - allPath[i]->getXYP(3)) + abs(allPath[i]->getXYP(1) - allPath[i]->getXYP(4))));
	}
}

void Routing::sortPaths() {
	// bubble sort
	for (int i = 0; i < allPath.size(); i++) {
		bool doneFlag = true;
		for (int j = allPath.size() - 1; j > i; j--) {
			if (allPath[j]->getPriority() < allPath[j - 1]->getPriority()) {
				RoutePath* temp = allPath[j];
				allPath[j] = allPath[j - 1];
				allPath[j - 1] = temp;
				doneFlag = false;
			}
		}
		if (doneFlag) // early termination
			break;
	}

}

void Routing::createGrid() {
	// creating the grid for routing, three dimension: x, y, i(index)
	for (int x = 0; x < (2 * GridSize + 1); x++) {
		bool isHorizontal = (x % 2 == 1) ? true : false;
		vector< vector< track* > > column;
		RoutingGrid.push_back(column);
		for (int y = 0; y < (GridSize + isHorizontal); y++) {
			vector< track* > channel;
			RoutingGrid[x].push_back(channel);
			for (int i = 0; i < (isHorizontal ? Wh : Wv); i++) {
				track* aTrack = new track(x, y, i);
				RoutingGrid[x][y].push_back(aTrack);
			}
		}
	}

}

void Routing::resetGrid() {
	for (int x = 0; x < RoutingGrid.size(); x++) {
		for (int y = 0; y < RoutingGrid[x].size(); y++) {
			for (int i = 0; i < RoutingGrid[x][y].size(); i++) {
				RoutingGrid[x][y][i]->resetTrack();
			}
		}
	}
}

void Routing::labelGrid() {
	for (int x = 0; x < RoutingGrid.size(); x++) {
		for (int y = 0; y < RoutingGrid[x].size(); y++) {
			for (int i = 0; i < RoutingGrid[x][y].size(); i++) {
				RoutingGrid[x][y][i]->setLabel(-1);
			}
		}
	}
}

int Routing::MazeRouting(int printOption) {
	// start Lee-Moore maze routing algorithm
	queue<track*> q;    // expansion list implemented as a FIFO queue
	int routeResult = 0;
	for (int p = 0; p < allPath.size(); p++) {
		// initialization
		bool reachedTarget = false;
		bool foundPathBack = true;
		int numSegment = 0;
		track* lastTrack;
		labelGrid();    // clear all labels in the Grid
		while (!q.empty()) q.pop();  // clear the expansion list
		// label start and target tracks
		bool shouldBreak = label0T(p, q, printOption);
		if (shouldBreak) { // no way out or no way in
			routeResult = p + 1;
			break;
		}
		while (!q.empty()) {  // expansion loop until target found
			track* tempTrack = q.front();
			// expand reachable tracks through 2 switch blocks
			vector<track*> adjacentTracks;
			expand(tempTrack, adjacentTracks);
			for (int t = 0; t < adjacentTracks.size(); t++) {   // determine whether or not to push the adjacent track into the queue
				track* adjacentTrack = adjacentTracks[t];
				if (adjacentTrack->getLabel() == -2) { // reached target
					reachedTarget = true;
					lastTrack = adjacentTrack;
					numSegment = tempTrack->getLabel() + 1; // record total number of track segments used
					break;
				}
				else if (adjacentTrack->getIsAvailable() && (adjacentTrack->getLabel() == -1)) { // unlabeled available track
					adjacentTrack->setLabelSource((tempTrack->getLabel() + 1), tempTrack->getSourceX(), tempTrack->getSourceY(), tempTrack->getSourceP());
					q.push(adjacentTrack);

				}
				else if ((tempTrack->getSourceX() == adjacentTrack->getSourceX()) &&
					(tempTrack->getSourceY() == adjacentTrack->getSourceY()) &&
					(tempTrack->getSourceP() == adjacentTrack->getSourceP())
					) {  //unavailable same-source track
					if (adjacentTrack->getLabel() == -1) { // unlabeled track
						adjacentTrack->setLabelSource((tempTrack->getLabel() + 1), tempTrack->getSourceX(), tempTrack->getSourceY(), tempTrack->getSourceP());
						q.push(adjacentTrack);
						//cout << "reuse happened!\n";
					}
					else if ((tempTrack->getLabel() + 1) < adjacentTrack->getLabel()) { // labeled same-source unavailable track with smaller distance
						adjacentTrack->setLabelSource((tempTrack->getLabel() + 1), tempTrack->getSourceX(), tempTrack->getSourceY(), tempTrack->getSourceP());
						q.push(adjacentTrack);
						//cout << "reuse happened!\n";
					}
				}
			} // end of adjacentTracks loop
			if (reachedTarget)
				break;
			q.pop();
		} // end of expanding loop
		if (!reachedTarget) {
			allPath[p]->setState(2);    // set as unroutable
			routeResult = p + 1;
			break;
		}
		else {
			// back trace to select the path
			allPath[p]->getTracks()->push_back(lastTrack); // record as used track for the path
			lastTrack->setAvailable(false);  // set as not available
			for (int d = numSegment - 1; d >= 0; d--) {
				vector<track*> adjacentTracks;
				expand(lastTrack, adjacentTracks);
				bool foundTrackBack = false;
				for (int t = 0; t < adjacentTracks.size(); t++) {   // determine whether or not to push the adjacent track into the queue
					track* adjacentTrack = adjacentTracks[t];
					if (adjacentTrack->getLabel() == d) {
						lastTrack = adjacentTrack;
						allPath[p]->getTracks()->push_back(lastTrack); // record as used track for the path
						lastTrack->setAvailable(false);  // set as not available
						foundTrackBack = true;
						break;
					}
				}
				if (!foundTrackBack) {
					foundPathBack = false;
					break;
				}
			}
		}
		if (foundPathBack) {
			if (printOption == 1) {
				cout << "Path " << p << " successfully back traced! Number of routing segments: " << numSegment + 1 << endl;
				// print path
				cout << "Print Path : ";
				for (int i = allPath[p]->getTracks()->size() - 1; i >= 0; i--) {
					int x = allPath[p]->getTracks()->at(i)->getGridX();
					int y = allPath[p]->getTracks()->at(i)->getGridY();
					int index = allPath[p]->getTracks()->at(i)->getIndex();
					if (i == 0)
						cout << "(" << x << ", " << y << ", " << index << ") \n";
					else
						cout << "(" << x << ", " << y << ", " << index << ") --> ";
				}
			}
			for (int i = allPath[p]->getTracks()->size() - 1; i >= 0; i--) {
				allPath[p]->setState(1);    // set as routed
			}
		}
		else {
			if (printOption == 1) {
				cout << "Path " << p << " failed to back trace!\n";
			}
			allPath[p]->setState(2);    // set as unroutable
			routeResult = p + 1;
			break;
		}

	} //end of path loop
	// output final result
	if (printOption == 1) {
		cout << "--------------------------------------------------------------------------------\n";
		if (!routeResult) {
			cout << "Final result: Routing is successful!!!\n";
		}
		else {
			cout << "Final result: Routing failed. Failed path number:" << routeResult << "\n";
		}
	}

	return routeResult;

}


void Routing::expand(track* tempTrack, vector<track*> &adjacentTracks) {
	int gridX = tempTrack->getGridX();
	int gridY = tempTrack->getGridY();
	int Index = tempTrack->getIndex();
	if (gridX % 2 == 0) { // for vertical tracks
		if (gridX > 0) {  // if not leftmost
			if (Wv < Wh) {
				int i = 0;
				while ((Index + i * Wv) < Wh) {  // planar connection + if Wv < Wh
					adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY][Index + i * Wv]);
					adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY + 1][Index + i * Wv]);
					i++;
				}
			}
			else {  // planar connection + if Wv >= Wh
				adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY][Index%Wh]);
				adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY + 1][Index%Wh]);
			}
		}
		if (gridX < (RoutingGrid.size() - 1)) { // if not rightmost
			if (Wv < Wh) {
				int i = 0;
				while ((Index + i * Wv) < Wh) {  // planar connection + if Wv < Wh
					adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY][Index + i * Wv]);
					adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY + 1][Index + i * Wv]);
					i++;
				}
			}
			else {  // planar connection + if Wv >= Wh
				adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY][Index%Wh]);
				adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY + 1][Index%Wh]);
			}
		}
		if (gridY > 0) {  // if not downmost
			adjacentTracks.push_back(RoutingGrid[gridX][gridY - 1][Index]);
		}
		if (gridY < (RoutingGrid[gridX].size() - 1)) {  // if not upmost
			adjacentTracks.push_back(RoutingGrid[gridX][gridY + 1][Index]);
		}
	}
	else {  // for horizontal tracks
		if (gridY > 0) {  // if not downmost
			if (Wh < Wv) {
				int i = 0;
				while ((Index + i * Wh) < Wv) {  // planar connection + if Wh < Wv
					adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY - 1][Index + i * Wh]);
					adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY - 1][Index + i * Wh]);
					i++;
				}
			}
			else {  // planar connection + if Wh >= Wv
				adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY - 1][Index%Wv]);
				adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY - 1][Index%Wv]);
			}
		}
		if (gridY < (RoutingGrid[gridX].size() - 1)) {  // if not upmost
			if (Wh < Wv) {
				int i = 0;
				while ((Index + i * Wh) < Wv) {  // planar connection + if Wh < Wv
					adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY][Index + i * Wh]);
					adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY][Index + i * Wh]);
					i++;
				}
			}
			else {  // planar connection + if Wh >= Wv
				adjacentTracks.push_back(RoutingGrid[gridX - 1][gridY][Index%Wv]);
				adjacentTracks.push_back(RoutingGrid[gridX + 1][gridY][Index%Wv]);
			}
		}
		if (gridX > 1) {  // if not leftmost
			adjacentTracks.push_back(RoutingGrid[gridX - 2][gridY][Index]);
		}
		if (gridX < (RoutingGrid.size() - 2)) { // if not rightmost
			adjacentTracks.push_back(RoutingGrid[gridX + 2][gridY][Index]);
		}

	}

}

bool Routing::label0T(int p, queue<track*> &q, int print) {
	int sourceX = allPath[p]->getXYP(0);
	int sourceY = allPath[p]->getXYP(1);
	int sourceP = allPath[p]->getXYP(2);
	int sinkX = allPath[p]->getXYP(3);
	int sinkY = allPath[p]->getXYP(4);
	int sinkP = allPath[p]->getXYP(5);

	bool noWayIn = true;
	bool shouldbreak = false;
	track* tempTrack;
	// label 0s
	for (int w = 0; w < RoutingGrid[2 * sourceX + 1][sourceY + 1].size(); w++) {
		tempTrack = RoutingGrid[2 * sourceX + 1][sourceY + 1][w];
		if (tempTrack->getIsAvailable()) {
			tempTrack->setLabelSource(0, sourceX, sourceY, sourceP);
			q.push(RoutingGrid[2 * sourceX + 1][sourceY + 1][w]);
		}
		else if ((tempTrack->getSourceX() == sourceX) && (tempTrack->getSourceY() == sourceY) && (tempTrack->getSourceP() == sourceP)) {
			q.push(RoutingGrid[2 * sourceX + 1][sourceY + 1][w]);
			tempTrack->setLabel(0);
		}
	}
	if (q.empty()) {
		if (print)
			cout << "Path " << p << " has no way out!\n";
		allPath[p]->setState(2);    // set as unroutable, TODO:change route sequence
		shouldbreak = true;
	}
	// label Targets(-2)
	for (int w = 0; w < RoutingGrid[2 * sinkX + 1][sinkY + 1].size(); w++) {
		tempTrack = RoutingGrid[2 * sinkX + 1][sinkY + 1][w];
		if (tempTrack->getIsAvailable()) {
			noWayIn = false;
			tempTrack->setLabelSource(-2, -1, -1, -1);
		}
		else if ((tempTrack->getSourceX() == sourceX) && (tempTrack->getSourceY() == sourceY) && (tempTrack->getSourceP() == sourceP)) {
			noWayIn = false;
			tempTrack->setLabel(-2);
		}
	}
	if (noWayIn) {
		if (print)
			cout << "Path " << p << " has no way in!\n";
		allPath[p]->setState(2);    // set as unroutable, TODO:change route sequence
		shouldbreak = true;
	}
	return shouldbreak;
}

int Routing::getTotalSegments() {
	int countS = 0;
	for (int x = 0; x < RoutingGrid.size(); x++) {
		for (int y = 0; y < RoutingGrid[x].size(); y++) {
			for (int i = 0; i < RoutingGrid[x][y].size(); i++) {
				if (!RoutingGrid[x][y][i]->getIsAvailable())
					countS++;
			}
		}
	}
	return countS;
}

vector<RoutePath*>* Routing::getPaths() {
	return &allPath;
}

vector< vector< vector<track*> > >* Routing::getGrid() {
	return &RoutingGrid;
}

