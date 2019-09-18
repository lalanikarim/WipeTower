//
// Created by karim on 9/17/19.
//

#ifndef WIPETOWER_WIPETOWER_H
#define WIPETOWER_WIPETOWER_H

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <float.h>

using namespace std;

class WipeTower {
public:
    WipeTower(float x, float y, float towerWidth, float lineWidth, int lines);
    void openFile(string filename);
    ~WipeTower();
private:
    bool getLiteral(string line, string literal, float &l);
    bool getX(string line, float &x);
    bool getY(string line, float &y);
    bool getE(string line, float &e);
    bool getF(string line, float &f);

    float m_x, m_y;
    float m_towerWidth, m_lineWidth;
    int m_lines;
    float m_minx, m_miny;
    float m_maxx, m_maxy;
    float m_maxToolChanges;
    vector<string> m_gcode;
    map<int,int> m_layerToolChanges;
    vector<string> m_layerLastPosition;
};


#endif //WIPETOWER_WIPETOWER_H
