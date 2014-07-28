#ifndef not_jelly_splash_globalConstants_h
#define not_jelly_splash_globalConstants_h

#include <string>
#include <SFML/Graphics.hpp>

// Entity IDs (colour is an attribute of entities)
enum class EntType  : short int { jelly = 1, superJelly = 2,
                                  diamond = 6,
                                  block = 7,
                                  guard = 8 };
enum class EntColour: short int { random = 0, red = 1, green = 2, blue = 3, yellow = 4, purple = 5 };

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

// Graphics constants
const float                     gkCellPixSizeX { 100 };
const float                     gkCellPixSizeY { 100 };
const sf::Vector2f              gkClosedBkgTexCoords { 0, 0 };
const sf::Vector2f              gkLightBkgTexCoords { 100, 0 };
const sf::Vector2f              gkDarkBkgTexCoords { 200, 0 };
const float                     gkScrollSpeed { 200 };
const float                     gkJellySpeed { 600 };               // e.g. when zigzagging

#endif
