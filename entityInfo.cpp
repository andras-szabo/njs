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
    
    inFile >> mTexID;
    
    std::string stmp;
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
