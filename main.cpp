#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <float.h>
#include <vector>
#define EPSILON 0.001f

#include "WipeTower.h"

using namespace std;

int main(int argc, char ** argv) {
    cout << "Hello, World!" << endl;
    if (argc > 1)
    {
        char * fileName = argv[1];

        WipeTower w(0,0,60,0.5,10);
        w.openFile(string(fileName));

        return 0;
        ifstream file(fileName);
        stringstream ss;
        string line;
        vector<string> gcodelines;
        float lastx = FLT_MIN,x,e;
        while(getline(file,line))
        {
            gcodelines.push_back(line);
        }
        file.close();

        bool toolchange = false;
        string toolchange_x,toolchange_y;
        for(size_t i = 0; i < gcodelines.size(); i++)
        {
            line = gcodelines.at(i);
            if(line.substr(0,1) == "T")
            {
                toolchange = true;
                continue;
            }
            if(toolchange && line.substr(0,2) == "G1" &&
               line.find_first_of("X") != string::npos &&
               //line.find_first_of("Y") != string::npos &&
               line.find_first_of("E") == string::npos &&
               line.find_first_of("Z") == string::npos)
            {
                /*float locx, locy*/;
                size_t xpos, ypos, xspos, yspos;
                xpos = ypos = xspos = yspos = string::npos;
                xpos = line.find_first_of("X");
                xspos = line.find_first_of(" ",xpos+1);
                toolchange_x = line.substr(xpos+1,xspos-xpos - 1);
                ypos = line.find_first_of("Y");
                if(ypos != string::npos)
                {
                    yspos = line.find_first_of(" ",ypos+1);
                    //locx = strtof(line.substr(xpos+1,xspos - xpos - 1).data(),NULL);

                    //gcodelines.at(i) = "G1 X" + line.substr(xpos+1,xspos - xpos - 1) + ";" + line;
                    //gcodelines.at(i) = " G1 Y" + line.substr(ypos+1,yspos - ypos - 1) + ";" + line;
                    toolchange_y = line.substr(ypos+1,yspos - ypos - 1);
                    gcodelines.at(i) = ";" + line;
                }
                else
                {
                    gcodelines.at(i) = ";" + line;
                }
                cout << gcodelines.at(i) << endl;
                toolchange = false;
                continue;
            }
            if(!toolchange_x.empty() && line.substr(0,19) == "; CP TOOLCHANGE END")
            {
                //cout << "Tool Change End" << endl;
                if(toolchange_x.length() > 0)
                {
                    stringstream ss;
                    //ss << " G1 X135 Y300" << endl;
                    //ss << " G1 X165 E30 F2100" << endl;
                    ss << " G1 X" + toolchange_x;
                    if (!toolchange_y.empty())
                    {
                        ss << " Y" << toolchange_y;
                    }
                    ss << endl;
                    ss << "G91" << endl;
                    ss << "G1 Z0.5" << endl;
                    ss << "G90" << endl;
                    ss << " G1 X150 Y300" << endl;
                    ss << line;
                    //gcodelines.at(i) = " G1 X" + toolchange_x + line;
                    gcodelines.at(i) = ss.str();
                    cout << gcodelines.at(i) << endl;
                }
                toolchange_x.clear();
                toolchange_y.clear();
            }
        }

        for(size_t i = 0; i < gcodelines.size(); i++)
        {
            line = gcodelines.at(i);
            if (line.substr(0,2) == "G1")
            {
                if(line.find_first_of("X") != string::npos)
                {
                    size_t xpos = line.find_first_of("X");
                    size_t spos = line.find_first_of(" ",xpos+1);
                    if(spos != string::npos)
                    {
                        x = strtof(line.substr(xpos+1,spos - xpos - 1).data(),NULL);
                    }
                    else
                    {
                        x = strtof(line.substr(xpos+1).data(),NULL);
                    }
                    if (abs(lastx - FLT_MIN) < EPSILON)
                    {
                        lastx = x;
                    }
                    if(line.find_first_of("E") != string::npos && line.find_first_of("Y") == string::npos)
                    {
                        size_t epos = line.find_first_of("E");
                        size_t espos = line.find_first_of(" ",epos+1);
                        if(espos != string::npos)
                        {
                            e = strtof(line.substr(epos+1,espos - epos - 1).data(),NULL);
                        }
                        else
                        {
                            e = strtof(line.substr(epos+1).data(),NULL);
                        }
                        if (abs(lastx - x) > EPSILON && e > abs(lastx - x))
                        {
                            float f = 0.0f;
                            size_t fpos = line.find_first_of("F");
                            if(fpos != string::npos)
                            {
                                size_t fspos = line.find_first_of(" ",fpos+1);
                                if(fspos != string::npos)
                                {
                                    f = strtof(line.substr(fpos + 1,fspos - fpos - 1).data(),NULL);
                                }
                                else
                                {
                                    f = strtof(line.substr(fpos + 1).data(),NULL);
                                }
                            }

                            float xdist = abs(lastx - x);
                            float edist = 0.0f;
                            bool curx = true;
                            do {
                                edist = edist + xdist;
                                ss << "G1 X" << (curx ? x : lastx) << " E" << xdist;
                                if (f > 0.0f) {
                                    ss << " F" << f;
                                }
                                ss << endl;
                                curx = !curx;
                            } while(edist + xdist < e);

                            if (e - edist > EPSILON)
                            {
                                ss << "G1 X" << (curx ? x : lastx) << " E" << (e - edist);

                                if (f > 0.0f) {
                                    ss << " F" << f;
                                }
                                ss << endl;
                            }
                        }
                        else
                        {
                            ss << line << endl;
                        }
                    }
                    else
                    {
                        ss << line << endl;
                    }
                    lastx = x;
                }
                else
                {
                    ss << line << endl;
                }
            }
            else
            {
                ss << line << endl;
            }
        }

        string newFileName(fileName);

        newFileName = newFileName.substr(0,newFileName.find_first_of(".",0)) + "_wt.gcode";
        //cout << "New File: " << newFileName << endl;
        ofstream newfile(newFileName);
        newfile << ss.str() << endl;
        newfile.close();
    }
    return 0;
}