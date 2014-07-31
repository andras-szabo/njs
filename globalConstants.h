#ifndef not_jelly_splash_globalConstants_h
#define not_jelly_splash_globalConstants_h

#include <string>
#include <SFML/Graphics.hpp>

// Entity IDs (colour is an attribute of entities)
enum class EntType  : short int { jelly = 1,
                                  diamond = 2,
                                  block = 3,
                                  guard = 4 };

enum class EntColour: short int { random = 0,         // or n/a
                                red = 1,
                                green = 2,
                                blue = 3,
                                yellow = 4,
                                purple = 5
    };
    
// Entity states
enum class EntState: short int { normal, exploding, moving, explOver };
    
// Directions
enum class Direction : short int { undecided = 0, horizontal = 1, vertical = 2 };
    
// Game states
enum class GameState :  unsigned int { waiting, executing, refilling, aftermath };

// Font and texture IDs
enum class FontID   :   unsigned int { defaultFont };

// Stack action IDs
enum class SAID     :   unsigned int { nothing, push, pop, swap, returnTo, clear };

// Window and view sizes
const sf::Vector2f              gkWindowSize { 800, 600 };
const sf::Vector2f              gkViewSize { 1200, 800 };

// Version string
const std::string               gkVersion { " alpha 0.1 " };

// Intro fade in / proceed length
const float                     gkIntroFadeInSecs { 1.5 };
const float                     gkIntroProceedSecs { 5.0 };
    
// Explosion delay
const sf::Time                  gkExplosionDelay { sf::seconds(0.1) };

// Significant bits
const int                       gkSuperBit = 32;
const int                       gkStuckBit = 64;
const int                       gkSlimeBit = 128;
    
// Graphics constants
const float                     gkCellPixSizeX { 89 };
const float                     gkCellPixSizeY { 89 };
const sf::Vector2f              gkClosedBkgTexCoords { 178, 0 };
const sf::Vector2f              gkLightBkgTexCoords { 0, 0 };
const sf::Vector2f              gkDarkBkgTexCoords { 89, 0 };
const sf::Vector2f              gkSlimeTexCoords { 445, 0 };
const float                     gkScrollSpeed { 200 };
const float                     gkJellySpeed { 400 };               // e.g. when zigzagging
const float                     gkNearEnough { 30 };
const float                     gkScrLeft { (gkViewSize.x - (gkCellPixSizeX * 7)) / 2.f };
const float                     gkScrTop { 0 };
const sf::Color                 gkHilightColor (220, 120, 220, 120);
const int                       gkSideLeeway { 8 }; // pixels
#endif
