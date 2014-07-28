#ifndef not_jelly_splash_globalConstants_h
#define not_jelly_splash_globalConstants_h

#include <string>
#include <SFML/Graphics.hpp>

// Font and texture IDs
enum class FontID   :   unsigned int { defaultFont };
enum class TexID    :   unsigned int { introScreen };

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


#endif
