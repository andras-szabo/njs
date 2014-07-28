#ifndef __not_jelly_splash__entityInfo__
#define __not_jelly_splash__entityInfo__

#include "globalConstants.h"

struct cEntityInfo {
    bool    loadFromFile(const std::string& file);
    
    EntType                     mType;
    std::string                 mName;
    std::string                 mTexID;
    int                         mAnimCount;
    std::vector<std::string>    mAnimations;
};

#endif /* defined(__not_jelly_splash__entityInfo__) */
