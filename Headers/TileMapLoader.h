#pragma once

#include "XMLParse.h"
#include <SFML/Graphics.hpp>

namespace CAP {

	struct _TileSet
	{   
		std::string  name;
		std::string  image;
		int          tilewidth;
		int          tileheight;

		_TileSet(_Node* doc)
		{

			for (_Node* node : doc->Children)
			{

				if (node->tag == "image")
				{


					image = node->find_keyValue("source");


					break;
				}
			}


		}



	};

	struct _TileLayer {

		int rows;
		int columns;
		int id;
		std::vector<int> lay;

		_TileLayer(_Node node)
		{
			columns = stoi(node.find_keyValue("width"));
			rows = stoi(node.find_keyValue("height"));
			id = stoi(node.find_keyValue("id"));

			std::string layer = node.Children[0]->inner_Text;

			int i = 0; std::string number;
			while (layer[i] != '\0')
			{
				if (layer[i] == ',' || layer[i + 1] == '\0')
				{
					lay.push_back(stoi(number));
					number.clear();
				}
				else {
					number = number + layer[i];
				}

				i++;
			}

		}
	};

	class TileMapFile
	{
		XMLDocument* Map = nullptr;


	public:
		TileMapFile()
		{
			Map = new XMLDocument;

		}

		std::vector<_TileSet> TileSets;
		std::vector<_TileLayer > TileLayers;
		std::vector<_Node> ObjectLayers;
		std::vector<_Node> Imagelayers;
		int width;
		int height;


		bool load(std::string FOLDER, std::string FILE)
		{
			int result = Map->load((FOLDER + FILE).c_str());
			if (result == Successful) {

				for (_Node* child : Map->Root->Children[0]->Children)
				{
					if (child->tag == "tileset") {
						XMLDocument newdoc; std::string newfile = child->find_keyValue("source");
						int ok = newdoc.load((FOLDER + newfile).c_str());
						if (ok == FileNotFound)
						{
							std::cout << FOLDER + newfile << " : Could not be found "; return 0;
						}

						else if (ok == Successful)
						{

							std::cout << "Tileset " << newfile << " : has loaded succesfully \n"; TileSets.push_back(_TileSet(newdoc.Root->Children[0])); // Load the tilesets 

						}


					}

					if (child->tag == "objectgroup")    // Save all object layers
					{
						ObjectLayers.push_back(*child);
					}

					if (child->tag == "imagelayer")    // Save all image layers
						Imagelayers.push_back(*child);
					if (child->tag == "layer")
					{
						TileLayers.push_back(_TileLayer(*child));
					}


				}
				std::cout << "Tilemap " << FILE << " :  has loaded succesfully \n";

				width = stoi(Map->Root->Children[0]->find_keyValue("tilewidth"));
				height = stoi(Map->Root->Children[0]->find_keyValue("tileheight"));
			}
			else if (result == FileNotFound) {
				std::cout << FOLDER + FILE << " : Could not be found "; return 0;
			}

			delete(Map); // The Document is of not use from now on 



			return 1;

		}


	};


	class SFMLMap : public sf::Drawable {
		sf::Texture m_texture;
		sf::VertexArray m_vertices;

		std::string _FOLDER;
		std::string _FILE;
	public:
		TileMapFile doc;
		SFMLMap(std::string Folder, std::string FILE)
		{
			_FOLDER = Folder;
			_FILE = FILE;
		}

		
		bool load()
		{
			

			if (doc.load(_FOLDER, _FILE) == Successful)
			{   
				if (doc.TileSets.size())
				{
					
					m_texture.loadFromFile(_FOLDER + doc.TileSets[0].image);
					std::cout << doc.TileSets[0].name << "\n";
					int dis = m_texture.getSize().x / doc.width;
					m_vertices.setPrimitiveType(sf::Quads);
					m_vertices.resize(doc.TileLayers[0].columns * doc.TileLayers[0].rows * 4);

					for (int i = 0; i < doc.TileLayers[0].rows; i++)

						for (int j = 0; j < doc.TileLayers[0].columns; j++)
						{
							int index = (j + i * doc.TileLayers[0].columns) * 4;
							int gid = doc.TileLayers[0].lay[j + i * doc.TileLayers[0].columns];

							sf::Vertex* vertex = &m_vertices[index];
							vertex[0].position = sf::Vector2f(doc.width * j, doc.height * i);
							vertex[1].position = sf::Vector2f((j + 1) * doc.width, i * doc.height);
							vertex[2].position = sf::Vector2f((j + 1) * doc.width, (i + 1) * doc.height);
							vertex[3].position = sf::Vector2f(j * doc.width, (i + 1) * doc.height);

							int colp = gid % dis - 1;
							int rowp = gid / dis;

							vertex[0].texCoords = sf::Vector2f(doc.width * colp, doc.height * rowp);
							vertex[1].texCoords = sf::Vector2f((colp + 1) * doc.width, rowp * doc.height);
							vertex[2].texCoords = sf::Vector2f((colp + 1) * doc.width, (rowp + 1) * doc.height);
							vertex[3].texCoords = sf::Vector2f(colp * doc.width, (rowp + 1) * doc.height);


						}
					
				}
				else 
				std::cout << _FILE << ": no tileset loaded... \n";

				
				return 1;
			
			}
			std::cout << "\n" << _FILE << " failed to load... \n";
			return 0;
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const {
			states.texture = &m_texture;
			target.draw(m_vertices, states);

		}



	};

}