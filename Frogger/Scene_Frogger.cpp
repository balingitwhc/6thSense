#include <fstream>
#include <iostream>

#include "Scene_Frogger.h"
#include "Components.h"
#include "Physics.h"
#include "Utilities.h"
#include "MusicPlayer.h"
#include "Assets.h"
#include "SoundPlayer.h"
#include <random>
#include <numbers>

#pragma region [Load]

//Random Number Generator (Seeder)
namespace {
    std::random_device rd;
    std::mt19937 rng(rd());
}

std::uniform_real_distribution<double> vehicleSpeed(50.f, 100.f);
std::uniform_real_distribution<double> vehicleDistance(140.f, 160.f);
std::uniform_int_distribution<int> floor23Enemy(1, 2);
std::uniform_int_distribution<int> floor45Enemy(1, 3);
std::uniform_int_distribution<int> floor67Enemy(1, 4);

// Play Screen
Scene_Frogger::Scene_Frogger(GameEngine* gameEngine, const std::string& levelPath)
    : Scene(gameEngine)
    , m_worldView(gameEngine->window().getDefaultView()) {
    loadLevel(levelPath);   // Load Stage
    registerActions();  // Detect Keyboard controls/triggers
    init();

    MusicPlayer::getInstance().play("gameTheme");
    MusicPlayer::getInstance().setVolume(100);
}

// Get the parameters for a rectangular boundaary
sf::FloatRect Scene_Frogger::getViewBounds() {
    return sf::FloatRect();
}

// Initializer ---- const std::string& path???
void Scene_Frogger::init() {
    spawnPlayer(m_worldView.getCenter());

    std::uniform_real_distribution<double> range(1.f, 5.f);
    double randNum = range(rng);

    m_clock_text.setFont(Assets::getInstance().getFont("main"));
    m_clock_text.setPosition(10.f, 75.f);
    m_clock_text.setCharacterSize(25);

    m_score_text.setFont(Assets::getInstance().getFont("main"));
    m_score_text.setPosition(10.f, 105.f);
    m_score_text.setCharacterSize(25);

    m_lives_text.setFont(Assets::getInstance().getFont("main"));
    m_lives_text.setPosition(10.f, 135.f);
    m_lives_text.setCharacterSize(25);

    m_ammo_text.setFont(Assets::getInstance().getFont("main"));
    m_ammo_text.setPosition(10.f, 165.f);
    m_ammo_text.setCharacterSize(25);

    m_kill_text.setFont(Assets::getInstance().getFont("main"));
    m_kill_text.setPosition(10.f, 195.f);
    m_kill_text.setCharacterSize(25);

    spawnEnemies(10, 310.f, 2000.f);
}

// Set Player Components/Properties
void Scene_Frogger::spawnPlayer(sf::Vector2f pos) {
    
    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CTransform>(pos);
    m_player->addComponent<CBoundingBox>(sf::Vector2f(32.f, 32.f));
    m_player->addComponent<CInput>();
    m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("up"));

    //m_player->addComponent<CState>().state = "grounded";
}

//====================================================================================================


void Scene_Frogger::manageMobs()
{
    for (auto e : m_entityManager.getEntities("car")) {
        auto& tfm = e->getComponent<CTransform>();
        if (tfm.pos.x < -60.f and tfm.vel.x < 0) {
            tfm.pos.x += 540;
        }

        if (tfm.pos.x > 540.f and tfm.vel.x > 0) {
            tfm.pos.x -= 540;
        }
    }

    for (auto e : m_entityManager.getEntities("tree1")) {
        auto& tfm = e->getComponent<CTransform>();
        if (tfm.pos.x < -60.f and tfm.vel.x < 0) {
            tfm.pos.x += 540;
        }

        if (tfm.pos.x > 540.f and tfm.vel.x > 0) {
            tfm.pos.x -= 540;
        }
    }

    for (auto e : m_entityManager.getEntities("tree2")) {
        auto& tfm = e->getComponent<CTransform>();
        if (tfm.pos.x < -260.f and tfm.vel.x < 0) {
            tfm.pos.x += 940;
        }

        if (tfm.pos.x > 540.f and tfm.vel.x > 0) {
            tfm.pos.x -= 540;
        }
    }

    for (auto e : m_entityManager.getEntities("3turtles")) {
        auto& tfm = e->getComponent<CTransform>();
        if (tfm.pos.x < -60.f and tfm.vel.x < 0) {
            tfm.pos.x += 540;
        }

        if (tfm.pos.x > 540.f and tfm.vel.x > 0) {
            tfm.pos.x -= 540;
        }
    }

    for (auto e : m_entityManager.getEntities("2turtles")) {
        auto& tfm = e->getComponent<CTransform>();
        if (tfm.pos.x < -60.f and tfm.vel.x < 0) {
            tfm.pos.x += 540;
        }

        if (tfm.pos.x > 540.f and tfm.vel.x > 0) {
            tfm.pos.x -= 540;
        }
    }
}

void Scene_Frogger::spawnBullet(sf::Vector2f mPos)
{
    if (m_ammo <= 0) {
        return;
    }

    // Define the interval between shots
    const sf::Time intervalBetweenShots = sf::seconds(shotDecay);

    // Check if enough time has passed since the last shot
    if (shotClock.getElapsedTime() < intervalBetweenShots)
        return; // Not enough time has passed, so don't shoot

    // Restart the clock to measure the time interval for the next shot
    shotClock.restart();

    if (m_player == nullptr)
        return; // No player entity to spawn a bullet from

    if (bulletSpread == true) {
        auto e = m_entityManager.addEntity("bullet");

        sf::Vector2f clickPoint(mPos);
        auto pPos = m_player->getComponent<CTransform>().pos;
        sf::Vector2f direction = clickPoint - pPos;

        float bulletSpeed = 1000.0f; // Speed of the bullet
        direction = normalize(direction); // Ensure the direction vector is normalized
        startingAngle = atan2(direction.y, direction.x);

        float angleIncrement = 15 * (3.14159265 / 180.0);

        for (size_t i = 0; i < spreadCount; ++i)
        {
            shotAngles[i] = startingAngle - angleIncrement * 2 + angleIncrement * i;
            SoundPlayer::getInstance().play("laserGun", m_player->getComponent<CTransform>().pos);
        }

        for (size_t i = 0; i < spreadCount; ++i)
        {
            sf::Vector2f dir(cos(shotAngles[i]), sin(shotAngles[i]));
            sf::Vector2f bulletVelocity = dir * bulletSpeed;

            auto e = m_entityManager.addEntity("bullet");

            e->addComponent<CTransform>(pPos, bulletVelocity);
            e->addComponent<CBoundingBox>(sf::Vector2f(20.f, 20.f));
            e->addComponent<CAnimation>(Assets::getInstance().getAnimation("bugIcon"));
            e->addComponent<CLifespan>(3.f);
            SoundPlayer::getInstance().play("laserGun", m_player->getComponent<CTransform>().pos);

        }
        if (!spectralEcho) { m_ammo -= 1; }
        
    }
    else {
        auto e = m_entityManager.addEntity("bullet");

        sf::Vector2f clickPoint(mPos);
        auto pPos = m_player->getComponent<CTransform>().pos;
        sf::Vector2f direction = clickPoint - pPos;

        float bulletSpeed = 1000.0f; // Speed of the bullet
        direction = normalize(direction); // Ensure the direction vector is normalized
        sf::Vector2f bulletVelocity = direction * bulletSpeed;

        e->addComponent<CTransform>(pPos, bulletVelocity);
        e->addComponent<CBoundingBox>(sf::Vector2f(20.f, 20.f));
        e->addComponent<CAnimation>(Assets::getInstance().getAnimation("bugIcon"));
        e->addComponent<CLifespan>(3.f);

        std::cout << "bullet shot at " << direction << "\n";
        SoundPlayer::getInstance().play("laserGun", m_player->getComponent<CTransform>().pos);
        if (!spectralEcho) { m_ammo -= 1; }
    }
}

void Scene_Frogger::spawnEnemies(int num, float min, float max) {

    sf::Vector2f pPos = m_player->getComponent<CTransform>().pos;

    std::uniform_real_distribution<> distrAngle(0, 2 * std::numbers::pi); // Define the range for angles (0 to 2*pi)
    std::uniform_real_distribution<> distrDistance(min, max); // Define the range for distance

    for (int i = 0; i < num; ++i) {
        float distance = distrDistance(rng);
        float angle = distrAngle(rng);

        sf::Vector2f ePos;
        ePos.x = pPos.x + distance * std::cos(angle);
        ePos.y = pPos.y + distance * std::sin(angle);

        auto pPos = m_player->getComponent<CTransform>().pos;
        sf::Vector2f direction = pPos - ePos;

        // increase enemy speed by 10 after every 5 kills
        if (m_kill % 5 == 0) {
            enemySpeed += 1.f;
            std::cout << enemySpeed;
        }

        direction = normalize(direction);
        sf::Vector2f eVel = direction * enemySpeed;

        auto enemy = m_entityManager.addEntity("enemy");
        enemy->addComponent<CTransform>(ePos, eVel);
        enemy->addComponent<CBoundingBox>(sf::Vector2f(32.f, 32.f));
        enemy->addComponent<CLifespan>(1000.f);

        if (m_floor == 2 || m_floor == 3) {
            m_enemyType = floor23Enemy(rng);
            switch (m_enemyType)
            {
            case 1: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("car")); break;
            case 2: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("raceCarL")); break;
            default:
                break;
            }
        }
        else if (m_floor == 4 || m_floor == 5) {
            m_enemyType = floor45Enemy(rng);
            switch (m_enemyType)
            {
            case 1: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("car")); break;
            case 2: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("raceCarL")); break;
            case 3: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("raceCarR")); break;
            default:
                break;
            }
        }
        else if (m_floor == 6 || m_floor == 7) {
            m_enemyType = floor67Enemy(rng);
            switch (m_enemyType)
            {
            case 1: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("car")); break;
            case 2: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("raceCarL")); break;
            case 3: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("raceCarR")); break;
            case 4: enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("tractor")); break;
            default:
                break;
            }
        }
        else {
            enemy->addComponent<CAnimation>(Assets::getInstance().getAnimation("car"));
        }
        std::cout << enemySpeed << "\n";
    }
}

void Scene_Frogger::enemyAction(sf::Time dt) {
    sf::Vector2f playerPos = m_player->getComponent<CTransform>().pos;

    for (const auto& entity : m_entityManager.getEntities("enemy")) {
        sf::Vector2f enemyPos = entity->getComponent<CTransform>().pos;
        sf::Vector2f direction = playerPos - enemyPos;
        direction = normalize(direction); // Normalize the direction vector

        float enemySpeed = length(entity->getComponent<CTransform>().vel); // Preserve enemy speed
        sf::Vector2f newVelocity = direction * enemySpeed; // Adjust velocity towards the player

        entity->getComponent<CTransform>().vel = newVelocity; // Update enemy velocity
    }
}
#pragma endregion

#pragma region [Update]

#pragma endregion

#pragma region [Render]

#pragma endregion


void Scene_Frogger::sMovement(sf::Time dt) {
    playerMovement();

    // move all objects
    for (auto e: m_entityManager.getEntities()) {
        if (e->hasComponent<CTransform>()) {
            auto&tfm = e->getComponent<CTransform>();

            tfm.pos += tfm.vel * dt.asSeconds();
            tfm.angle += tfm.angVel * dt.asSeconds();
        }
    }
}

// Map Keyboard Keys to Event Names
void Scene_Frogger::registerActions() {

    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "BACK");
    registerAction(sf::Keyboard::Q, "QUIT");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::X, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::Z, "TOGGLE_GRID");
    registerAction(sf::Keyboard::O, "TOGGLE_SHROUD");
    registerAction(sf::Keyboard::Tab, "TOGGLE_HUD");
    registerAction(sf::Keyboard::Tilde, "TOGGLE_DEVHUD");

    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::Left, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Right, "RIGHT");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Down, "DOWN");

    registerAction(sf::Keyboard::Num1, "SKILL_1");
    registerAction(sf::Keyboard::Num2, "SKILL_2");
    registerAction(sf::Keyboard::Num3, "SKILL_3");

    registerAction(sf::Keyboard::J, "PISTOL");
    registerAction(sf::Keyboard::K, "SHOTGUN");
    registerAction(sf::Keyboard::L, "SNIPER");

    registerAction(sf::Keyboard::Numpad1, "FLOOR_1");
    registerAction(sf::Keyboard::Numpad2, "FLOOR_2");
    registerAction(sf::Keyboard::Numpad3, "FLOOR_3");
    registerAction(sf::Keyboard::Numpad4, "FLOOR_4");
    registerAction(sf::Keyboard::Numpad5, "FLOOR_5");
    registerAction(sf::Keyboard::Numpad6, "FLOOR_6");
    registerAction(sf::Keyboard::Numpad7, "FLOOR_7");

}

// Set and Detect Keyboard Actions/Triggers
void Scene_Frogger::sDoAction(const Command& action) {
    // On Key Press
    if (action.type() == "START") {
        if (action.name() == "PAUSE") { setPaused(!m_isPaused); }
        else if (action.name() == "QUIT") { m_game->quitLevel(); }
        else if (action.name() == "BACK") { m_game->backLevel(); }

        else if (action.name() == "TOGGLE_DEVHUD") { m_drawDevHUD = !m_drawDevHUD; }
        else if (action.name() == "TOGGLE_HUD" and m_drawDevHUD) { m_drawHUD = !m_drawHUD; }
        else if (action.name() == "TOGGLE_SHROUD" and m_drawDevHUD) { m_drawShroud = !m_drawShroud; }
        else if (action.name() == "TOGGLE_COLLISION" and m_drawDevHUD) { m_drawAABB = !m_drawAABB; }
        else if (action.name() == "TOGGLE_GRID" and m_drawDevHUD) { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "TOGGLE_TEXTURE" and m_drawDevHUD) { m_drawTextures = !m_drawTextures; }
        
        else if (action.name() == "SKILL_1" and m_drawDevHUD) { if (shadowSight == true) { return; } skill1_unlocked = true; shadowSight = true; } // Shadow Sight
        else if (action.name() == "SKILL_2" and m_drawDevHUD) { if (abyssalResonance == true) { return; } skill2_unlocked = true; abyssalResonance = true; enemySpeed *= 0.25f; } // Abyssal Resonance
        else if (action.name() == "SKILL_3" and m_drawDevHUD) { if (spectralEcho == true) { return; } skill3_unlocked = true; spectralEcho = true; bulletSpread = true; bulletPierce = true; shotDecay = 0.1f;} // Spectral Echo

        else if (action.name() == "PISTOL" and m_drawDevHUD) { pistolEquipped = true; shotgunEquipped = false; sniperEquipped = false; bulletPierce = false; bulletSpread = false; }
        else if (action.name() == "SHOTGUN" and m_drawDevHUD) { shotgunEquipped = true; pistolEquipped = false; sniperEquipped = false; bulletSpread = true; bulletPierce = false;}
        else if (action.name() == "SNIPER" and m_drawDevHUD) { sniperEquipped = true; pistolEquipped = false; shotgunEquipped = false; bulletPierce = true; bulletSpread = false;}

        else if (action.name() == "FLOOR_1" and m_drawDevHUD) { m_floor = 1; displayTimer.restart();        }
        else if (action.name() == "FLOOR_2" and m_drawDevHUD) { m_floor = 2; displayTimer.restart();        }
        else if (action.name() == "FLOOR_3" and m_drawDevHUD) { m_floor = 3; displayTimer.restart();        }
        else if (action.name() == "FLOOR_4" and m_drawDevHUD) { m_floor = 4; displayTimer.restart();        }
        else if (action.name() == "FLOOR_5" and m_drawDevHUD) { m_floor = 5; displayTimer.restart();        }
        else if (action.name() == "FLOOR_6" and m_drawDevHUD) { m_floor = 6; displayTimer.restart();        }
        else if (action.name() == "FLOOR_7" and m_drawDevHUD) { m_floor = 7; displayTimer.restart();        }

        // Player control
        else if (action.name() == "LEFT") {
            m_player->getComponent<CInput>().left = true;

            const sf::Time intervalBetweenSteps = sf::seconds(0.5f);
            if (stepClock.getElapsedTime() < intervalBetweenSteps)
                return;
            stepClock.restart();

            SoundPlayer::getInstance().play("footstep", m_player->getComponent<CTransform>().pos);
        }
        else if (action.name() == "RIGHT") {
            m_player->getComponent<CInput>().right = true;

            const sf::Time intervalBetweenSteps = sf::seconds(0.5f);
            if (stepClock.getElapsedTime() < intervalBetweenSteps)
                return;
            stepClock.restart();

            SoundPlayer::getInstance().play("footstep", m_player->getComponent<CTransform>().pos);
        }
        else if (action.name() == "UP") {
            m_player->getComponent<CInput>().up = true;

            const sf::Time intervalBetweenSteps = sf::seconds(0.5f);
            if (stepClock.getElapsedTime() < intervalBetweenSteps)
                return;
            stepClock.restart();

            SoundPlayer::getInstance().play("footstep", m_player->getComponent<CTransform>().pos);
        }
        else if (action.name() == "DOWN") {
            m_player->getComponent<CInput>().down = true;

            const sf::Time intervalBetweenSteps = sf::seconds(0.5f);
            if (stepClock.getElapsedTime() < intervalBetweenSteps)
                return;
            stepClock.restart();

            SoundPlayer::getInstance().play("footstep", m_player->getComponent<CTransform>().pos);
        }

    }
    // on Key Release
    else if (action.type() == "END") {
        if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
        else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
        else if (action.name() == "UP") { m_player->getComponent<CInput>().up = false; }
        else if (action.name() == "DOWN") { m_player->getComponent<CInput>().down = false; }
    }
}

// Go to the MENU SCREEN
void Scene_Frogger::onEnd() {
    m_game->changeScene("MENU", nullptr, false);
}


void Scene_Frogger::playerMovement() {
    // no movement if player is dead
    if (m_player->hasComponent<CState>() && m_player->getComponent<CState>().state == "dead")
        return;

    // player movement
    sf::Vector2f pv{ 0, 0.f };
    auto& pInput = m_player->getComponent<CInput>();
    if (pInput.left) pv.x -= 1;
    if (pInput.right) pv.x += 1;
    if (pInput.up) pv.y -= 1;
    if (pInput.down) pv.y += 1;
    pv = normalize(pv);

    sf::Vector2f aimDirection(50, 50);
    m_player->getComponent<CTransform>().vel = playerSpeed * pv;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        auto clickPos = sf::Mouse::getPosition(m_game->window());
        //auto worldMousePos = m_game->window().mapPixelToCoords(clickPos);
        //auto playerPos = m_player->getComponent<CTransform>().pos;
        //aimDirection = normalize(worldMousePos - playerPos);
        std::cout << clickPos.x << ", " << clickPos.y << "\tLEFT\n";
        spawnBullet(sf::Vector2f(clickPos));
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        auto clickPos = sf::Mouse::getPosition(m_game->window());
        /*auto worldMousePos = m_game->window().mapPixelToCoords(clickPos);
        auto playerPos = m_player->getComponent<CTransform>().pos;
        aimDirection = normalize(worldMousePos - playerPos);*/
        std::cout << clickPos.x << ", " << clickPos.y << "\tRIGHT\n";
    }

    // console out
    //std::cout << "( " << pv.x << ", " << pv.y << " )\n";



}


void Scene_Frogger::sRender() {
    m_game->window().setView(m_worldView);

    // draw bkg first
    for (auto e : m_entityManager.getEntities("bkg")) {
        if (e->getComponent<CSprite>().has) {
            auto& sprite = e->getComponent<CSprite>().sprite;
            m_game->window().draw(sprite);
        }
    }

    for (auto& e : m_entityManager.getEntities()) {
        if (!e->hasComponent<CAnimation>())
            continue;

        // Draw Sprite
        auto& anim = e->getComponent<CAnimation>().animation;
        auto& tfm = e->getComponent<CTransform>();
        anim.getSprite().setPosition(tfm.pos);
        anim.getSprite().setRotation(tfm.angle);
        m_game->window().draw(anim.getSprite());

        if (m_drawAABB) { // Toggle Hitbox Here
            if (e->hasComponent<CBoundingBox>()) {
                auto box = e->getComponent<CBoundingBox>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f{ box.size.x, box.size.y });
                centerOrigin(rect);
                rect.setPosition(e->getComponent<CTransform>().pos);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color{ 0, 255, 0 });
                rect.setOutlineThickness(2.f);
                m_game->window().draw(rect);
            }
        }

        if (m_drawGrid) { // Toggle Grid View Here
            sf::RectangleShape line(sf::Vector2f(m_worldView.getSize().y, 1));
            line.setPosition(m_worldView.getSize().x / 2.f, 0.f);
            line.rotate(90);
            m_game->window().draw(line);
        }

        if (m_drawTextures) {
            // Toggle Textures Here
        }
    }

    if (m_drawShroud) {
        if (m_floor == 1 || shadowSight == true) {
            for (auto e : m_entityManager.getEntities("frg3")) {
                if (e->getComponent<CSprite>().has) {
                    auto& sprite = e->getComponent<CSprite>().sprite;
                    m_game->window().draw(sprite);
                }
            }
        }

        else if (m_floor == 2 || m_floor == 3) {
            for (auto e : m_entityManager.getEntities("frg2")) {
                if (e->getComponent<CSprite>().has) {
                    auto& sprite = e->getComponent<CSprite>().sprite;
                    m_game->window().draw(sprite);
                }
            }
        }

        else if (m_floor == 4 || m_floor == 5) {
            for (auto e : m_entityManager.getEntities("frg1")) {
                if (e->getComponent<CSprite>().has) {
                    auto& sprite = e->getComponent<CSprite>().sprite;
                    m_game->window().draw(sprite);
                }
            }
        }

        else if (m_floor == 6 || m_floor == 7) {
            for (auto e : m_entityManager.getEntities("frg0")) {
                if (e->getComponent<CSprite>().has) {
                    auto& sprite = e->getComponent<CSprite>().sprite;
                    m_game->window().draw(sprite);
                }
            }
        }
    }
    
    if (m_drawHUD) {
        for (auto e : m_entityManager.getEntities("cur")) {
            if (e->getComponent<CSprite>().has) {
                auto clickPos = sf::Mouse::getPosition(m_game->window());
                sf::Vector2f clickPosF(static_cast<float>(clickPos.x), static_cast<float>(clickPos.y));
                auto& sprite = e->getComponent<CSprite>().sprite;
                e->addComponent<CTransform>(clickPosF);
                m_game->window().draw(sprite);
            }
        }

        hours = static_cast<int>(gameClock.getElapsedTime().asSeconds()) / 3600;
        minutes = (static_cast<int>(gameClock.getElapsedTime().asSeconds()) / 60) % 60;
        seconds = static_cast<int>(gameClock.getElapsedTime().asSeconds()) % 60;

        ss.str("");
        ss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

        m_clock_text.setString("Time    : " + ss.str());
        m_score_text.setString("Score   : " + std::to_string(m_score));
        m_lives_text.setString("Lives   : " + std::to_string(m_lives));
        m_ammo_text.setString("Ammo   : " + std::to_string(m_ammo));
        m_kill_text.setString("Kills   : " + std::to_string(m_kill));

        m_game->window().draw(m_clock_text);
        m_game->window().draw(m_score_text);
        m_game->window().draw(m_lives_text);
        m_game->window().draw(m_ammo_text);
        m_game->window().draw(m_kill_text);
    }

    if (floor_display == true && (displayTimer.getElapsedTime() < displayDuration))
    {
        m_floor_text.setFont(Assets::getInstance().getFont("abyssal"));
        m_floor_text.setPosition(m_worldView.getCenter().x, m_worldView.getCenter().y - m_worldView.getCenter().y + 150.f);
        m_floor_text.setOrigin(123, 74);
        m_floor_text.setCharacterSize(75);

        switch (m_floor)
        {
            case 1: m_floor_text.setString("Floor 1"); break;
            case 2: m_floor_text.setString("Floor 2"); break;
            case 3: m_floor_text.setString("Floor 3"); break;
            case 4: m_floor_text.setString("Floor 4"); break;
            case 5: m_floor_text.setString("Floor 5"); break;
            case 6: m_floor_text.setString("Floor 6"); break;
            case 7: m_floor_text.setString("Floor 7"); break;
            default: break;
        }

        m_game->window().draw(m_floor_text);
    }

    if (m_lives == 0) {
        m_gameOver_text.setFont(Assets::getInstance().getFont("main"));
        m_gameOver_text.setPosition(m_worldView.getCenter());
        m_gameOver_text.setOrigin(288,55);
        m_gameOver_text.setCharacterSize(75);
        m_gameOver_text.setString("!!! GAME OVER !!!");
        m_game->window().draw(m_gameOver_text);
    }

    if (m_drawDevHUD) {
        m_controls_text.setFont(Assets::getInstance().getFont("main"));
        m_controls_text.setPosition(10.f, 255.f);
        m_controls_text.setCharacterSize(25);
        m_controls_text.setString(R"(
        DEV MODE [ON]
            1    : Shadow Sight
            2   : Abyssal Resonance
            3   : Spectral Echo

            J    : Pistol
            K   : Shotgun
            L   : Sniper
        )");
        m_game->window().draw(m_controls_text);
    }
}


void Scene_Frogger::update(sf::Time dt) {
    sUpdate(dt);
    sf::Time skill1Cooldown = skill1Clock.getElapsedTime();
    if (skill1Cooldown.asSeconds() >= 12) {
        shadowSight = false;
        skill1Clock.restart();
    }

    sf::Time skill2Cooldown = skill2Clock.getElapsedTime();
    if (skill2Cooldown.asSeconds() >= 9) {
        abyssalResonance = false; enemySpeed *= 1.25f;
        skill2Clock.restart();
    }

    sf::Time skill3Cooldown = skill3Clock.getElapsedTime();
    if (skill3Cooldown.asSeconds() >= 6) {
        spectralEcho = false; 
        if (!shotgunEquipped) bulletSpread = false; 
        if (!sniperEquipped) bulletPierce = false;
        shotDecay = 1.f;
        skill3Clock.restart();
    }

    if (m_drawDevHUD == false) {
        switch (m_kill)
        {
            case 20: m_floor = 2; skill1_unlocked = true; break;
            case 30: m_floor = 3; skill2_unlocked = true; playerSpeed *= 1.5f; break;
            case 50: m_floor = 4; skill3_unlocked = true; playerSpeed *= 1.5f; break;
            case 60: m_floor = 5; playerSpeed *= 1.25f; break;
            case 80: m_floor = 6; playerSpeed *= 1.25f; break;
            case 100: m_floor = 7; break;
            default:
                break;
        }
    }

}

// Set Collision Events
void Scene_Frogger::sCollisions() {
    adjustPlayerPosition();

    auto pPos = m_player->getComponent<CTransform>().pos;
    auto& pVel = m_player->getComponent<CTransform>().vel;
    bool m_riding{ false };

    //Player vs Enemy
    for (auto e : m_entityManager.getEntities("enemy")) {
        auto collide = Physics::getOverlap(m_player, e);

        // Check if the player is currently invincible
        if (playerInvincible) {
            // Check if the invincibility duration has elapsed
            if (invincibilityTimer.getElapsedTime() >= sf::seconds(3.f)) {
                // Reset invincibility status after 3 seconds
                playerInvincible = false;
                invincibilityTimer.restart();
            }
            else {
                // Skip this iteration if the player is still invincible
                continue;
            }
        }

        if (collide.x > 0.f and collide.y > 0.f) {

            if (!spectralEcho)
            {
                m_player->addComponent<CState>().state = "die";
                m_player->addComponent<CAnimation>(Assets::getInstance().getAnimation("die"));
                m_player->removeComponent<CBoundingBox>();
                m_player->removeComponent<CInput>();
            }

            e->addComponent<CState>().state = "die";
            e->addComponent<CAnimation>(Assets::getInstance().getAnimation("die"));
            e->removeComponent<CBoundingBox>();
            e->removeComponent<CAnimation>();

            m_lives--;
            m_ammo = 7;
            SoundPlayer::getInstance().play("playerDie", m_player->getComponent<CTransform>().pos);
        }
    } 

    //Bullet vs Enemy
    for (auto b : m_entityManager.getEntities("bullet")) {
        for (auto e : m_entityManager.getEntities("enemy")) {
            auto collide = Physics::getOverlap(b, e);
            if (collide.x > 0.f and collide.y > 0.f) {
                e->addComponent<CState>().state = "die";
                e->addComponent<CAnimation>(Assets::getInstance().getAnimation("die"));
                e->removeComponent<CBoundingBox>();

                if (bulletPierce == false){
                    b->removeComponent<CBoundingBox>();
                    b->removeComponent<CAnimation>();
                }

                e->removeComponent<CAnimation>();
                m_score += 100;
                m_ammo += 1;
                m_kill += 1;
                SoundPlayer::getInstance().play("death", e->getComponent<CTransform>().pos);
                spawnEnemies(1, 210.f, 2000.f);
            }
        }
    }

    if (m_lives == 7) {
        m_isPaused = false;
    }
    
}


void Scene_Frogger::sUpdate(sf::Time dt) {
    SoundPlayer::getInstance().removeStoppedSounds();
    m_entityManager.update();

    if (m_isPaused)
        return;

    sAnimation(dt);
    
    sMovement(dt);
    adjustPlayerPosition();
    
    enemyAction(dt);
    manageMobs();
    sCollisions();
    sRender();
}

// Animations Event Manager
void Scene_Frogger::sAnimation(sf::Time dt) {
    auto list = m_entityManager.getEntities();
    auto pState = m_player->getComponent<CState>().state;
    auto pAnim = m_player->getComponent<CAnimation>();

    for (auto e: m_entityManager.getEntities()) {
        // update all animations
        if (e->hasComponent<CAnimation>()) {
            auto&anim = e->getComponent<CAnimation>();
            anim.animation.update(dt);
            // do nothing if animation has ended
            
            // CUSTOM EVENTS
            if (pAnim.animation.hasEnded() and pState == "die") {
                m_player->destroy();
                playerInvincible = true;
                spawnPlayer(m_worldView.getCenter());
            }

        }
    }
}

// Constraints for Plyer Positioning
void Scene_Frogger::adjustPlayerPosition() {

    if (m_player->getComponent<CState>().state == "dead")
        return;

    auto center = m_worldView.getCenter();
    sf::Vector2f viewHalfSize = m_worldView.getSize() / 2.f;

    auto left = center.x - viewHalfSize.x;
    auto right = center.x + viewHalfSize.x;
    auto top = center.y - viewHalfSize.y;
    auto bot = center.y + viewHalfSize.y;

    auto&player_pos = m_player->getComponent<CTransform>().pos;
    auto halfSize = m_player->getComponent<CBoundingBox>().halfSize;
    // keep player in bounds
    player_pos.x = std::max(player_pos.x, left + halfSize.x);
    player_pos.x = std::min(player_pos.x, right - halfSize.x);
    player_pos.y = std::max(player_pos.y, top + halfSize.y);
    player_pos.y = std::min(player_pos.y, bot - halfSize.y);
}

// Check Player State(s)
void Scene_Frogger::checkPlayerState() {
}

// Load Level properties from config file
void Scene_Frogger::loadLevel(const std::string&path) {
    std::ifstream config(path);
    if (config.fail()) {
        std::cerr << "Open file " << path << " failed\n";
        config.close();
        exit(1);
    }

    std::string token{""};
    config >> token;
    while (!config.eof()) {
        if (token == "Bkg") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("bkg");

            // for background, no textureRect its just the whole texture
            // and no center origin, position by top left corner
            // stationary so no CTransfrom required.
            auto&sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }

        else if (token == "Frg0") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("frg0");
            auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }

        else if (token == "Frg1") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("frg1");
            auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }

        else if (token == "Frg2") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("frg2");
            auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }

        else if (token == "Frg3") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("frg3");
            auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }

        else if (token == "Cur") {
            std::string name;
            sf::Vector2f pos;
            config >> name >> pos.x >> pos.y;
            auto e = m_entityManager.addEntity("cur");
            auto& sprite = e->addComponent<CSprite>(Assets::getInstance().getTexture(name)).sprite;
            sprite.setOrigin(0.f, 0.f);
            sprite.setPosition(pos);
        }

        else if (token[0] == '#') {
            std::cout << token;
        }

        config >> token;
    }

    config.close();
}
