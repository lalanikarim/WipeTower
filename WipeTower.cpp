//
// Created by karim on 9/17/19.
//

#include "WipeTower.h"
#include <iostream>

WipeTower::WipeTower(float x, float y, float towerWidth, float lineWidth, int lines) {
    m_x = x;
    m_y = y;
    m_towerWidth = towerWidth;
    m_lineWidth = lineWidth;
    m_lines = lines;
    m_minx = m_miny = FLT_MAX;
    m_maxx = m_maxy = 0.0f;
    m_maxToolChanges = 0;
}

void WipeTower::openFile(string filename) {
    ifstream file(filename.c_str());
    stringstream ss;
    string line;
    while(getline(file,line))
    {
        if(line.find("G1") != string::npos)
        {
            float x,y;
            if(getX(line,x))
            {
                if (x < m_minx && x > 0) {
                    m_minx = x;
                }
                else if (x > m_maxx ) {
                    m_maxx = x;
                }
            }
            if(getY(line, y))
            {
                if (y < m_miny && y > 0) {
                    m_miny = y;
                }
                else if (y > m_maxy) {
                    m_maxy = y;
                }
            }
        }
        m_gcode.push_back(line);
    }
    int lastTool = -1;
    int lastLayer = 0;
    int toolCount = 0;
    float lastZ = 0;
    bool toolChange = false;
    bool layerChange = false;
    bool firstToolChange = true;
    bool startScriptEndFound = false;
    for(size_t i = 0; i < m_gcode.size(); i++)
    {
        line = m_gcode.at(i);

        if(!startScriptEndFound)
        {
            if(line.find("; START SCRIPT END") == string::npos) continue;
            startScriptEndFound = true;
        }

        if (toolChange)
        {
            if(line.find_first_of("T") == string::npos) continue;
            int currentTool = stoi(line.substr(1));
            cout << "Current Tool: " << currentTool << ", Last Tool: " << lastTool <<  endl;
            if(currentTool != lastTool)
            {
                if(lastTool != -1)
                {
                    toolCount++;
                }
                lastTool = currentTool;
            }
            toolChange = false;
        }
        if(line.find("; TOOL CHANGE") != string::npos)
        {
            toolChange = true;
            continue;
        }
        if(line.find(";BEFORE_LAYER_CHANGE") == 0)
        {
            size_t sp1, sp2;
            sp1 = line.find_first_of(" ");
            sp2 = line.find_first_of(" ", sp1 + 1);
            int currentLayer = stoi(line.substr(sp1+1,sp2 - sp1));
            lastZ = stof(line.substr(sp2+1));

            if (currentLayer != lastLayer)
            {
                m_layerToolChanges.insert(make_pair(lastLayer, toolCount));
                toolCount = 0;
                lastLayer = currentLayer;
            }
        }
    }
    if(toolCount > 0)
    {
        m_layerToolChanges.insert(make_pair(lastLayer, toolCount));
        toolCount = 0;
    }
    file.close();

    for(map<int,int>::iterator it = m_layerToolChanges.begin(); it != m_layerToolChanges.end(); it++)
    {
        if (it->second > 0)
        {
            cout << "Layer: " << it->first << ", Tool Changes: " << it->second << endl;
        }
        if(m_maxToolChanges < it->second)
        {
            m_maxToolChanges = it->second;
        }
    }

    cout << "Min X: " << m_minx << ", Min Y: " << m_miny << endl;
    cout << "Max X: " << m_maxx << ", Max Y: " << m_maxy << endl;
    cout << "Max Tool Changes: " << m_maxToolChanges << endl;
}

bool WipeTower::getX(string line, float &x) {
    return getLiteral(line,"X",x);
}

bool WipeTower::getY(string line, float &y) {
    return getLiteral(line,"Y", y);
}

bool WipeTower::getLiteral(string line, string literal, float &l) {
    size_t posl = line.find_first_of(literal);
    size_t posg1 = line.find("G1");
    if (posg1 != string::npos && posl != string::npos)
    {
        size_t possp = line.find_first_of(" ",posl);
        l = strtof(line.substr(posl+1,possp).data(),NULL);
        return true;
    }
    return false;
}

WipeTower::~WipeTower() {

}
