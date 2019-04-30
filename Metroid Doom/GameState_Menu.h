#pragma once

#include "Common.h"
#include "GameState.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class GameState_Menu : public GameState
{

protected:

    EntityManager               m_entityManager;
    std::string                 m_title;
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_levelPaths;
    sf::Text                    m_menuText;
    size_t                      m_selectedMenuIndex = 0;
    std::shared_ptr<Entity>     m_menuTitle;
    std::shared_ptr<Entity>     m_menuBackground;
    sf::SoundBuffer             musicBuffer; // music load
    sf::Sound                   music;
    
    void init();
    void update();
    void sUserInput();
    void sRender();
    void sAnimation(); // Add title animation

public:

    GameState_Menu(GameEngine & game);

};
