#include "guiMenu.h"

void cMenu::setup(std::vector<std::string>& items,
                  sf::Font& font,
                  int size,
                  sf::Vector2f pos,
                  sf::Color piColor,
                  sf::Color aiColor,
                  sf::Color hColor)
{
    // Set up menu items and check width, height, for
    // positioning purposes
    
    mActiveItemColor = aiColor;
    mPassiveItemColor = piColor;
    mHighlightColor = hColor;
    
    float width = 0.0;
    for ( auto& i : items )
    {
        mItems.push_back(sf::Text(i, font, size));
        mItems.back().setColor(piColor);
        
        if ( mItems.back().getLocalBounds().width > width )
        {
            width = mItems.back().getLocalBounds().width;
        }
        
        if ( mItems.back().getLocalBounds().height > mHeight )
        {
            mHeight = mItems.back().getLocalBounds().height;
        }
        
        ++mItemCount;
    }
    
    // Negative pos coordinates mean: invert these coords
    // and center the thing in them
    
    if ( pos.x < 0 )
    {
        pos.x *= -1;
        pos.x -= width;
        pos.x /= 2;
        
    }
    
    if ( pos.y < 0 )
    {
        pos.y *= -1;
        pos.y -= mHeight * items.size();
        pos.y /= 2;
    }
    
    mTop = mItems[0].getLocalBounds().top + pos.y - mHeight / 2;
    mHighlight[0].position.x = pos.x - 20;
    mHighlight[1].position.x = pos.x + width + 35;
    mHighlight[2].position.x = pos.x + width + 15;
    mHighlight[3].position.x = pos.x - 20;
    updateHighlight();
    
    for ( auto& i : mHighlight )
    {
        i.color = hColor;
    }
    
    mHighlight[2].color.a = 80;
    mHighlight[3].color.a = 80;
    
    // Align items centered on top of one another
    size_t c = 0;
    for ( auto& i : mItems )
    {
        auto diff = (width - i.getLocalBounds().width) / 2;
        i.setPosition(pos.x + diff, pos.y + c*mHeight);
        c += 2;
    }
    
    mItems[mActiveID].setColor(mActiveItemColor);
}

cMenu::cMenu(sf::RenderWindow& w,
             std::vector<std::string>& items,
             sf::Font& font, int size,
             sf::Vector2f pos,
             sf::Color piColor,
             sf::Color aiColor,
             sf::Color hColor):
rWindow { w }
{
    setup(items, font, size, pos, piColor, aiColor, hColor);
}

void cMenu::updateHighlight()
{
    mHighlight[0].position.y = mActiveID * 2 * mHeight + mTop - mHeight / 8;
    mHighlight[1].position.y = mHighlight[0].position.y;
    mHighlight[2].position.y = mHighlight[1].position.y + mHeight + mHeight;
    mHighlight[3].position.y = mHighlight[2].position.y;
    for (auto i = 0; i < mItemCount; ++i)
    {
        if ( i == mActiveID ) mItems[i].setColor(mActiveItemColor);
        else mItems[i].setColor(mPassiveItemColor);
    }
    
}

void cMenu::render(sf::RenderStates s)
{
    // First: draw highlight, if needed
    if ( mDrawHighlight )
    {
        mItems[mActiveID].setColor(mActiveItemColor);
        rWindow.draw(&mHighlight[0], 4, sf::Quads, s);
    } else
    {
        mItems[mActiveID].setColor(mPassiveItemColor);
    }
    
    // Then draw items
    for ( auto& i : mItems )
        rWindow.draw(i, s);
    
    // Maybe render title, hints
    if ( mTitleOn ) rWindow.draw(tTitle);
    if ( mHintsOn ) rWindow.draw(mHints[mActiveID]);
}

void cMenu::setTitle(const std::string& title, sf::Font& font,
                     int size, sf::Color col, sf::Vector2f pos)
{
    mTitleOn = true;
    tTitle.setString(title);
    tTitle.setFont(font);
    tTitle.setCharacterSize(size);
    tTitle.setColor(col);
    
    // Negative values refer to defaults.
    if ( pos.x < 0 )
    {
        pos.x = ((-pos.x) - tTitle.getLocalBounds().width) / 2.0;
    }
    
    if ( pos.y < 0 )
    {
        pos.y = ((-pos.y) - tTitle.getLocalBounds().height) / 10.0;
    }
    
    tTitle.setPosition(pos.x, pos.y);
    
}


void cMenu::setHints(const std::vector<std::string>& hintItems,
                     sf::Font& font, int size, sf::Color col, sf::Vector2f pos)
{
    mHintsOn = true;
    for ( auto& i : hintItems )
    {
        mHints.push_back(sf::Text(i, font, size));
        mHints.back().setColor(col);
        mHints.back().setPosition(pos);
    }
}

bool cMenu::update()
{
    
    // If nothing selected yet, check keys and see
    // where we're going with them
    
    if ( mStatus == MenuStatus::normal )
    {
        sf::Event event;
        while ( rWindow.pollEvent(event) )
        {
            if ( event.type == sf::Event::KeyPressed )
            {
                switch ( event.key.code )
                {
                    case ( sf::Keyboard::Key::Up ) : { moveUp(); break; }
                    case ( sf::Keyboard::Key::Down ) : { moveDown(); break; }
                    case ( sf::Keyboard::Key::Return ) : { selectItem(); break; }
                    case ( sf::Keyboard::Key::Escape ) :
                    {
                        mSelectedItem = "__BACK__";
                        return true; break;
                    }
                    default: break;
                }
            }
        }
    }
    
    // In any case, update blinking if necessary
    if ( mStatus == MenuStatus::blinking )
    {
        if ( mStillBlinking > sf::Time::Zero )
        {
            mDrawHighlight = (mStillBlinking.asMilliseconds()/100) % 2;
            mStillBlinking -= mClock.restart();
        } else
        {
            mStatus = MenuStatus::done;
            mSelectedItem = mItems[mActiveID].getString();
            return true;
        }
    }
    
    return false;   // Return false if no menu item selected
}

void cMenu::moveUp()
{
    mItems[mActiveID].setColor(mPassiveItemColor);
    mActiveID = mActiveID == 0 ? mItemCount-1 : mActiveID-1;
    mItems[mActiveID].setColor(mActiveItemColor);
    updateHighlight();
}

void cMenu::moveDown()
{
    mItems[mActiveID].setColor(mPassiveItemColor);
    mActiveID = mActiveID == mItemCount-1 ? 0 : mActiveID+1;
    mItems[mActiveID].setColor(mActiveItemColor);
    updateHighlight();
}

void cMenu::selectItem()
{
    mStatus = MenuStatus::blinking;
    mClock.restart();
    mStillBlinking = sf::seconds(kBlinkTimeSecs);
}

void cMenu::reset()
{
    mStatus = MenuStatus::normal;
    mActiveID = 0;
    mDrawHighlight = true;
    updateHighlight();
}