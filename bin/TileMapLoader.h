#pragma once

#include <iostream>
#include <string> 
#include <vector>
#include <fstream>
#include <chrono>
#include <map>

namespace CAP {
    
    enum {
		FAIL,     // Node is bad 
		MULTIPLE, // Node is structured on multiple levels
		SINGLE   // Node is structured on a single line 
	};

	enum  LoadValue {
		Fail,
		Successful,
		FileNotFound

	};
   
    // Basic Clock to get elapsed time
    class Clock
	{std::chrono::system_clock::time_point _STARTING_POINT;
    public:
	Clock(){_STARTING_POINT= std::chrono::system_clock::now();}
		 // Return the elapsed time in milliseconds 
		 inline int elapsedTime(){  
			return std::chrono::duration_cast<std::chrono::milliseconds>
			      (std::chrono::system_clock::now() -_STARTING_POINT).count();}};
    
    // Struct used for easier manipulation of Data
     struct _Variable
	{
	     _Variable(std::string value)
		{_value = value;}
		_Variable(){_value="\0";}
		int toInt() {return atoi(_value.c_str());}
		float toDouble(){ return atof(_value.c_str()); } 
		std::string toString(){ return _value;}
	protected:
		std::string _value;
	};
	struct  _Node {
		std::map<std::string,_Variable> AttributesList; // This map contains all the keys and values of the node
		std::string               tag;           // Tag of the Node
		std::string               inner_Text;    // Inner text of the node
		class _Node*              parent;        // Point to the upper node
		std::vector<_Node*>       Children;      // This vector contains pointers to all lower nodes		

        // Goes through the node and gets all Attributes
		int find_attrs(std::string Buffer, unsigned& position)  
		{
			std::string  Search;       // Attribute data parser
			position++;

			while (Buffer[position] != '>')
			{
				if (Buffer[position] != ' ' && Buffer[position] != '"')
					Search.push_back(Buffer[position]); // Read data
				position++;                        // Go to next character

				 if (Buffer[position] == ' '  && tag.empty())
				{					
					tag = Search;   // Add node tag
					position++;          // The data ends on the next position
					Search.clear();     // Clear vector for the next data input
				}
				 
				 else if (Buffer[position + 1] == '>' && Buffer[position] != '?' && tag.empty()) // Nodes with no keys
				 {
					 Search.push_back(Buffer[position]);
					 tag = Search;        // Add node tag
					 position++;          // The data ends on the next position
					 Search.clear();     // Clear vector for the next data input
				 }

				if (Buffer[position] == '=')
				{
					std::string key = Search;  // = marks the end of a key

					Search.clear();   // Clear vector for the next data input
					position += 2;     // The value starts on the next position

					if (Buffer[position - 1] == '"')
						while (Buffer[position] != '"')
							Search.push_back(Buffer[position++]); // Read the value 
					else return false;  // False because the file is incomplete
                         
					this->AttributesList[key]=_Variable(Search); // Add the new Attribute to the node's list
					Search.clear();              // Clear vector for the next data input
                         
					continue;
				}

				if (Buffer[position] == '/' && Buffer[position + 1] == '>')
				{
					position++;
					return SINGLE;  // Return that the node is on a single line (just one end)}
				}
			}
			return MULTIPLE;
		}
	};

	class XMLDocument
	{


	private:
		_Node* newChild(_Node* parent)
		{
			_Node* child = new _Node;   // Create a new node
			child->parent = parent;     // Specify upper level
			parent->Children.push_back(child); // Add the child to the parents vector 
			return child; // Return the node
		}
	public:

		std::string version;   // Version of the XML File
		std::string encoding; // Type of Encoding
		_Node* Root;    // This is the xml root (<?xml?>)

		LoadValue load(const char* path) {

			std::ifstream file(path); // Load the XML Document
			if (!file.is_open())
				return FileNotFound; // Failed to load

			else    // If the load didnt fail , get data
			{
				std::string Buffer((std::istreambuf_iterator<char>(file)),   // The buffer is used to store all data from the file
					std::istreambuf_iterator<char>());  // Convert data from the file into char and append it to the buffer

				unsigned     position = 0;      // Place in the document
				std::string  Search;           // Saves text as going through the document
				unsigned     lenght = Buffer.size();    // Lenght of the XML doc
				Root = new _Node;

				while (position < lenght)  // Go through the document
				{
					if (Buffer[position] == '<')  // New node
					{
						if (!Search.empty())
						{
							if (Search != " ")
								Root->inner_Text.append(Search + " "); // If the node has any data outside of tag , we save it
							Search.clear(); // Clear vector for next data imput
						}

						if (Buffer[position + 1] == '!' && Buffer[position + 2] == '-' ) // Check for comments
						{   
							while (Buffer[position] != '>')position++;   // Comments are useless for the program so it will be parsed but ignored
							continue;    // Node done , search for the next one , breaking the if 
						}

						if (Buffer[position + 1] == '/')
						{
							position += 2;    //  '/' marks the end of a node , and the name start 2 spaces later
							while (Buffer[position] != '>')
								Search.push_back(Buffer[position++]);
							
							
							// Read the end tag found
							
							if (Root->tag != Search )    // Eevery node contains a Start_node 
							{
								std::cout << Search << " Tag missmatch" << Root->tag << "\n";
								return Fail;
							}   // and a End_node which must be equal
							Root = Root->parent;
							position++;
							Search.clear();
							continue;   // Must skip to the next Search
						}

						if (Buffer[position + 1] == '?')
						{

							while (Buffer[position] != ' ' && Buffer[position] != '>')
								Search.push_back(Buffer[position++]);   // Save to the vector the new found text

							if (Search == "<?xml")   // Root of the XML Document used to store version and encoding
							{
								_Node Description;   // A node to store data 

								Description.find_attrs(Buffer, position); // Get all data from the root
								Root->AttributesList = Description.AttributesList;
								Root->tag = "xml";
								Search.clear();  // Clear the vector for new data imput
								position++; // Continue the search
								continue;  // Skip to the next node
							}
						}

						Root = newChild(Root); // Search if the node has any child

						if (Root->find_attrs(Buffer, position) == SINGLE)
						{ // Some Nodes are on a single line and dont have children
							
							Root = Root->parent; // If the Node is inline we return to it's parent
							
							position++;  // Continue the search 
							continue;   // Skip to the next node
						}

						if (Root->tag.empty())
						{
							Root->tag = Search;
							position++;
							continue;
						}

					}
					else {
						if (Buffer[position] != '\n' && Buffer[position] != '>')
							Search.push_back(Buffer[position]);// Skip new lines and end tags 
						position++;   // Read the file

					}
				}
			}
			return Successful; // If the file has loaded correctly
		}

	};

    // Used to save a key and it's value <key="value"> 
    struct _KEY_STRUCT
		{
		std::string key   ="\0";   // Name of the Attribute
		std::string value ="\0";   // Value of the key
		};

    struct ListClass {
		std::string Tag;
		std::vector<std::string> Data;
	};

	struct _Member
	{
		std::vector<_KEY_STRUCT> Keys;
		std::vector<ListClass> List;
		std::vector<_Member> Members;
		std::string  Type;
		std::string find_keyValue(std::string KEY)
		{
			for (_KEY_STRUCT& Data : Keys)
				if (Data.key == KEY)
					return Data.value;
			return "\0";
		}

	};

	class JParser {

		std::string Buffer;
		unsigned     position = 1;  // Place in the document
		unsigned     lenght;
		
		_Member _getAttr()
		{
			_Member newM;
			while (Buffer[position] != '}')
			{
				if (Buffer[position] == '"')
				{
					position++;
					
					_KEY_STRUCT newKeys;
					while (Buffer[position] != '"')
						newKeys.key.push_back(Buffer[position++]);
					
					position += 3;
					if (Buffer[position] == '"')
					{
						position++;
						while (Buffer[position] != '"')
						    newKeys.value.push_back(Buffer[position++]);
						if (newKeys.key == "type")
							newM.Type = newKeys.value;
							
							newM.Keys.push_back(newKeys);
							position++;
							
							continue;
						}
						else if (Buffer[position] == '[')
						{
							
							while (Buffer[position] != ']')
							{		
								if (Buffer[position] == '{')
									newM.Members.push_back(_getAttr());
								else position++;
							}
							continue;
						}
						else if (Buffer[position] != ' ')
						{
						
							while (Buffer[position] != ' ' &&  Buffer[position] != ',')								
								newKeys.value.push_back(Buffer[position++]);
							
							newM.Keys.push_back(newKeys);
							continue;
						}
				}
				position++;
			}
			return newM;
		 }



	public:
		
		_Member Root;
		std::vector<std::string> Class_IDs;

		bool read(std::string Json_File)
		{
			Class_IDs.resize(64);
			std::ifstream file(Json_File);
			if (file.is_open()) {
				 Buffer = std::string((std::istreambuf_iterator<char>(file)), // The buffer is used to store all data from the file
					std::istreambuf_iterator<char>());
				
				
				std::string  Search;       // Saves text as going through the document
				lenght = Buffer.size();    // Lenght of the XML doc
				
				

				while (position < lenght-1 )
				{
					if (Buffer[position] == '{')
						Root.Members.push_back(_getAttr());
					
				   position++;
				}

				for (auto &ITEM : Root.Members)
					if (ITEM.Type == "class")
						for (auto& PROPERTIES : ITEM.Members)
							if (PROPERTIES.find_keyValue("name") == "Class_ID")
							{
								Class_IDs[atoi(PROPERTIES.find_keyValue("value").c_str())]
									= ITEM.find_keyValue("name");
							}
				return 1;
			}
			else { 
				std::cerr << "Failed to load " << Json_File << '\n';
				return 0; }
		}

	};


	struct _TileSet
	{   
		std::string  name;
		std::string  image;
		int          tilewidth;
		int          tileheight;

		_TileSet(_Node* doc)
		{
			for (_Node* node : doc->Children)
				if (node->tag == "image")
				{
					image = node->AttributesList["source"].toString();
					break;
				}
		}
	};

	class _TileLayer {
    
    std::vector<std::vector<int>> read_InnerText(std::string Text, unsigned _Width, unsigned _Height)
	{   
		
		std::vector<std::vector<int>> Data(_Height,std::vector<int>(_Width));

		int position = 0;int I=0;int J=0;  std::string number;
			while (Text[position] != '\0')
			{
				if (Text[position] == ',' || Text[position + 1] == '\0')
				{
				    Data[I][J++]=(stoi(number));
					number.clear();
					I+=J/_Width;
					J=J%_Width;
				}
				else number = number + Text[position];
				position++;
			}
		return Data;
	}

	struct Layer_Data{
		std::vector<std::vector<int>> Tile_IDS;
		int columns =0;
		int rows =0;
		int x=0;
		int y=0;
	};
    public:
		int id=0;
		std::string Name;
		std::vector<Layer_Data> Chunks;

		_TileLayer(_Node node,bool Type)
		{
		  id   = node.AttributesList["id"].toInt();
            Name = node.AttributesList["name"].toString();
            if( Type )
			{
             for(auto& Chunk: node.Children[0]->Children)
			 {  
				Layer_Data newChunk;
				newChunk.x = Chunk->AttributesList["x"].toInt();
				newChunk.y = Chunk->AttributesList["y"].toInt();
				newChunk.columns  = Chunk->AttributesList["width"].toInt();
			    newChunk.rows     = Chunk->AttributesList["height"].toInt();
				newChunk.Tile_IDS = read_InnerText(Chunk->inner_Text,
				                                   newChunk.columns,newChunk.rows);
				Chunks.push_back(newChunk);
			 }
			}
			else 
			{
            Chunks.resize(1);
			Chunks[0].columns  = node.AttributesList["width"].toInt();
			Chunks[0].rows     = node.AttributesList["height"].toInt();
			Chunks[0].Tile_IDS =read_InnerText(node.Children[0]->inner_Text,
			                                   Chunks[0].columns,Chunks[0].rows);
			}
		}
	};


	class TileMapFile
	{
		XMLDocument* Map = nullptr;
		JParser* PropertyTypes = nullptr;
        
        LoadValue NewTileSet(std::string folder,_Node* child)
		{
            XMLDocument newdoc; std::string newfile = child->AttributesList["source"].toString();
			LoadValue Result = newdoc.load((folder + newfile).c_str());
			if (Result == FileNotFound)
			{std::cerr << folder + newfile << " : Could not be found "; return Fail;}
	
			else if (Result == Successful)
			{std::cout << "Tileset " << newfile << " : has loaded succesfully \n"; 
		    Data.TileSets.push_back(_TileSet(newdoc.Root->Children[0])); // Load the tilesets 
			return Successful;}

			return Fail;
		}
		std::string get_CLASS_ID(CAP::_Node * node) {
			
			std::string CLASS = node->AttributesList["class"].toString();
			if (!CLASS.empty()) {
				for (int i = 0; i < PropertyTypes->Class_IDs.size(); i++)
					if (PropertyTypes->Class_IDs[i] == CLASS)
						return std::to_string(i);
			}
			return "\0";
		}

	public:
		TileMapFile()
		{Map = new XMLDocument;
		PropertyTypes = new JParser;}
        
		//<-----------Map Data----------->//
		struct _Map_Data{
		std::vector<_TileSet>   TileSets;
		std::vector<_TileLayer> TileLayers;
		std::vector<_Node> ObjectLayers;
		std::vector<_Node> Imagelayers;}Data;
        //<-----------Map Data----------->//

        //<-------Map properties------->//
        struct _Map_Properties{
		std::string Orientation= "\0";
		int Tile_Width  = 0;
		int Tile_Height = 0;
		bool infinite   = 0;
		int width  =0;
		int height =0;
		}Properties;
		//<-------Map properties------->//

		bool load(std::string FOLDER, std::string FILE)
		{   
			
			std::cerr<<"Loading "+FILE<<" ... \n";
			
			Clock appClock; // Used to get Loading time
               
			if (PropertyTypes->read(FOLDER+"propertytypes.json"))
			{   LoadValue Result = Map->load((FOLDER + FILE).c_str()) ;
				if (Result == Successful) { 
					
					Properties.infinite  = Map->Root->Children[0]->AttributesList["infinite"].toInt();
					
					for (_Node* child : Map->Root->Children[0]->Children)
					{    
						if (child->tag == "tileset") 
							{if(!NewTileSet(FOLDER , child))return FAIL;}
                              
						if (child->tag == "objectgroup")    // Save all object layers
						{

							for (auto ITEM : child->Children)
							{
								if (ITEM->tag != "properties")
								{
									std::string ID = get_CLASS_ID(ITEM);
									if (ID.empty())
										{std::cerr << "Error : Class '" <<
										   ITEM->AttributesList["class"].toString() 
										    << "' is undefined";
										return Fail;}	
								}
							}
							Data.ObjectLayers.push_back(*child);
								     
						}
						if (child->tag == "imagelayer")    // Save all image layers
							Data.Imagelayers.push_back(*child);
						if (child->tag == "layer")
							Data.TileLayers.push_back(_TileLayer(*child,Properties.infinite));
					}
					
					std::cout << "Tilemap " << FILE << " :  has loaded succesfully \n";
                    std::cout<<"Loading time : "<<appClock.elapsedTime()<<"ms\n";
					Properties.width       = Map->Root->Children[0]->AttributesList["width"].toInt();
					Properties.height      = Map->Root->Children[0]->AttributesList["height"].toInt();
					Properties.Tile_Width  = Map->Root->Children[0]->AttributesList["tilewidth"].toInt();
					Properties.Tile_Height = Map->Root->Children[0]->AttributesList["tileheight"].toInt();
					Properties.Orientation = Map->Root->Children[0]->AttributesList["orientation"].toInt();
					
				}
				else if (Result == FileNotFound) {
					std::cout << FOLDER + FILE << " : Could not be found "; return Fail;
				}
			}
			else {
				delete(PropertyTypes); return Fail; 
			}
			delete(Map);           // The Document is of not use from now on 
			delete(PropertyTypes); 
			return Successful;

		}


	};

}