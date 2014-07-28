#include "animation.h"
#include <fstream>

bool cAnimation::loadFromFile(const std::string& file)
{
    std::ifstream   inFile(file);
    if ( !inFile.is_open() ) return false;
    
    inFile >> mName;
    inFile >> mFirstPhaseRect.left >> mFirstPhaseRect.top;
    inFile >> mFirstPhaseRect.width >> mFirstPhaseRect.height;
    inFile >> mSteps;
    inFile >> mTimeInMs;

    inFile.close();
    return true;
}
