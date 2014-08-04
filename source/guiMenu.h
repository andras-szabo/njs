#ifndef __sstc__guiMenu__
#define __sstc__guiMenu__

#include <SFML/Graphics.hpp>
#include <queue>

class cMenu {
public:
    
    enum class MenuStatus : unsigned short int { normal, blinking, done };
    
    cMenu(sf::RenderWindow& w): rWindow { w } {  };
    
    cMenu(sf::RenderWindow& w,
          std::vector<std::string>& items,
          sf::Font& font,
          int size,
          sf::Vector2f pos,                         // negative "pos" has special meaning, see guiMenu.cpp
          sf::Color piColor = sf::Color::White,     // passive item color
          sf::Color aiColor = sf::Color::Black,     // active item color
          sf::Color hColor = sf::Color::White);     // highlighting color
    
    void                    setup(std::vector<std::string>&,
                                  sf::Font&, int, sf::Vector2f,
                                  sf::Color piColor = sf::Color::White,
                                  sf::Color aiColor = sf::Color::Black,
                                  sf::Color hColor = sf::Color::White);
    
    void                    setTitle(const std::string& title, sf::Font&,
                                     int size, sf::Color col,
                                     sf::Vector2f pos);
    
    void                    setHints(const std::vector<std::string>& hintItems, sf::Font&,
                                     int size, sf::Color,
                                     sf::Vector2f pos);
    
    bool                    update();               // returns true if item chosen
    void                    render(sf::RenderStates = sf::RenderStates::Default);
    void                    reset();
    
    const std::string&      getSelectedItem() const { return mSelectedItem; }
    
private:
    void                    updateHighlight();
    void                    moveUp();
    void                    moveDown();
    void                    selectItem();
    
public:
    MenuStatus               mStatus { MenuStatus::normal };
    
private:
    sf::RenderWindow&       rWindow;
    sf::Text                tTitle;
    std::vector<sf::Text>   mItems;
    std::vector<sf::Text>   mHints;                 // Hints: explaining what each item does
    
    bool                    mTitleOn { false };
    bool                    mHintsOn { false };
    
    sf::Color               mPassiveItemColor;
    sf::Color               mActiveItemColor;
    sf::Color               mHighlightColor;
    sf::Vertex              mHighlight[4];
    size_t                  mActiveID { 0 };
    size_t                  mItemCount { 0 };
    float                   mHeight { 0 };
    float                   mTop { 0 };
    bool                    mDrawHighlight { true };
    
    sf::Time                mStillBlinking { sf::Time::Zero };
    sf::Clock               mClock;
    float                   kBlinkTimeSecs { 0.5 };
    
    std::string             mSelectedItem { "__NONE__" };
};

#endif /* defined(__sstc__guiMenu__) */
