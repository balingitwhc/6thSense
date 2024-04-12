#include "Scene_Menu.h"
#include "Scene_Frogger.h"
#include "MusicPlayer.h"
#include <memory>

void Scene_Menu::onEnd()
{
	m_game->window().close();
}

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu:: init()
{
    registerAction(sf::Keyboard::W,			"UP");
    registerAction(sf::Keyboard::Up,		"UP");
    registerAction(sf::Keyboard::S,			"DOWN");
    registerAction(sf::Keyboard::Down,	 	"DOWN");
	registerAction(sf::Keyboard::D,			"PLAY");
	registerAction(sf::Keyboard::Escape,	"QUIT");
	registerAction(sf::Keyboard::Enter,		"SELECT");
	registerAction(sf::Keyboard::Return,	"SELECT");

	m_title = "6th Sense";
	m_menuStrings.push_back("Play");
	m_menuStrings.push_back("Options");
	m_menuStrings.push_back("Help");
	m_menuStrings.push_back("About");
	m_menuStrings.push_back("Quit");

	m_levelPaths.push_back("../assets/floor1.txt");
	m_levelPaths.push_back("../assets/floor2.txt");
	m_levelPaths.push_back("../assets/floor3.txt");
	m_levelPaths.push_back("../assets/floor4.txt");
	m_levelPaths.push_back("../assets/floor5.txt");
	m_levelPaths.push_back("../assets/floor6.txt");
	m_levelPaths.push_back("../assets/floor7.txt");

	m_menuText.setFont(Assets::getInstance().getFont("abyssal"));

	const size_t CHAR_SIZE{ 64 };
	m_menuText.setCharacterSize(CHAR_SIZE);

	MusicPlayer::getInstance().play("menuTheme");
	MusicPlayer::getInstance().setVolume(100);
}

void Scene_Menu::update(sf::Time dt)
{
	m_entityManager.update();
}


void Scene_Menu::sRender()
{
	 
	sf::View view = m_game->window().getView();
	view.setCenter(m_game->window().getSize().x / 2.f, m_game->window().getSize().y / 2.f);
	m_game->window().setView(view);

	static const sf::Color selectedColor(255, 255, 255);
	static const sf::Color normalColor(100, 100, 255);

	static const sf::Color backgroundColor(0, 0, 0);

	m_menuBG.loadFromFile("../assets/Textures/bg_splash.jpg");
	sf::Sprite ms_menuBG(m_menuBG);
	m_game->window().draw(ms_menuBG);

	sf::Text footer("Navigate: [UP/DOWN]   |   SELECT: [ENTER]   |   QUIT: [ESCAPE]", 
		Assets::getInstance().getFont("main"), 25);
	footer.setFillColor(normalColor);
	footer.setPosition(64, 720);

	//m_game->window().clear(backgroundColor);

	m_menuText.setFillColor(normalColor);
	m_menuText.setString(m_title);
	m_menuText.setPosition(32, 32);
	m_game->window().draw(m_menuText);

	for (size_t i{ 0 }; i < m_menuStrings.size(); ++i)
	{
		m_menuText.setFillColor((i == m_menuIndex ? selectedColor : normalColor));
		m_menuText.setPosition(64, 64 + (i+1) * 96);
		m_menuText.setString(m_menuStrings.at(i));
		m_game->window().draw(m_menuText);
	} 

	m_game->window().draw(footer);

	//m_game->window().display();

}


void Scene_Menu::sDoAction(const Command& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			m_menuIndex = (m_menuIndex + m_menuStrings.size() - 1) % m_menuStrings.size();
		} 
		else if (action.name() == "DOWN")
		{
			m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			//m_game->changeScene("PLAY", std::make_shared<Scene_Frogger>(m_game, m_levelPaths[m_menuIndex]));
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}

		else if (action.name() == "SELECT")
		{
			// std::cout << m_menuIndex; // FOR DEBUGGING

			switch (m_menuIndex)
			{
			case 0:
				m_game->changeScene("PLAY", std::make_shared<Scene_Frogger>(m_game, m_levelPaths[m_menuIndex]));
				std::cout << "GAME ON! \n";
				break;	// Play

			case 1:
				std::cout << "SETTINGS! \n";
				break;	// Options

			case 2:
				std::cout << "HOW-TO? \n";
				break;	// Help

			case 3:
				std::cout << "HMMM... \n";
				break;	// About

			case 4:
				onEnd();
				std::cout << "EXIT \n";
				break;	// Quit


			default:
				break; // ERROR
			}
		}
	}

}
