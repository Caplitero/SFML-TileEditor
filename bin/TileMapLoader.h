#pragma once

#include <iostream>
#include <string> 
#include <vector>
#include <fstream>




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

	class _Node {

		struct XMLAttribute
		{
			std::string key;    // Name of the Attribute
			std::string value; // Value of the key
		};

	public:

		std::string               tag;               // Tag of the Node
		std::string               inner_Text;       // Inner text of the node
		class _Node* parent;          // Point to the upper node
		std::vector<XMLAttribute> AttributesList; // This vector contains all the attributes from the node
		std::vector<_Node*>       Children;      // This vector contains pointers to all lower nodes

		bool replace_keyValue(const char* Key, const char* newValue)
		{
			for (XMLAttribute& attr : AttributesList)
				if (attr.key == Key) { attr.value = newValue; return 1; }
			return 0;
		}

		std::string find_keyValue(const char* Key)
		{

			for (XMLAttribute& attr : AttributesList)
				if (attr.key == Key) { return attr.value; break; }

			return "\0";
		}


		int find_attrs(std::string Buffer, unsigned& position)  // Goes through the node and get Attributes
		{
			XMLAttribute New_Attribute; // Load the new Attribute
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
					New_Attribute.key = Search;  // = marks the end of a key

					Search.clear();   // Clear vector for the next data input
					position += 2;     // The value starts on the next position

					if (Buffer[position - 1] == '"')
						while (Buffer[position] != '"')
							Search.push_back(Buffer[position++]); // Read the value 
					else return false;  // False because the file is incomplete

					New_Attribute.value = Search; // Set new value
					Search.clear();              // Clear vector for the next data input

					this->AttributesList.push_back(New_Attribute); // Add the new Attribute to the node's list

					New_Attribute.key.clear();       // Clear the key for next attribute
					New_Attribute.value.clear();    // Clear the value for next atttribute
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

    struct ListClass {
		std::string Tag;
		std::vector<std::string> Data;
	};

	struct Keys {
		std::string key="\0";
		std::string value="\0";
	};


	struct _Member
	{
		std::vector<Keys> Keys;
		std::vector<ListClass> List;
		std::vector<_Member> Members;
		std::string  Type;
		std::string find_keyValue(std::string KEY)
		{
			for (auto& Data : Keys)
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
					
					Keys newKeys;
					while (Buffer[position] != '"')
					{
						
						newKeys.key.push_back(Buffer[position++]);
						
					}
					
						position += 3;
						if (Buffer[position] == '"')
						{
							position++;
							while (Buffer[position] != '"')
								newKeys.value.push_back(Buffer[position++]);
							if (newKeys.key == "type")
							{
								newM.Type = newKeys.value;
							}
							newM.Keys.push_back(newKeys);
							position++;
							
							continue;
						}
						else if (Buffer[position] == '[')
						{
							
							
							while (Buffer[position] != ']')
							{
								
								
								if (Buffer[position] == '{')
								{
									newM.Members.push_back(_getAttr());
								}
								else position++;
								
							}
							
							continue;
						}
						else if (Buffer[position] != ' ')
						{
						
							while (Buffer[position] != ' ' &&  Buffer[position] != ',')
							{
								
								newKeys.value.push_back(Buffer[position++]);
								
							}
							
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
				{
					if (ITEM.Type == "class")
					{
						for (auto& PROPERTIES : ITEM.Members)
						{
							if (PROPERTIES.find_keyValue("name") == "Class_ID")
							{
								Class_IDs[atoi(PROPERTIES.find_keyValue("value").c_str())]
									= ITEM.find_keyValue("name");
							}

						}
					}
				}

				return 1;
			}
			else { 
				std::cerr << "Failed to load" << Json_File << '\n';
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
		JParser* PropertyTypes = nullptr;

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
		
		std::string get_CLASS_ID(CAP::_Node * node) {
			
			std::string CLASS = node->find_keyValue("class");
			if (!CLASS.empty()) {
				for (int i = 0; i < PropertyTypes->Class_IDs.size(); i++)
					if (PropertyTypes->Class_IDs[i] == CLASS)
						return std::to_string(i);
			}
			return "\0";

		}

		bool load(std::string FOLDER, std::string FILE)
		{
			PropertyTypes = new JParser;
			if (PropertyTypes->read("propertytypes.json"))
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

							for (auto ITEM : child->Children)
							{
								if (ITEM->tag != "properties")
								{
									std::string ID = get_CLASS_ID(ITEM);
									if (!ID.empty())
										ITEM->replace_keyValue("class", ID.c_str());
									else
									{
										std::cerr << "Error : Class '" << ITEM->find_keyValue("class") << "' is undefined";
										return 0;
									}
								}

								
							}

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

			}
			else {
				return 0;
				delete(PropertyTypes);
			}
			delete(Map);           // The Document is of not use from now on 
			delete(PropertyTypes); 
			return 1;

		}


	};

}