
using namespace std;

class track{
private:
    int gridX;
    int gridY;
    int Index;
    int label; // initial: -1, target: -2, others:0,1...
    int sourceX;
    int sourceY;
    int sourceP;
    bool isAvailable;

public:
    track(int x, int y, int i);

    void resetTrack();
    void setLabel(int l);
    void setLabelSource(int l, int x, int y, int p);
    void setAvailable(bool notUsed);
    int getGridX(){ return gridX; }
    int getGridY(){ return gridY; }
    int getIndex(){ return Index; }
    int getLabel(){ return label; }
    int getSourceX(){ return sourceX; }
    int getSourceY(){ return sourceY; }
    int getSourceP(){ return sourceP; }
    bool getIsAvailable(){ return isAvailable; }

};

