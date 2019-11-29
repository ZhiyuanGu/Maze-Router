#include "track.h"

using namespace std;

track::track(int x, int y, int i){
    gridX = x;
    gridY = y;
    Index = i;
    label = -1; // initial: -1, target: -2, others:0,1...
    sourceX = -1;
    sourceY = -1;
    sourceP = -1;
    isAvailable = true;

}

void track::resetTrack(){
    label = -1; // initial: -1, target: -2, others:0,1...
    sourceX = -1;
    sourceY = -1;
    sourceP = -1;
    isAvailable = true;
}

void track::setLabel(int l) {
    label = l;
}

void track::setLabelSource(int l, int x, int y, int p){
    label = l;
    sourceX = x;
    sourceY = y;
    sourceP = p;
}

void track::setAvailable(bool notUsed){
    isAvailable = notUsed;
}


