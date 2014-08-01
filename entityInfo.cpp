#include "entityInfo.h"
#include <fstream>

bool cEntityInfo::loadFromFile(const std::string &file)
{
    std::ifstream   inFile(file);
    if ( !inFile.is_open() ) return false;
    
    inFile >> mName;
    short tmp;
    inFile >> tmp;
    
    mType = static_cast<EntType>(tmp);
    
    std::string stmp;
    
    inFile >> stmp;
    
    if ( stmp == "red" ) mColour = EntColour::red;
    if ( stmp == "green" ) mColour = EntColour::green;
    if ( stmp == "blue" ) mColour = EntColour::blue;
    if ( stmp == "yellow" ) mColour = EntColour::yellow;
    if ( stmp == "purple" ) mColour = EntColour::purple;
    if ( stmp == "random" ) mColour = EntColour::random;        // random is also the colour
                                                                // of colourless entities (e.g. guards)
    inFile >> stmp;
    if ( stmp == "fallible" ) mFallible = true;
    if ( stmp == "stable" ) mFallible = false;
    
    inFile >> mTexID;
    
    inFile >> stmp;
    if ( stmp != "ANIMATIONS" )
    {
        inFile.close(); return false;
    }
    inFile >> stmp;
    while ( stmp != "END" )
    {
        mAnimations.push_back(stmp);
        inFile >> stmp;
    }
    inFile.close();
    return true;
}
