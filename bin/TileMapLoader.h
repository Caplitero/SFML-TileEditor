#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <map>

namespace CAP
{

	enum
	{
		FAIL,	// Node is bad
		MULTIPLE, // Node is structured on multiple levels
		SINGLE	// Node is structured on a single line
	};

	enum LoadValue
	{
		Fail,
		Successful,
		FileNotFound

	};

	// Basic Clock to get elapsed time
	class Clock
	{
		std::chrono::system_clock::time_point _STARTING_POINT;

	public:
		class _Time_Conversion
		{
			int value = 0;

		public:
			_Time_Conversion(int Time) { value = Time; }
			float asSeconds() { return value / 1e9f; }
			float asMilliSeconds() { return value / 1e6f; }
			int asNanoSeconds() { return value; }
		};
		Clock() { _STARTING_POINT = std::chrono::system_clock::now(); }
		// Return the elapsed time in milliseconds
		inline _Time_Conversion elapsedTime()
		{

			return _Time_Conversion(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - _STARTING_POINT).count());
		}
	};

	// Struct used for easier manipulation of Data
	struct _Variable
	{
		_Variable(std::string value)
		{
			_value = value;
		}
		_Variable() { _value = "\0"; }
		int toInt() { return atoi(_value.c_str()); }
		float toDouble() { return atof(_value.c_str()); }
		std::string toString() { return _value; }

	protected:
		std::string _value;
	};
     
     
	// Structure used to save data about the Layers of the game
	struct _Chunk
	{
	uint16_t Width;      // Width of the Layer's Data
	uint16_t Height;     // Height of the Layer's Data
	uint32_t x_offset=0; // X Offset for Infinite_Maps only 
	uint16_t y_offset=0; // Y Offset for Infinite_Maps only
	std::vector<std::vector<uint16_t>> Tile_Ids;  // The Chunk's data 
	_Chunk(uint16_t _Width,uint16_t _Height)
	{
		Width =_Width ; Height = _Height; // The Width and Height of the Chunk
		Tile_Ids.resize(Height, std::vector<uint16_t>(Width));  // Resize to the Size of the layer
	}
	void load(std::string *Buffer,unsigned &position)
	{
		std::string number; // Used to store the number that will be converted to int
		uint16_t I=0;       // Used to store the Y position
		uint16_t J=0;       // Used to store the X position

		position+=2; // The last position is a '<' so we must jump it and '\n' to get to the inner_text
          while((*Buffer)[position]!='<')
		     {
			while(isdigit((*Buffer)[position]))       
				number= number+(*Buffer)[position++]; // Parse the new number

		     if((*Buffer)[position]==',' || (*Buffer)[position+1]=='\n')
			{
               	Tile_Ids[I][J++]=(std::stoi(number)); // Convert string to int
				number.clear(); // Clear the Data for the next number
				I+=J/Width;     // Jump to the next row
				J=J%Width;      // Can't go further than the Width
			}
			position++;  // Continue to parse the inner_text
			}	
	}
};

	struct _Node
	{
		std::map<std::string, _Variable> AttributesList; // This map contains all the keys and values of the node
		std::string tag;						    // Tag of the Node
		std::string inner_Text;					    // Inner text of the node
		class _Node *parent;					    // Point to the upper node
		std::vector<_Node *> Children;			    // This vector contains pointers to all lower nodes

		// Goes through the node and gets all Attributes
		int find_attrs(std::string Buffer, unsigned &position)
		{
			std::string Search; // Attribute data parser
			position++;

			while (Buffer[position] != '>')
			{
				if (Buffer[position] != ' ' && Buffer[position] != '"')
					Search.push_back(Buffer[position]); // Read data
				position++;						 // Go to next character

				if (Buffer[position] == ' ' && tag.empty())
				{
					tag = Search;	 // Add node tag
					position++;	 // The data ends on the next position
					Search.clear(); // Clear vector for the next data input
				}

				else if (Buffer[position + 1] == '>' && Buffer[position] != '?' && tag.empty()) // Nodes with no keys
				{
					Search.push_back(Buffer[position]);
					tag = Search;	 // Add node tag
					position++;	 // The data ends on the next position
					Search.clear(); // Clear vector for the next data input
				}

				if (Buffer[position] == '=')
				{
					std::string key = Search; // = marks the end of a key

					Search.clear(); // Clear vector for the next data input
					position += 2;	 // The value starts on the next position

					if (Buffer[position - 1] == '"')
						while (Buffer[position] != '"')
							Search.push_back(Buffer[position++]); // Read the value
					else
						return false; // False because the file is incomplete

					this->AttributesList[key] = _Variable(Search); // Add the new Attribute to the node's list
					Search.clear();						  // Clear vector for the next data input

					continue;
				}

				if (Buffer[position] == '/' && Buffer[position + 1] == '>')
				{
					position++;
					return SINGLE; // Return that the node is on a single line (just one end)}
				}
			}
			return MULTIPLE;
		}
	};

	class XMLDocument
	{

	private:
		_Node *newChild(_Node *parent)
		{
			_Node *child = new _Node;		// Create a new node
			child->parent = parent;			// Specify upper level
			parent->Children.push_back(child); // Add the child to the parents vector
			return child;					// Return the node
		}
          
		void load_Properties(_Node *Map_Data)
		{
		std::cerr<<"->Loading map Properties \n";
		Properties.width       = Map_Data->AttributesList["width"].toInt();
		Properties.height      = Map_Data->AttributesList["height"].toInt();
		Properties.infinite    = Map_Data->AttributesList["infinite"].toInt();
		Properties.Tile_Width  = Map_Data->AttributesList["tilewidth"].toInt();
		Properties.Tile_Height = Map_Data->AttributesList["tileheight"].toInt();
		Properties.Orientation = Map_Data->AttributesList["orientation"].toInt();
		}
	public:
     //<-------Map properties------->//
	struct _Map_Properties
	{
		std::string Orientation = "\0";
		int Tile_Width = 0;
		int Tile_Height = 0;
		bool infinite = 0;
		int width = 0;
		int height = 0;
	} Properties;
	//<-------Map properties------->//

		std::string version;  // Version of the XML File
		std::string encoding; // Type of Encoding
		_Node *Root; // This is the xml root (<?xml?>)
		std::vector<_Chunk> Map_Layers; // All the layers saved as Chunks of data

		LoadValue load(const char *path)
		{

			std::ifstream file(path); // Load the XML Document
			if (!file.is_open())
				return FileNotFound; // Failed to load

			else // If the load didnt fail , get data
			{
				std::string Buffer((std::istreambuf_iterator<char>(file)), // The buffer is used to store all data from the file
							    std::istreambuf_iterator<char>());	    // Convert data from the file into char and append it to the buffer

				unsigned position = 0;		   // Place in the document
				std::string Search;			   // Saves text as going through the document
				unsigned lenght = Buffer.size(); // Lenght of the XML doc
				Root = new _Node;

				while (position < lenght) // Go through the document
				{
					if (Buffer[position] == '<') // New node
					{
						if (!Search.empty())
						{
							
							if (Search != " ")
								Root->inner_Text.append(Search + " "); // If the node has any data outside of tag , we save it
							Search.clear();					    // Clear vector for next data imput
						}

						if (Buffer[position + 1] == '!' && Buffer[position + 2] == '-') // Check for comments
						{
							while (Buffer[position] != '>')
								position++; // Comments are useless for the program so it will be parsed but ignored
							continue;		  // Node done , search for the next one , breaking the if
						}

						if (Buffer[position + 1] == '/')
						{
							position += 2; //  '/' marks the end of a node , and the name start 2 spaces later
							while (Buffer[position] != '>')
								Search.push_back(Buffer[position++]);

							// Read the end tag found

							if (Root->tag != Search) // Eevery node contains a Start_node
							{
								std::cout << Search << " Tag missmatch" << Root->tag << "\n";
								return Fail;
							} // and a End_node which must be equal
							Root = Root->parent;
							position++;
							Search.clear();
							continue; // Must skip to the next Search
						}

						if (Buffer[position + 1] == '?')
						{

							while (Buffer[position] != ' ' && Buffer[position] != '>')
								Search.push_back(Buffer[position++]); // Save to the vector the new found text

							if (Search == "<?xml") // Root of the XML Document used to store version and encoding
							{
								_Node Description; // A node to store data

								Description.find_attrs(Buffer, position); // Get all data from the root
								Root->AttributesList = Description.AttributesList;
								Root->tag = "xml";
								Search.clear(); // Clear the vector for new data imput
								position++;	 // Continue the search
								continue;		 // Skip to the next node
							}
						}

						Root = newChild(Root); // Search if the node has any child

						if (Root->find_attrs(Buffer, position) == SINGLE)
						{ // Some Nodes are on a single line and dont have children

							Root = Root->parent; // If the Node is inline we return to it's parent

							position++; // Continue the search
							continue;	  // Skip to the next node
						}
						if(Root->tag == "map")
							{load_Properties(Root);
							continue;}
					     
						if (Root->tag == "data" && !Properties.infinite) 
						{    
							std::cerr<<"new Layer \n";
							// Create a new Chunk to store Layer Data
							_Chunk newChunk(Root->parent->AttributesList["width"].toInt(),
										 Root->parent->AttributesList["height"].toInt());
						  	// Parse the new data
							newChunk.load(&Buffer,position); 
                                   Map_Layers.emplace_back(newChunk);
							// Jump to the next Node
							continue;
						}

						if(Root->tag=="chunk" ) // The map is infinite
						{   
							std::cerr<<"new Chunk \n";
							// The infinite map's Chunks have the same Sizes
							_Node *LData= Root->parent->parent;
						     // Create a new Chunk to store Layer Data
						     _Chunk newChunk(LData->AttributesList["width"].toInt(),
										LData->AttributesList["height"].toInt());
							// Parse the new data
						     newChunk.load( &Buffer, position);
							Map_Layers.emplace_back(newChunk);
							// Jump to the next Node
							continue;
						}	
					}
					else
					{
						if (Buffer[position] != '\n' && Buffer[position] != '>')
						{
							Search.push_back(Buffer[position]); // Skip new lines and end tags
						}
						position++; // Read the file
					}
				}
			}
			return Successful; // If the file has loaded correctly
		}
	};

	struct ListClass
	{
		std::string Tag;
		std::vector<std::string> Data;
	};

	struct _Member
	{
		std::map<std::string, _Variable> Keys;
		std::vector<ListClass> List;
		std::vector<_Member> Members;
		std::string Type;
	};

	class JParser
	{

		std::string Buffer;
		unsigned position = 1; // Place in the document
		unsigned lenght;

		_Member _getAttr()
		{
			_Member newM;
			while (Buffer[position] != '}')
			{
				if (Buffer[position] == '"')
				{
					position++;

					std::string Key;
					while (Buffer[position] != '"')
						Key.push_back(Buffer[position++]);

					position += 3;
					if (Buffer[position] == '"')
					{
						std::string Value; // Sore the key value
						position++;
						while (Buffer[position] != '"')
							Value.push_back(Buffer[position++]);
						if (Key == "type")
							newM.Type = Value;

						newM.Keys[Key] = Value;
						position++;

						continue;
					}
					else if (Buffer[position] == '[')
					{

						while (Buffer[position] != ']')
						{
							if (Buffer[position] == '{')
								newM.Members.push_back(_getAttr());
							else
								position++;
						}
						continue;
					}
					else if (Buffer[position] != ' ')
					{
						std::string Value; // Store the key value
						while (Buffer[position] != ' ' && Buffer[position] != ',')
							Value.push_back(Buffer[position++]);

						newM.Keys[Key] = Value;
						continue;
					}
				}
				position++;
			}
			return newM;
		}

	public:
		_Member Root;

		bool read(std::string Json_File, std::map<std::string, _Member> *Class_Properties = nullptr)
		{

			std::ifstream file(Json_File);
			if (file.is_open())
			{
				Buffer = std::string((std::istreambuf_iterator<char>(file)), // The buffer is used to store all data from the file
								 std::istreambuf_iterator<char>());

				std::string Search;	    // Saves text as going through the document
				lenght = Buffer.size(); // Lenght of the XML doc

				while (position < lenght - 1)
				{
					if (Buffer[position] == '{')
						Root.Members.push_back(_getAttr());

					position++;
				}

				if (Class_Properties)
					for (auto &ITEM : Root.Members)
						if (ITEM.Type == "class")
							Class_Properties->operator[](ITEM.Keys["name"].toString()) = ITEM;
				return 1;
			}
			else
			{
				std::cerr << "Failed to load " << Json_File << '\n';
				return 0;
			}
		}
	};

	struct _TileSet
	{
		std::string name;
		std::string image;
		int tilewidth;
		int tileheight;

		_TileSet(_Node *doc)
		{
			for (_Node *node : doc->Children)
				if (node->tag == "image")
				{
					image = node->AttributesList["source"].toString();
					break;
				}
		}
	};

	struct Layer_Data
	{
		std::vector<std::vector<int>> Tile_IDS;
		int columns = 0;
		int rows = 0;
		int x = 0;
		int y = 0;
	};



class TileMapFile
{
	XMLDocument *Map = nullptr;
	JParser *PropertyTypes = nullptr;

	LoadValue NewTileSet(std::string folder, _Node *child)
	{
		XMLDocument newdoc;
		std::string newfile = child->AttributesList["source"].toString();
		LoadValue Result = newdoc.load((folder + newfile).c_str());
		if (Result == FileNotFound)
		{
			std::cerr << folder + newfile << " : Could not be found ";
			return Fail;
		}
		else if (Result == Successful)
		{
			std::cout << "Tileset " << newfile << " : has loaded succesfully \n";
			Data.TileSets.push_back(_TileSet(newdoc.Root->Children[0])); // Load the tilesets
			return Successful;
		}

		return Fail;
	}

	bool is_Class_defined(CAP::_Node *node)
	{
		return Class_Properties.count(node->AttributesList["class"].toString());
	}

public:
	TileMapFile()
	{
		Map = new XMLDocument;
		PropertyTypes = new JParser;
	}

	//<-----------Map Data----------->//
	struct _Map_Data
	{
		std::vector<_TileSet> TileSets;
		std::vector<_Node> ObjectLayers;
		std::vector<_Node> Imagelayers;
	} Data;
	//<-----------Map Data----------->//


	/* Data about all the classes inside "propertytypes.json"
	 Class_Properties["Name of class"]-> Data (json)  */
	std::map<std::string, _Member> Class_Properties;

	bool load(std::string FOLDER, std::string FILE)
	{

		std::cerr << "Loading " + FILE << " ... \n";

		Clock appClock; // Used to get Loading time

		if (PropertyTypes->read(FOLDER + "propertytypes.json", &Class_Properties))
		{
			LoadValue Result = Map->load((FOLDER + FILE).c_str());
			if (Result == Successful)
			{
				_Node *Map_Data = Map->Root->Children[0];
				
				for (_Node *child : Map_Data->Children)
				{
					if (child->tag == "tileset")
					{
						if (!NewTileSet(FOLDER, child))
							return FAIL;
					}

					if (child->tag == "objectgroup") // Save all object layers
					{
						for (auto ITEM : child->Children)
							if (ITEM->tag != "properties")
								if (!is_Class_defined(ITEM))
								{
									std::cerr << "Error : Class '" << ITEM->AttributesList["class"].toString()
											<< "' is undefined";
									return Fail;
								}

						Data.ObjectLayers.push_back(*child);
					}
					if (child->tag == "imagelayer") // Save all image layers
						Data.Imagelayers.push_back(*child);
				}

				std::cout << "Tilemap " << FILE << " :  has loaded succesfully \n";
				std::cout << "Loading time : " << appClock.elapsedTime().asMilliSeconds() << "ms\n";

			}
			else if (Result == FileNotFound)
			{
				std::cout << FOLDER + FILE << " : Could not be found ";
				return Fail;
			}
		}
		else
		{
			delete (PropertyTypes);
			return Fail;
		}
		delete (Map); // The Document is of not use from now on
		delete (PropertyTypes);
		return Successful;
	}
};
}