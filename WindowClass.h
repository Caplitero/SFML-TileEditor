#pragma once

#include "TileMapLoader.h"
class Window {

	sf::RenderWindow* window = nullptr;
	CAP::SFMLMap* Map;


	void update()
	{
		sf::RenderStates states;
		window->clear();
		Map->draw(*window,states);
		window->display();
	}

	void close(){

		delete window;
	}

	void loop()
	{
		sf::Event event;
		while (window->isOpen())
		{
			while (window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window->close();

				}


			}
			update();


		}
		close();


	}


public :

	
	void start(unsigned int Width, unsigned int Height, const char* Title)
	{

		window = new sf::RenderWindow(sf::VideoMode(Width, Height), Title);
		Map = new CAP::SFMLMap("Maps/", "untitled.tmx");
		loop();

	}

};