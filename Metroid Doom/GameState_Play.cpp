#include "GameState_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include <cmath>
#include "ResourcePath.hpp"

GameState_Play::GameState_Play(GameEngine & game, const std::string & levelPath)
    : GameState(game)
    , m_levelPath(levelPath)
{
    init(m_levelPath);
}

void GameState_Play::init(const std::string & levelPath)
{
    loadLevel(resourcePath()+levelPath);
    
    if (levelPath == "level1.txt")
    {
        levelIndex = 0;
    }
    if (levelPath == "level2.txt")
    {
        levelIndex = 1;
    }
    if (levelPath == "level3.txt")
    {
        levelIndex = 2;
    }
    if (levelPath == "bosslevel.txt")
    {
        levelIndex = 3;
    }

    spawnPlayer();

    // Load laser sound
    if (!laserBuffer.loadFromFile(resourcePath()+"sounds/laser.wav")) {
        exit(-1);
    }
    
    laser.setBuffer(laserBuffer);
    laser.setVolume(30.0f);
    
    // Load explosion sound
    if (!explosionBuffer.loadFromFile(resourcePath()+"sounds/explosion.wav")) {
        exit(-1);
    }
    
    explosion.setBuffer(explosionBuffer);
    explosion.setVolume(30.0f);
    
    // Load special weapon sound
    if (!specialBuffer.loadFromFile(resourcePath()+"sounds/special.wav")) {
        exit(-1);
    }
    
    special.setBuffer(specialBuffer);
    special.setVolume(30.0f);
    
    // Load player hit by enemy sound
    if (!playerHitBuffer.loadFromFile(resourcePath()+"sounds/playerhit.wav")) {
        exit(-1);
    }
    
    playerHit.setBuffer(playerHitBuffer);
    playerHit.setVolume(30.0f);
    
    if (!collectKeyBuffer.loadFromFile(resourcePath()+"sounds/collect_key.wav")) {
        exit(-1);
    }
    
    collectKey.setBuffer(collectKeyBuffer);
    collectKey.setVolume(30.0f);
    
    if (!doorOpenBuffer.loadFromFile(resourcePath()+"sounds/doorOpen.wav")) {
        exit(-1);
    }
    
    doorOpen.setBuffer(doorOpenBuffer);
    doorOpen.setVolume(30.0f);
    
    if (!doorLockedBuffer.loadFromFile(resourcePath()+"sounds/doorLocked.wav")) {
        exit(-1);
    }
    
    doorLocked.setBuffer(doorLockedBuffer);
    doorLocked.setVolume(30.0f);
    
    if (!portalBuffer.loadFromFile(resourcePath()+"sounds/RiftToGo.wav")) {
        exit(-1);
    }
    
    portal.setBuffer(portalBuffer);
    portal.setVolume(30.0f);
    
    portal.play();

    // game arrays
    weaponArray = {"Buster", "Plasma", "Run"};
    m_currentWeapon = "Buster";
    levelArray = {"level1.txt", "level2.txt", "level3.txt", "bosslevel.txt"};
    

}

void GameState_Play::loadLevel(const std::string & filename)
{
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();
    float xpos;
    float ypos;
    std::string animationName;
    std::string musicPath;
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();
    
    std::string token, name, AI;
    int TX, TY, RX, RY;
    float S;
    bool BM, BV;
    int health;
    
    
    // Read in assets
    std::ifstream inFile(filename);
    while (inFile.good())
    {
        std::string token;
        inFile>>token;
        
        // Tiles
        if (token == "Tile")
        {
            inFile >> animationName >> xpos >>  ypos;
            auto block = m_entityManager.addEntity("tile");
            block->addComponent<CTransform>(Vec2(xpos, ypos));
            block->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);
            // add a bounding box, this will now show up if we press the 'F' key
            block->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animationName).getSize(), false, false);
            
            if(block->getComponent<CAnimation>()->animation.getName() == "Brick")
            {
                block->addComponent<CState>("block");
            }
            
            if(block->getComponent<CAnimation>()->animation.getName() == "Question")
            {
                block->addComponent<CState>("question");
            }
            
            if(block->getComponent<CAnimation>()->animation.getName() == "GreenPlat")
            {
                block->getComponent<CTransform>()->speed.x = 2;
            }
        }
        
        // Decor
        if(token =="Dec")
        {
            inFile>> animationName >> xpos >> ypos;
            auto block = m_entityManager.addEntity("dec");
            block->addComponent<CTransform>(Vec2(xpos, ypos));
            block->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);
            
        }
        
        // Player
        if(token == "Player")
        {
            inFile>> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >>m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.MAXSPEED >> m_playerConfig.JUMP >>m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
        }
        
        if (token == "Music")
        {
            inFile >> musicPath;
            
            if (!musicBuffer.loadFromFile(resourcePath()+musicPath)) {
                exit(-1);
            }
            
            music.setBuffer(musicBuffer);
            music.setLoop(true);
            music.setVolume(25.0f);
            music.play();
            
        }
        
        if (token =="Jet")
        {
            inFile >> animationName >> xpos >> ypos;
            auto jp = m_entityManager.addEntity("jetpack");
            jp->addComponent<CTransform>(Vec2(xpos,ypos));
            jp->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animationName).getSize(), false, false);
            jp->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName),true);
            jp->getComponent<CTransform>()->scale = Vec2(3.0f,3.0f);
            jp->addComponent<CState>("inactive");
            jp->addComponent<CLifeSpan>(10000);
            
        }
        
        if (token == "Background")
        {
            inFile >> animationName >> xpos >>  ypos;
            
            auto m_menuBackground = m_entityManager.addEntity("background");
            m_menuBackground->addComponent<CTransform>(Vec2(xpos, ypos));
            m_menuBackground->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);
            
        }
        
        if (token == "Key")
        {
            inFile >> animationName >> xpos >>  ypos;
        
            auto m_key = m_entityManager.addEntity("key");
            m_key->addComponent<CTransform>(Vec2(xpos, ypos));
            m_key->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);
            m_key->addComponent<CBoundingBox>(Vec2(60, 40), false, false);
        }
        
        if (token == "NPC")
        {
            inFile >> animationName >> TX >> TY >> BM >> BV >> AI >> S >> health;
            
            auto m_npc = m_entityManager.addEntity("npc");
            m_npc->addComponent<CTransform>(Vec2(TX, TY));
            m_npc->addComponent<CAnimation>(m_game.getAssets().getAnimation(animationName), true);
            m_npc->addComponent<CBoundingBox>(m_game.getAssets().getAnimation(animationName).getSize(), BM, BV);
            m_npc->addComponent<CHealth>(health);
            
            if (AI == "Patrol")
            {
                int n;
                inFile >> n;
                std::vector<Vec2> pos;
                int x, y;
                for (int i = 0; i < n; i++)
                {
                    inFile >> x >> y;
                    pos.push_back(Vec2(x, y));
                    
                }
                m_npc->addComponent<CPatrol>(pos, S);
            }
            else
            {
                m_npc->addComponent<CFollowPlayer>(Vec2(TX, TY), S);
                
            }
            
        }
        
    }
}

void GameState_Play::spawnPlayer()
{
    // here is a sample player entity which you can use to construct other entities
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
    m_player->getComponent<CTransform>()->facing = Vec2(0, 1);
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), false, false);
    m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("SStand"), true);
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CInput>();
    m_player->addComponent<CState>("standing");
    m_player->addComponent<CHasItem>(false);
    m_player->addComponent<CHasKey>(false);
    m_player->addComponent<CHealth>(3);

        

}

void GameState_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
    
    if (m_currentWeapon == "Buster")
    {
        auto e = entity;
        auto bullet = m_entityManager.addEntity("bullet");
        bullet->addComponent<CTransform>(Vec2(e->getComponent<CTransform>()->pos.x, e->getComponent<CTransform>()->pos.y));
        bullet->addComponent<CBoundingBox>(m_game.getAssets().getAnimation("Buster").getSize(), false, false);
        bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("Buster"), true);
        bullet->addComponent<CState>("shooting");
        bullet->addComponent<CLifeSpan>(500);
        bullet->addComponent<CDamage>(1);

    }
    
    if (m_currentWeapon == "Plasma")
    {
        auto e = entity;
        auto bullet = m_entityManager.addEntity("bullet");
        bullet->addComponent<CTransform>(Vec2(e->getComponent<CTransform>()->pos.x, e->getComponent<CTransform>()->pos.y));
        bullet->addComponent<CBoundingBox>(Vec2(40, 40), false, false);
        bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("Plasma"), true);
        bullet->addComponent<CState>("shooting");
        bullet->addComponent<CLifeSpan>(500);
        bullet->addComponent<CDamage>(3);

    }
    
    if (m_currentWeapon == "Run")
    {
        auto e = entity;
        auto bullet = m_entityManager.addEntity("bullet");
        bullet->addComponent<CTransform>(Vec2(e->getComponent<CTransform>()->pos.x, e->getComponent<CTransform>()->pos.y));
        bullet->addComponent<CBoundingBox>(m_game.getAssets().getAnimation("Run").getSize(), false, false);
        bullet->addComponent<CAnimation>(m_game.getAssets().getAnimation("Run"), true);
        bullet->addComponent<CState>("shooting");
        bullet->addComponent<CLifeSpan>(500);
        bullet->addComponent<CDamage>(2);

    }
    
    
}

void GameState_Play::update()
{

    if (!m_paused)
    {
        sAI();
        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
        Jetpack();
        m_entityManager.update();
       
    }
    sHealth();

    sRender();
    sUserInput();
}


void GameState_Play::sMovement()

{
    m_player->getComponent<CTransform>()->speed.x = 0;
    
    if (m_player->getComponent<CInput>()->up)
    {
        if (m_player->getComponent<CState>()->state == "standing")
        {
            m_player->getComponent<CTransform>()->speed.y += m_playerConfig.JUMP;
        }
        
        if (m_player->getComponent<CState>()->state == "running")
        {
            m_player->getComponent<CTransform>()->speed.y += m_playerConfig.JUMP;
        }
        
        if (m_player->getComponent<CHasItem>()->has == true)
        {
            m_player->getComponent<CTransform>()->speed.y = m_playerConfig.SPEED * 3.5f;
        }
    }
    
    if (m_player->getComponent<CInput>()->right)
    {
        
        m_player->getComponent<CTransform>()->scale.x = 1;
        m_player->getComponent<CTransform>()->speed.x += m_playerConfig.SPEED;
        m_player->getComponent<CTransform>()->facing = Vec2(0, 1);
    }
    
    
    if (m_player->getComponent<CInput>()->left)
    {
        m_player->getComponent<CTransform>()->scale.x = -1;
        m_player->getComponent<CTransform>()->facing = Vec2(1, 0);
        if (m_player->getComponent<CTransform>()->pos.x > m_playerConfig.CX / 2)
        {
            m_player->getComponent<CTransform>()->speed.x -= m_playerConfig.SPEED;
        }
    }
    
    m_player->getComponent<CTransform>()->speed.y += m_playerConfig.GRAVITY;
    
    if (m_player->getComponent<CTransform>()->speed.y > m_playerConfig.MAXSPEED) { m_player->getComponent<CTransform>()->speed.y = m_playerConfig.MAXSPEED; }
    if (m_player->getComponent<CTransform>()->speed.y < -(m_playerConfig.MAXSPEED)) { m_player->getComponent<CTransform>()->speed.y = -(m_playerConfig.MAXSPEED); }
    if (m_player->getComponent<CTransform>()->speed.x > m_playerConfig.MAXSPEED) { m_player->getComponent<CTransform>()->speed.x = m_playerConfig.MAXSPEED; }
    if (m_player->getComponent<CTransform>()->speed.x < -(m_playerConfig.MAXSPEED)) { m_player->getComponent<CTransform>()->speed.x = -(m_playerConfig.MAXSPEED); }
    
    for (auto e : m_entityManager.getEntities())
    {
        e->getComponent<CTransform>()->pos.y += e->getComponent<CTransform>()->speed.y;
        e->getComponent<CTransform>()->pos.x += e->getComponent<CTransform>()->speed.x;
    }
    
    
    if (m_player->getComponent<CInput>()->shoot && m_player->getComponent<CInput>()->canShoot)
    {
        spawnBullet(m_player);
        laser.play();
        m_player->getComponent<CInput>()->canShoot = false;
    }
    
    
    if (m_player->getComponent<CTransform>()->pos.y <= 0)
    {
        m_player->destroy();
        spawnPlayer();
    }
    
    for (auto e : m_entityManager.getEntities("npc"))
    {
        e->getComponent<CTransform>()->pos += e->getComponent<CTransform>()->speed;
    }
    
    
    for (auto & e : m_entityManager.getEntities("bullet"))
    {
    
        if (m_player->getComponent<CInput>()->shoot == true)
        {
            if (m_currentWeapon == "Buster")
            {
                if (m_player->getComponent<CTransform>()->facing == Vec2(0, 1))
                {
                    e->getComponent<CTransform>()->speed.x = m_playerConfig.SPEED * 2.0f;
                }
                if (m_player->getComponent<CTransform>()->facing == Vec2(1, 0))
                {
                    e->getComponent<CTransform>()->speed.x = -m_playerConfig.SPEED * 2.0f;
                }
            }
            
            if (m_currentWeapon == "Plasma")
            {
                if (m_player->getComponent<CTransform>()->facing == Vec2(0, 1))
                {
                    e->getComponent<CTransform>()->speed.x = m_playerConfig.SPEED * 1.2f;
                }
                if (m_player->getComponent<CTransform>()->facing == Vec2(1, 0))
                {
                    e->getComponent<CTransform>()->speed.x = -m_playerConfig.SPEED * 1.2f;
                }
            }
            
            if (m_currentWeapon == "Run")
            {
                if (m_player->getComponent<CTransform>()->facing == Vec2(0, 1))
                {
                    e->getComponent<CTransform>()->speed.x = m_playerConfig.SPEED * 1.2f;
                }
                if (m_player->getComponent<CTransform>()->facing == Vec2(1, 0))
                {
                    e->getComponent<CTransform>()->speed.x = -m_playerConfig.SPEED * 1.2f;
                }
            }
            
        }

        e->getComponent<CTransform>()->pos.x += e->getComponent<CTransform>()->speed.x;

    }
    
    for (auto & e : m_entityManager.getEntities("shield"))
    {
        e->getComponent<CTransform>()->pos = m_player->getComponent<CTransform>()->pos;
        
        
        
        
    }
    
    
    for (auto & e : m_entityManager.getEntities("tile"))
    {
        if (e->getComponent<CAnimation>()->animation.getName() == "GreenPlat")
        {
            e->getComponent<CTransform>()->pos += e->getComponent<CTransform>()->speed;
            
            int level1PlatX1 = 1632;
            int level1PlatX2 = 1952;
            int level2PlatX1 = 2784;
            int level2PlatX2 = 3616;
            
            if (m_levelPath == "level1.txt")
            {
                if (e->getComponent<CTransform>()->pos.x <= level1PlatX1)
                {
                    e->getComponent<CTransform>()->speed.x *= -1;
                }
                
                if (e->getComponent<CTransform>()->pos.x >= level1PlatX2)
                {
                    e->getComponent<CTransform>()->speed.x *= -1;
                }
            }
            
            if (m_levelPath == "level3.txt")
            {
                if (e->getComponent<CTransform>()->pos.x <= level2PlatX1)
                {
                    e->getComponent<CTransform>()->speed.x *= -1;
                }
                
                if (e->getComponent<CTransform>()->pos.x >= level2PlatX2)
                {
                    e->getComponent<CTransform>()->speed.x *= -1;
                }
            }
            
            
        }
    }
    
    for(auto & e : m_entityManager.getEntities("life"))
    {
        if (m_player->getComponent<CTransform>()->facing == Vec2(0, 1))
        {
            e->getComponent<CTransform>()->pos.x = m_player->getComponent<CTransform>()->pos.x;
        }
        if (m_player->getComponent<CTransform>()->facing == Vec2(1, 0))
        {
             e->getComponent<CTransform>()->pos.x = m_player->getComponent<CTransform>()->pos.x * -1;
        }
    }
}



void GameState_Play::sLifespan()
{
    for (auto & e : m_entityManager.getEntities("jetpack")) {
        if(e->getComponent<CState>()->state == "active")
        {
            auto clock = e->getComponent<CLifeSpan>()->clock;
            auto lifespan = e->getComponent<CLifeSpan>()->lifespan;
            sf::Time life = clock.getElapsedTime();
            int time = life.asMilliseconds();
            if(time > lifespan)
            {
                m_player->getComponent<CHasItem>()->has = false;
                e->destroy();
                
            }
            
        }
    }
    
    
    for (auto & e : m_entityManager.getEntities("bullet")) {
        
        auto clock = e->getComponent<CLifeSpan>()->clock;
        auto lifespan = e->getComponent<CLifeSpan>()->lifespan;
        sf::Time life = clock.getElapsedTime();
        int time = life.asMilliseconds();
        if(time > lifespan)
        {
            e->destroy();
            
        }
    }
    
    for (auto & e : m_entityManager.getEntities("jetpackfire")) {
        
        auto clock = e->getComponent<CLifeSpan>()->clock;
        auto lifespan = e->getComponent<CLifeSpan>()->lifespan;
        sf::Time life = clock.getElapsedTime();
        int time = life.asMilliseconds();
        if(time > lifespan)
        {
            e->destroy();
            
        }
    }
    
    for (auto & e : m_entityManager.getEntities("shield")) {
        
        auto clock = e->getComponent<CLifeSpan>()->clock;
        auto lifespan = e->getComponent<CLifeSpan>()->lifespan;
        sf::Time life = clock.getElapsedTime();
        int time = life.asMilliseconds();
        if(time > lifespan)
        {
            e->destroy();
            m_shieldOn = false;
            
        }
    }
    
    for (auto & e : m_entityManager.getEntities("Fire")) {
        
        auto clock = e->getComponent<CLifeSpan>()->clock;
        auto lifespan = e->getComponent<CLifeSpan>()->lifespan;
        sf::Time life = clock.getElapsedTime();
        int time = life.asMilliseconds();
        if(time > lifespan)
        {
            e->destroy();
            
        }
        
        
    }
    
    
}

void GameState_Play::sAI()
{
    
    for (auto e : m_entityManager.getEntities("npc"))
    {
        auto eTransform = e->getComponent<CTransform>();
        Vec2 dest(0, 0);
        float speed = 0;
        eTransform->speed = Vec2(0, 0);
        if (e->hasComponent<CPatrol>())
        {
            auto ePatrol = e->getComponent<CPatrol>();
            speed = ePatrol->speed;
            
            dest = ePatrol->positions[ePatrol->currentPosition];
            
            if (eTransform->pos.dist(dest) <= 5)
            {
                ePatrol->currentPosition += 1;
                if (ePatrol->positions.size() <= ePatrol->currentPosition)
                {
                    ePatrol->currentPosition = 0;
                }
                dest = ePatrol->positions[ePatrol->currentPosition];
            }
        }
        else
        {
            bool cansee = true;
            speed = e->getComponent<CFollowPlayer>()->speed;
            for (auto t : m_entityManager.getEntities())
            {
                if (t->hasComponent<CBoundingBox>())
                {
                    if (t->getComponent<CBoundingBox>()->blockVision && t != e && t != m_player)
                    {
                        if (Physics::EntityIntersect(m_player->getComponent<CTransform>()->pos, eTransform->pos, t))
                        {
                            cansee = false;
                            break;
                        }
                    }
                }
            }
            
            if (cansee) { dest = m_player->getComponent<CTransform>()->pos; }
            else { dest = e->getComponent<CFollowPlayer>()->home; }
        }
        if (eTransform->pos.dist(dest) <= 5)
        {
            eTransform->speed = Vec2(0, 0);
        }
        else
        {
            float dx = dest.x - eTransform->pos.x;
            float dy = dest.y - eTransform->pos.y;
            float ang = atan2(dy, dx);
            float vx = speed*cos(ang);
            float vy = speed*sin(ang);
            eTransform->speed = Vec2(vx, vy);
        }
        
    }
}


void GameState_Play::sCollision()
{
    m_player->getComponent<CState>()->state = "air";
    auto pTransform = m_player->getComponent<CTransform>();
    for (auto e : m_entityManager.getEntities("tile"))
    {
        Vec2 overlap = Physics::GetOverlap(e, m_player);
        if (overlap.x > 0 && overlap.y > 0)
        {
            Vec2 prevPos = pTransform->pos - pTransform->speed;
            Vec2 prevOv = Physics::GetOverlap(e, m_player, prevPos);
            if (prevOv.y > 0)
            {
                pTransform->pos.x -= (overlap.x * pTransform->scale.x);
            }
            else
            {
                if (pTransform->pos.y > e->getComponent<CTransform>()->pos.y)
                {
                    pTransform->pos.y += overlap.y;
                    
                    m_player->getComponent<CState>()->state = "standing";
                }
                
                
                else
                {
                    pTransform->pos.y -= overlap.y;
                    
                    
                    if (e->getComponent<CAnimation>()->animation.getName() == "Brick")
                    {
                        auto tempDec = m_entityManager.addEntity("dec");
                        Vec2 pos(e->getComponent<CTransform>()->pos);
                        e->destroy();
                        tempDec->addComponent<CTransform>(pos);
                        tempDec->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
                    }
                    
                }
                pTransform->speed.y = 0;
            }
            
            if (e->getComponent<CAnimation>()->animation.getName() == "Fire")
            {
            
                m_player->getComponent<CTransform>()->pos = Vec2(m_playerConfig.X, m_playerConfig.Y);
                
                if(!m_shieldOn)
                {
                    m_player->getComponent<CHealth>()->health -= 1;
                    std::cout<<m_player->getComponent<CHealth>()->health << "="<<m_player->getComponent<CHealth>()->health<< "-1"<<"\n";
                    m_player->getComponent<CTransform>()->pos = Vec2(m_playerConfig.X, m_playerConfig.Y);
                }
                
                explosion.play();
            }
            
            
            if (e->getComponent<CAnimation>()->animation.getName() == "Door")
            {
                m_doorCollision = true;
            }

            if (e->getComponent<CAnimation>()->animation.getName() != "Door")
            {
                m_doorCollision = false;
            }
            
            if (e->getComponent<CAnimation>()->animation.getName() == "GreenPlat")
            {
                m_player->getComponent<CTransform>()->pos += e->getComponent<CTransform>()->speed;
            }
            
            if (e->getComponent<CAnimation>()->animation.getName() == "Shield")
            {
                spawnShield();
                e->destroy();
                std::cout << "shield on!";
            }
            
            if (e->getComponent<CAnimation>()->animation.getName() == "Portal")
            {
                levelIndex += 1;
                music.stop();
                if (levelIndex == 0)
                {
                    init(levelArray[0]);
                }
                if (levelIndex == 1)
                {
                    init(levelArray[1]);
                }
                if (levelIndex == 2)
                {
                    init(levelArray[2]);
                }
                if (levelIndex == 3)
                {
                    init(levelArray[3]);
                }
                
            }
            
            if ((m_player->getComponent<CState>()->state == "standing") && (m_player->getComponent<CInput>()->right == true || m_player->getComponent<CInput>()->left == true))
            {
                m_player->getComponent<CState>()->state = "running";
    
            }
            
            if ((m_player->getComponent<CState>()->state == "running") && (m_player->getComponent<CInput>()->right == false && m_player->getComponent<CInput>()->left == false && m_player->getComponent<CInput>()->up == false))
            {
                m_player->getComponent<CState>()->state = "standing";
    
            }
            
        }
        
        for (auto & e : m_entityManager.getEntities("key"))
        {
            auto overlap = Physics::GetOverlap(e, m_player);
            
            if (overlap.x > 0 && overlap.y > 0)
            {
                collectKey.play();
                m_player->getComponent<CHasKey>()->hasKey = true;
                std::cout << "Player has key!";
                e->destroy();
            }
        }
        
        
        
        for (auto & e : m_entityManager.getEntities("npc"))
        {
            auto overlap = Physics::GetOverlap(e, m_player);
            
            if (overlap.x > 0 && overlap.y > 0)
            {
                
                if (!m_shieldOn)
                {
                    m_player->getComponent<CTransform>()->pos = Vec2(m_playerConfig.X, m_playerConfig.Y);
                    m_player->getComponent<CHealth>()->health -= 1;
                    std::cout<<m_player->getComponent<CHealth>()->health << "="<<m_player->getComponent<CHealth>()->health<< "-1"<<"\n";
                }
                
                m_player->getComponent<CTransform>()->pos = Vec2(m_playerConfig.X, m_playerConfig.Y);
                
                playerHit.play();
            }
        }
        
    }
    
    
    for (auto bullet : m_entityManager.getEntities("bullet"))
    {
        for (auto e : m_entityManager.getEntities("tile"))
        {
            Vec2 overlap = Physics::GetOverlap(e, bullet);
            if (overlap.x > 0 && overlap.y > 0)
            {
                bullet->destroy();
                if (e->getComponent<CAnimation>()->animation.getName() == "Brick")
                {
                    auto tempDec = m_entityManager.addEntity("Dec");
                    Vec2 pos(e->getComponent<CTransform>()->pos);
                    e->destroy();
                    tempDec->addComponent<CTransform>(pos);
                    tempDec->addComponent<CAnimation>(m_game.getAssets().getAnimation("Explosion"), false);
                }
            }
        }
        
        for (auto & e : m_entityManager.getEntities("npc"))
        {
            auto overlap = Physics::GetOverlap(e, bullet);
            
            if (overlap.x > 0 && overlap.y > 0)
            {
                if (bullet->getComponent<CAnimation>()->animation.getName() == "Buster")
                {
                    e->getComponent<CHealth>()->health -= 1;
                    bullet->destroy();
                }
                if (bullet->getComponent<CAnimation>()->animation.getName() == "Plasma")
                {
                    e->getComponent<CHealth>()->health -= 2;
                    bullet->destroy();
                }
                if (bullet->getComponent<CAnimation>()->animation.getName() == "Run")
                {
                    e->getComponent<CHealth>()->health -= 3;
                    bullet->destroy();
                }
                
                if (e->getComponent<CHealth>()->health <= 0)
                {
                    if (e->getComponent<CAnimation>()->animation.getName() == "MotherBrain")
                    {
                        e->destroy();
                        m_game.popState();
                        music.stop();
                    }
                    e->destroy();
                    
                }
                special.play();
            }
        }
    }
    
    for (auto & e : m_entityManager.getEntities("jetpack"))
    {
        auto overlap = Physics::GetOverlap(e, m_player);
        if (overlap.x > 0 && overlap.y > 0)
        {
            m_player->getComponent<CHasItem>()->has = true;
            e->getComponent<CState>()->state = "active";
        }
    }
}

void GameState_Play::sHealth()
{
    auto h = m_entityManager.addEntity("life");
    h->addComponent<CTransform>(Vec2(75, 700 ));
    h->addComponent<CAnimation>(m_game.getAssets().getAnimation("Life3"),true);

    
    if (m_player->getComponent<CHealth>()->health <= 0)
    {
        m_game.popState();
        music.stop();
    }
    
    h->getComponent<CAnimation>()->animation.update();
    
    
}

void GameState_Play::spawnShield()
{

    auto shield = m_entityManager.addEntity("shield");
    shield->addComponent<CTransform>(Vec2(m_player->getComponent<CTransform>()->pos.x, m_player->getComponent<CTransform>()->pos.y));
    shield->addComponent<CBoundingBox>(Vec2(40, 40), false, false);
    shield->addComponent<CAnimation>(m_game.getAssets().getAnimation("ShieldOn"), true);
    shield->addComponent<CLifeSpan>(5000);
    m_shieldOn = true;
        
}

void GameState_Play::Jetpack()
{
    for (auto & e : m_entityManager.getEntities("jetpack"))
    {
        if(e->getComponent<CState>()->state == "active")
        {
            if (m_player->getComponent<CInput>()->right)
            {
                e->getComponent<CTransform>()->pos.x = m_player->getComponent<CTransform>()->pos.x-20;
                e->getComponent<CTransform>()->pos.y = m_player->getComponent<CTransform>()->pos.y+30;
            }
            if (m_player->getComponent<CInput>()->left)
            {
                e->getComponent<CTransform>()->pos.x = m_player->getComponent<CTransform>()->pos.x+20;
                e->getComponent<CTransform>()->pos.y = m_player->getComponent<CTransform>()->pos.y+30;
            }
            if (m_player->getComponent<CInput>()->up)
            {
                e->getComponent<CTransform>()->pos.x = m_player->getComponent<CTransform>()->pos.x-20;
                e->getComponent<CTransform>()->pos.y = m_player->getComponent<CTransform>()->pos.y+30;
                auto f = m_entityManager.addEntity("jetpackfire");
                f->addComponent<CTransform>(Vec2( e->getComponent<CTransform>()->pos.x, e->getComponent<CTransform>()->pos.y - 32));
                f->addComponent<CAnimation>(m_game.getAssets().getAnimation("JPF"),true);
                f->getComponent<CTransform>()->scale = Vec2(2.0f,2.0f);
                f->addComponent<CLifeSpan>(50);
                
                
                
            }
            
        }
    }
}

// Scroll through animations
void GameState_Play::sWeaponCycle(int n)
{
    m_currentWeapon = weaponArray[n];
    std::cout << m_currentWeapon;
}


void GameState_Play::sUserInput()
{
    
    sf::Event event;
    while (m_game.window().pollEvent(event))
    {
        CInput c;
        if (event.type == sf::Event::Closed)
        {
            m_game.quit();
        }
        
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:  { m_game.popState(); music.stop(); break; }
                case sf::Keyboard::Z:       { init(m_levelPath); break; }
                case sf::Keyboard::R:       { m_drawTextures = !m_drawTextures; break; }
                case sf::Keyboard::F:       { m_drawCollision = !m_drawCollision; break; }
                case sf::Keyboard::P:       { setPaused(!m_paused); break; }
                case sf::Keyboard::W:
                {
                    m_player->getComponent<CInput>()->up = true;
                    break;
                }
                case sf::Keyboard::A:
                {
                    m_player->getComponent<CInput>()->left = true;
                    break;
                }
                case sf::Keyboard::S:
                {
                    
                    m_player->getComponent<CInput>()->down = true;
                    break;
                }
                    
                case sf::Keyboard::D:
                {
                    m_player->getComponent<CInput>()->right = true;
                    break;
                }
                case sf::Keyboard::Space:
                {
                    m_player->getComponent<CInput>()->shoot = true;

                    
                    break;
                }
                case sf::Keyboard::O:
                {
                    // try to open door
                    m_openingDoor = true;
                    break;
                }
                case sf::Keyboard::RShift:
                {
                    weaponIndex += 1;
                    
                    if (weaponIndex > weaponArray.size() - 1)
                    {
                        weaponIndex = 0;
                    }
                    
                    std::cout << weaponIndex;
                    sWeaponCycle(weaponIndex);
                    break;
                }
                    
                    
                    
            }
        }
        
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape: { break; }
                    
                case sf::Keyboard::W:
                    m_player->getComponent<CInput>()->up = false;
                    m_player->getComponent<CTransform>()->speed.y = 0;
                    break;
                    
                case sf::Keyboard::A:
                    m_player->getComponent<CInput>()->left = false;
                    m_player->getComponent<CTransform>()->speed.x = 0;
                    break;
                    
                case sf::Keyboard::S:
                    m_player->getComponent<CInput>()->down = false;
                    m_player->getComponent<CTransform>()->speed.y = 0;
                    break;
                    
                case sf::Keyboard::D:
                    m_player->getComponent<CInput>()->right = false;
                    m_player->getComponent<CTransform>()->speed.x = 0;
                    break;
                    
                case sf::Keyboard::Space:
                    
                    
                    m_player->getComponent<CInput>()->shoot = false;
                    m_player->getComponent<CInput>()->canShoot = true;
                    break;
                    
            }
        }
    }
    
}
void GameState_Play::sAnimation()
{

    // tile animations
    for(auto & e: m_entityManager.getEntities("tile"))
    {
        // Portal Animation
        if (e->getComponent<CAnimation>()->animation.getName() == "Portal")
        {
            e->getComponent<CAnimation>()->animation.update();
        }
        
        // Fire animation
        if (e->getComponent<CAnimation>()->animation.getName() == "Fire")
        {
            e->getComponent<CAnimation>()->animation.update();
        }
        
        if (e->getComponent<CAnimation>()->animation.getName() == "Shield")
        {
            e->getComponent<CAnimation>()->animation.update();
        }
        
        if (e->getComponent<CAnimation>()->animation.getName() == "Invincibility")
        {
            e->getComponent<CAnimation>()->animation.update();
        }
        
        
        // Door animation
        if (e->getComponent<CAnimation>()->animation.getName() == "Door")
        {
            e->getComponent<CAnimation>()->repeat = false;
            
            if (m_player->getComponent<CHasKey>()->hasKey == true && m_openingDoor == true && m_doorCollision == true)
            {
                
                e->getComponent<CAnimation>()->animation.update();
                
                doorOpen.play();
                
                if (e->getComponent<CAnimation>()->animation.hasEnded() == true)
                {
                    e->destroy();
                }
                
                e->getComponent<CAnimation>()->animation.update();
            }
            
        }
        
    }
    
    for(auto & e: m_entityManager.getEntities("shield"))
    {
        e->getComponent<CAnimation>()->animation.update();
    }
    
    // bullet animation
    for(auto & e: m_entityManager.getEntities("bullet"))
    {
        e->getComponent<CAnimation>()->animation.update();
    }
    
    // player animations
    if (m_player->getComponent<CState>()->state == "running" && m_player->getComponent<CAnimation>()->animation.getName() == "SStand")
    {
        m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("SRun"), true);
    }
    
    if (m_player->getComponent<CState>()->state == "standing" && m_player->getComponent<CAnimation>()->animation.getName() == "SRun")
    {
        m_player->addComponent<CAnimation>(m_game.getAssets().getAnimation("SStand"), true);
    }
    
    
    // health animations
    for(auto & e: m_entityManager.getEntities("life"))
    {
        if(m_player->getComponent<CHealth>()->health == 2)
        {
            e->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Life2");
        }
        if(m_player->getComponent<CHealth>()->health == 3)
        {
            e->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Life3");
        }
        if(m_player->getComponent<CHealth>()->health == 1)
        {
            e->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("Life1");
        }
        e->getComponent<CAnimation>()->animation.update();
        
    }
    
    for (auto & e : m_entityManager.getEntities("jetpack"))
    {
        if (e->getComponent<CState>()->state == "active")
        {
            e->getComponent<CAnimation>()->animation = m_game.getAssets().getAnimation("JPU");
        }
        e->getComponent<CAnimation>()->animation.update();
    }
    
    for (auto & e : m_entityManager.getEntities("jetpackfire"))
    {
        e->getComponent<CAnimation>()->animation.update();
    }
    

    
    // npc animation
    for (auto e : m_entityManager.getEntities("npc"))
    {
        
        
        if (e->getComponent<CAnimation>()->animation.getName() == "KraidBrown")
        {
            e->getComponent<CTransform>()->scale = Vec2(-1.0f, 1.0f);
        }
        
        e->getComponent<CAnimation>()->animation.update();
        
    }
    
    m_player->getComponent<CAnimation>()->animation.update();
}

// this function has been completed for you
void GameState_Play::sRender()
{
    // color the background darker so you know that the game is paused
    if (!m_paused)  { m_game.window().clear(sf::Color(0, 0, 0)); }
    else            { m_game.window().clear(sf::Color(1, 0, 0)); }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto pPos = m_player->getComponent<CTransform>()->pos;
    float windowCenterX = fmax(m_game.window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game.window().getView();
    view.setCenter(windowCenterX, m_game.window().getSize().y - view.getCenter().y);
    m_game.window().setView(view);
    
        
    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto e : m_entityManager.getEntities("background"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(windowCenterX, m_game.window().getSize().y - view.getCenter().y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("tile"))
        {
            auto transform = e->getComponent<CTransform>();

            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("dec"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("player"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("jetpack"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("shield"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("bullet"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("life"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("key"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto e : m_entityManager.getEntities("npc"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        for (auto & e : m_entityManager.getEntities("jetpackfire"))
        {
            auto transform = e->getComponent<CTransform>();
            
            if (e->hasComponent<CAnimation>())
            {
                auto animation = e->getComponent<CAnimation>()->animation;
                animation.getSprite().setRotation(transform->angle);
                animation.getSprite().setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                animation.getSprite().setScale(transform->scale.x, transform->scale.y);
                m_game.window().draw(animation.getSprite());
            }
        }
        
        
        
    }

    // draw all Entity collision bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        sf::CircleShape dot(4);
        dot.setFillColor(sf::Color::Black);
        for (auto e : m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto box = e->getComponent<CBoundingBox>();
                auto transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box->size.x-1, box->size.y-1));
                rect.setOrigin(sf::Vector2f(box->halfSize.x, box->halfSize.y));
                rect.setPosition(transform->pos.x, m_game.window().getSize().y - transform->pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                
                if (box->blockMove && box->blockVision)  { rect.setOutlineColor(sf::Color::Black); }
                if (box->blockMove && !box->blockVision) { rect.setOutlineColor(sf::Color::Blue); }
                if (!box->blockMove && box->blockVision) { rect.setOutlineColor(sf::Color::Red); }
                if (!box->blockMove && !box->blockVision) { rect.setOutlineColor(sf::Color::White); }
                
                rect.setOutlineThickness(1);
                m_game.window().draw(rect);
            }
            
            if (e->hasComponent<CPatrol>())
            {
                auto & patrol = e->getComponent<CPatrol>()->positions;
                for (size_t p = 0; p < patrol.size(); p++)
                {
                    dot.setPosition(patrol[p].x, patrol[p].y);
                    m_game.window().draw(dot);
                }
            }
            
            if (e->hasComponent<CFollowPlayer>())
            {
                sf::VertexArray lines(sf::LinesStrip, 2);
                lines[0].position.x = e->getComponent<CTransform>()->pos.x;
                lines[0].position.y = e->getComponent<CTransform>()->pos.y;
                lines[0].color = sf::Color::Black;
                lines[1].position.x = m_player->getComponent<CTransform>()->pos.x;
                lines[1].position.y = m_player->getComponent<CTransform>()->pos.y;
                lines[1].color = sf::Color::Black;
                m_game.window().draw(lines);
                dot.setPosition(e->getComponent<CFollowPlayer>()->home.x, e->getComponent<CFollowPlayer>()->home.y);
                m_game.window().draw(dot);
            }
        }
    }
    
    
    

    m_game.window().display();
}
