#ifndef __not_jelly_splash__animation__
#define __not_jelly_splash__animation__

#include <SFML/Graphics.hpp>

struct cAnimation {
public:
    bool                loadFromFile(const std::string& file);
    
    std::string         mName;
    sf::IntRect         mFirstPhaseRect;
    int                 mSteps;
    float               mTimeInMs;
};

#endif /* defined(__not_jelly_splash__animation__) */
