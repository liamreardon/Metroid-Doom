#include "GameState_Menu.h"
#include "GameState_Play.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "ResourcePath.hpp"

GameState_Menu::GameState_Menu(GameEngine & game)
    : GameState(game)
{
    init();
}

void GameState_Menu::init()
{
    m_menuStrings.push_back("Level  1");
    m_menuStrings.push_back("Level  2");
    m_menuStrings.push_back("Level  3");
    m_menuStrings.push_back("Boss Level");

    m_levelPaths.push_back("level1.txt");
    m_levelPaths.push_back("level2.txt");
    m_levelPaths.push_back("level3.txt");
    m_levelPaths.push_back("bosslevel.txt");

    m_menuText.setFont(m_game.getAssets().getFont("Megaman"));
    m_menuText.setCharacterSize(64);
    
    float xpos;
    float ypos;
    std::string animationName;
    std::string musicPath;
    // reset the entity manager every time we load a level
    
    // Read in assets
    std::ifstream inFile(resourcePath()+"menusetup.txt");
    
    while (inFile.good())
    {
        std::string token;
        inFile >> token;
        
        if (token == "MenuTitle")
        {
            inFile >> animationName >> xpos >>  ypos;
            
            m_menuTitle = m_entityManager.addEntity("title");
            m_menuTitle->addComponent<CTransform>(Vec2(xpos, ypos));
            m_menuTitle->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);

        }
        
        if (token == "MenuBack")
        {
            inFile >> animationName >> xpos >>  ypos;
            
            m_menuBackground = m_entityManager.addEntity("title");
            m_menuBackground->addComponent<CTransform>(Vec2(xpos, ypos));
            m_menuBackground->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);
            
        }
        
//        if (token == "Music")
//        {
//            inFile >> musicPath;
//            
//            if (!musicBuffer.loadFromFile(resourcePath()+musicPath)) {
//                exit(-1);
//            }
//            
//            music.setBuffer(musicBuffer);
//            music.setLoop(true);
//            music.setVolume(15.0f);
//            music.play();
//            
//        }
        
        
    }
    
}

void GameState_Menu::update()
{
    m_entityManager.update();

    sUserInput();
    sRender();
    sAnimation();
}

// Menu Animations
void GameState_Menu::sAnimation()
{
    
    m_menuTitle->getComponent<CAnimation>()->animation.update();
    m_menuBackground->getComponent<CAnimation>()->animation.update();
    
}

void GameState_Menu::sUserInput()
{
    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_game.quit();
        }
        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape: 
                { 
                    m_game.quit(); 
                    break; 
                }
                case sf::Keyboard::W: 
                {
                    if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
                    else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
                    break;
                }
                case sf::Keyboard::S: 
                { 
                    m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size(); 
                    break; 
                }
                case sf::Keyboard::D: 
                {
                    music.stop();
                    m_game.pushState(std::make_shared<GameState_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
                    break; 
                }
            }
        }
    }

}

void GameState_Menu::sRender()
{
    // clear the window to a blue
    m_game.window().setView(m_game.window().getDefaultView());
    m_game.window().clear(sf::Color(0, 0, 0));
    
    // Draw backround
    for (auto e : m_entityManager.getEntities())
    {
        auto transform = e->getComponent<CTransform>();
        
        if (e->getComponent<CAnimation>()->animation.getName() == "MenuBackground")
        {
            auto animation = e->getComponent<CAnimation>()->animation;
            animation.getSprite().setRotation(transform->angle);
            animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
            animation.getSprite().setScale(transform->scale.x, transform->scale.y);
            m_game.window().draw(animation.getSprite());
        }
    }

    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(48);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color::Black);
    m_menuText.setPosition(sf::Vector2f(10, 10));
    m_game.window().draw(m_menuText);
    
    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color(255, 215, 0) : sf::Color::White);
        m_menuText.setPosition(sf::Vector2f(460, 275 + i * 72));
        m_game.window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(17);
    m_menuText.setFillColor(sf::Color(255, 215, 0));
    m_menuText.setString("movement: w a s d    back: esc   o: open door    rshift: toggle weapon    p: pause  ");
    m_menuText.setPosition(sf::Vector2f(10, 690));
    m_game.window().draw(m_menuText);
    
    
    // Draw Title
    for (auto e : m_entityManager.getEntities())
    {
        auto transform = e->getComponent<CTransform>();
        
        if (e->getComponent<CAnimation>()->animation.getName() == "Title")
        {
            auto animation = e->getComponent<CAnimation>()->animation;
            animation.getSprite().setRotation(transform->angle);
            animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
            animation.getSprite().setScale(transform->scale.x, transform->scale.y);
            m_game.window().draw(animation.getSprite());
        }
    }
    
    

    m_game.window().display();
}
