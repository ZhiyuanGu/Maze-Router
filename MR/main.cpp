#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "draw.h"
#include <time.h>

using namespace std;

int main()
{
	// configuration
	bool q1 = 1; // question1 switch
	bool q1_graphics = 1;   // question1 graphics switch
	bool q1_detailPrint = 0;    // print out detailed information for q1
	bool q2 = 0; // question2 switch
	bool q3 = 0; // question3 switch
	bool manhattan = 1;	// manhattan distance optimization switch
	int optL_max = 1000;  // maximum allowed shuffle optimization level

	// input routing parameters file path
	cout << "Please enter file path(cct1, etc.):\n";
	string filePath;
	getline(cin, filePath);
	// input optimization level (1 + total times to shuffle queue)
	int optL = optL_max + 1;
	if (q2 || q3) {
		while (optL > optL_max || optL < 1) {
			cout << "Please enter optimization level(times of shuffling, min:1, max:" << optL_max << "):\n";
			cin >> optL;
		}
	}
	// start point to record running time
	clock_t start_time = clock();
	//question 1
	if (q1) {
		cout << "--------------------------------------------------------------------------------\n";
		Routing* MR = new Routing();
		MR->inputFile(filePath);
		// test: print input information

		cout << "Grid size:" << MR->getGridSize() << endl;
		cout << "Wv:" << MR->getWv() << endl;
		cout << "Wh:" << MR->getWh() << endl;
		cout << "Number of routing paths:" << MR->getPaths()->size() << endl;

		if (manhattan) {
			MR->setManhattanPriority();
			MR->sortPaths();
		}
		MR->createGrid();

		// start routing algorithm
		MR->MazeRouting(q1_detailPrint);
		// print total number of routing segments used
		int countS = MR->getTotalSegments();
		cout << "Total number of routing segments used: " << countS << endl;
		// open the graphics tool
		if (q1_graphics)
			openGraphics(MR);
		delete MR;
	}

	// minimize channel width
	// input original width
	ifstream infile;
	infile.open(filePath.c_str(), std::ifstream::in);
	int temp, file_Wv, file_Wh;
	infile >> temp;
	infile >> file_Wv;
	infile >> file_Wh;
	infile.close();

	if (q2) {
		// minimize channel width when Wh = Wv
		cout << "--------------------------------------------------------------------------------\n";
		int smallestWidth = file_Wh;
		int countS2;
		int isFailure;
		for (int w = 1; w <= file_Wh; w++) {
			//cout<<"q2 w:"<< w <<"\n";
			Routing* MR = new Routing(w, w);
			MR->inputFile(filePath);
			if (manhattan) {
				MR->setManhattanPriority();
				MR->sortPaths();
			}
			MR->createGrid();
			for (int opt = 0; opt < optL; opt++)
			{
				// put the problem case to the front
				if (opt > 0) {
					MR->resetPaths();
					MR->resetGrid();
					if (isFailure == 1) {
						break;
					}
					else {
						RoutePath* ProblemCase = MR->getPaths()->at(isFailure - 1);
						MR->getPaths()->erase(MR->getPaths()->begin() + isFailure - 1);
						MR->getPaths()->insert(MR->getPaths()->begin(), ProblemCase);
					}
				}

				isFailure = MR->MazeRouting(0);
				if (isFailure == 0) {
					countS2 = MR->getTotalSegments();
					smallestWidth = w;
					break;
				}
			}
			delete MR;
			if (isFailure == 0)
				break;

		}
		if (isFailure != 0)
			cout << "Failed to find a solution!\n";
		else {
			cout << "Smallest channel width when Wh=Wv is: " << smallestWidth << endl;
			cout << "Total number of routing segments used: " << countS2 << endl;
		}
	}
	if (q3) {
		// minimize channel width when Wh != Wv
		cout << "--------------------------------------------------------------------------------\n";
		int smallestWh = file_Wh;
		int smallestWv = file_Wv;
		int countS3;
		bool readyToBreak = false;
		int isFailure;
		for (int wt = 2; wt <= file_Wh + file_Wv; wt++) {
			for (int wv = 1; (wv < wt); wv++) {
				int wh = wt - wv;
				Routing* MR = new Routing(wv, wh);
				MR->inputFile(filePath);
				if (manhattan) {
					MR->setManhattanPriority();
					MR->sortPaths();
				}
				MR->createGrid();
				for (int opt = 0; opt < optL; opt++)
				{
					// put the problem case to the front
					if (opt > 0) {
						MR->resetPaths();
						MR->resetGrid();
						if (isFailure == 1) {
							break;
						}
						else {
							RoutePath* ProblemCase = MR->getPaths()->at(isFailure - 1);
							MR->getPaths()->erase(MR->getPaths()->begin() + isFailure - 1);
							MR->getPaths()->insert(MR->getPaths()->begin(), ProblemCase);
						}
					}
					isFailure = MR->MazeRouting(0);
					if (isFailure == 0) {
						readyToBreak = true;
						if (wv + wh < smallestWh + smallestWv) {
							smallestWv = wv;
							smallestWh = wh;
							countS3 = MR->getTotalSegments();
						}
						else if ((wv + wh == smallestWh + smallestWv) && MR->getTotalSegments() < countS3) {
							countS3 = MR->getTotalSegments();
							smallestWv = wv;
							smallestWh = wh;
						}
						break;
					}

				}
				delete MR;
			}
			if (readyToBreak)
				break;
		}
		if (!readyToBreak)
			cout << "Failed to find a solution!\n";
		else {
			cout << "Smallest channel widths when allowing Wh!=Wv are: Wv = " << smallestWv << ", Wh = " << smallestWh << endl;
			cout << "Total number of routing segments used: " << countS3 << endl;
		}

	}




	clock_t end_time = clock();
	cout << "--------------------------------------------------------------------------------\n";
	cout << "Total running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC << "s" << endl;

	return 0;
}

