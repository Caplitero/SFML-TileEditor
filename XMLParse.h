#pragma once
 
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

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


}
