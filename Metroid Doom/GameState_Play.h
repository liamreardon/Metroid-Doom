#pragma once

#include "Common.h"
#include "GameState.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

struct PlayerConfig 
{ 
    float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY; 
    std::string WEAPON; 
};


class GameState_Play : public GameState
{

protected:

    EntityManager           m_entityManager;
    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_openingDoor = false;
    bool                    m_doorCollision = false;
    bool                    m_shieldOn = false;
    std::vector<std::string> weaponArray;
    int                      weaponIndex = 0;
    std::string              m_currentWeapon;
    int                      levelIndex;
    std::vector<std::string> levelArray;
    std::string              m_currentLevel;
    sf::SoundBuffer         musicBuffer; // music load
    sf::Sound               music;
    sf::SoundBuffer         laserBuffer;        // laser load
    sf::Sound               laser;              // laser sound
    sf::SoundBuffer         explosionBuffer;    // load explosion
    sf::Sound               explosion;          // explosion sound
    sf::SoundBuffer         specialBuffer;      // load special sound
    sf::Sound               special;            // special sound
    sf::SoundBuffer         playerHitBuffer;    // load player hit
    sf::Sound               playerHit;          // player hit sound
    sf::SoundBuffer         collectKeyBuffer;   
    sf::Sound               collectKey;
    sf::SoundBuffer         doorLockedBuffer;
    sf::Sound               doorLocked;
    sf::SoundBuffer         doorOpenBuffer;
    sf::Sound               doorOpen;
    sf::SoundBuffer         portalBuffer;
    sf::Sound               portal;
    
    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void update();
    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> entity);
    void Jetpack();
    void sHealth();
    void sMovement();
    void sLifespan();
    void sUserInput();
    void sAnimation();
    void sCollision();
    void sRender();
    void sWeaponCycle(int n);
    void sAI();
    void spawnShield();
    
    

public:

    GameState_Play(GameEngine & game, const std::string & levelPath);

};
