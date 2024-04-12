//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_SCENE_PLAYSCREEN_H
#define BREAKOUT_SCENE_PLAYSCREEN_H

#include <queue>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics.hpp>
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"
#include "GameEngine.h"
#include "chrono"
#include "iomanip"
#include "sstream"
#include "iostream"

//struct LevelConfig {
//	float       scrollSpeed{ 100.f };
//	float       playerSpeed{ 200.f };
//	float       enemySpeed{ 200.f };
//	float       bulletSpeed{ 400.f };
//	float       missileSpeed{ 150.f };
//	sf::Time    fireInterfal{ sf::seconds(5) };
//
//	std::map<std::string,
//		std::vector<std::pair<float, sf::Time>>> directions;
//};

struct SpawnPoint {
	float           y;
	std::string     type;
	unsigned char   number;
	//auto operator<=>(const SpawnPoint& rhs) const = default;
	auto operator<=>(const SpawnPoint& rhs) const {
		return y <=> rhs.y;
	}
};

class Scene_Frogger : public Scene {
private:
	sPtrEntt        m_player{ nullptr };
	sf::View        m_worldView;
	sf::FloatRect   m_worldBounds;
	sf::RenderWindow            m_window;

	//LevelConfig                         m_config;
	std::priority_queue<SpawnPoint>     m_spawnPoints;
	
	sf::Clock		gameClock;
	sf::Clock		shotClock;
	sf::Clock		stepClock;
	sf::Clock		invincibilityTimer;
	//sf::Clock		displayTimer;
	//const sf::Time displayDuration = sf::seconds(3.0f);


	sf::Clock		skill1Clock;
	sf::Clock		skill2Clock;
	sf::Clock		skill3Clock;

	std::stringstream ss;
	sf::Music		bgm;

	
	bool			playerInvincible{ true };
	bool			m_drawTextures{ true };
	bool			m_drawAABB{ false };
	bool			m_drawGrid{ false };
	bool			m_drawShroud{ true };
	bool			m_drawHUD{ true };
	bool			m_drawDevHUD{ false };
	bool			canDie{ true };

	bool			bulletSpread{ false };
	int				spreadCount{ 5 };
	bool			bulletPierce{ false };

	bool			shadowSight{ false };
	bool			abyssalResonance{ false };
	bool			spectralEcho{ false };

	bool			skill1_unlocked{ false };
	bool			skill2_unlocked{ false };
	bool			skill3_unlocked{ false };

	bool			floor_display{ true };

	float			enemySpeed = 50.0f;

	sf::Texture		m_shroud;
	sf::Texture		m_light_bullet;


	int				hours = static_cast<int>(gameClock.getElapsedTime().asSeconds()) / 3600;
	int				minutes = (static_cast<int>(gameClock.getElapsedTime().asSeconds()) / 60) % 60;
	int				seconds = static_cast<int>(gameClock.getElapsedTime().asSeconds()) % 60;

	int             m_lives{ 7 };
	int             m_score{ 0 };
	int				m_ammo{ 7 };
	int				m_kill{ 0 };
	int				m_enemyType;

	float			shotSpeed;
	float			shotDecay{ 1.f };
	float			shotAngles[5];
	float			startingAngle;
	int				m_floor{ 1 };
	int				m_totalKills{ 0 };
	int				m_totalDeaths{ 0 };
	int				m_totalExp{ 0 };
	int				m_totalClockBonus{ 0 };	// Exp is set to 100 in level 1... Exp goes down by half per missed shot, resets per hit
	

	sf::Text		m_floor_text;
	sf::Text        m_clock_text;
	sf::Text        m_lives_text;
	sf::Text        m_score_text;
	sf::Text		m_ammo_text;
	sf::Text		m_kill_text;
	sf::Text		m_gameOver_text;
	sf::Text		m_controls_text;


	//systems
	void            sMovement(sf::Time dt);
	void            sCollisions();
	void            sUpdate(sf::Time dt);
	void            sAnimation(sf::Time dt);

	void            sGunUpdate(sf::Time dt);
	void            sGuideMissiles(sf::Time dt);
	void            sSpawnEnemies();

	void	        onEnd() override;

	// helper functions
	void            addSpawnPoint(float y, const std::string& name, unsigned char number);

	void            playerMovement();
	void            adjustPlayerPosition();
	void            checkPlayerState();
	void	        registerActions();
	void            spawnPlayer(sf::Vector2f pos);
	void            manageMobs();
	void            checkCollision();

	void            checkMissileCollision();
	void            checkBulletCollision();
	void            checkPickupCollision();
	void            dropPickup(sf::Vector2f pos);
	void            fireBullet();
	void            fireMissile();
	sf::Vector2f    findClosestEnemy(sf::Vector2f mPos);
	void            spawnBullet(sf::Vector2f dir);

	void			spawnEnemies(int amount, float minDist, float maxDist);
	void            enemyAction(sf::Time dt);


	void            init();
	void            loadLevel(const std::string& path);
	sf::FloatRect   getViewBounds();
	sf::FloatRect   getBattlefieldBounds() const;

public:

	Scene_Frogger(GameEngine* gameEngine, const std::string& levelPath);

	void		  update(sf::Time dt) override;
	void		  sDoAction(const Command& command) override;
	void		  sRender() override;

};



#endif //BREAKOUT_SCENE_PLAYSCREEN_H
