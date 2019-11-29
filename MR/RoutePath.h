#include <vector>
#include <iostream>
#include "track.h"

using namespace std;

class RoutePath{
private:
    int sourceX;
    int sourceY;
    int sourceP;
    int sinkX;
    int sinkY;
    int sinkP;

    int priority;
    int state;  // current state of this path: 0-not routed, 1-routed, 2-unroutable
    vector<track*> usedTracks;

public:
    RoutePath(int num0, int num1, int num2, int num3, int num4, int num5);
    ~RoutePath();

    void resetUsedTracks();
    void setPriority(int p);
    void setState(int s);
    void useTrack(track* thisTrack);
    int getXYP(int index);  // 0-5
    int getPriority(){ return priority; }
    int getState(){ return state; }
    vector<track*>* getTracks();


};
