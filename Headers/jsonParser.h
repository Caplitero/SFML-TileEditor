#pragma once

#ifndef JSON_PARS_H
#define JSON_PARS_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

namespace JCAP {


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

		bool read(std::string Json_File)
		{
			std::ifstream file(Json_File);
			if (file.is_open()) {
				 Buffer = std::string((std::istreambuf_iterator<char>(file)), // The buffer is used to store all data from the file
					std::istreambuf_iterator<char>());
				
				
				std::string  Search;                    // Saves text as going through the document
				lenght = Buffer.size();    // Lenght of the XML doc
				
				

				while (position < lenght-1 )
				{
					if (Buffer[position] == '{')
						Root.Members.push_back(_getAttr());
					
				   position++;
				}
				return 1;
			}
			else return 0;
		}

	};
}


#endif // JSON_PARS_H

