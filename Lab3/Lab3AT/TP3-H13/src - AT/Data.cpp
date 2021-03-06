///////////////////////////////////////////////////////////////////////////////
//
//	Data.cpp
//
///////////////////////////////////////////////////////////////////////////////
#include "Data.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Constructeur
//
///////////////////////////////////////////////////////////////////////////////
Data::Data( sc_module_name zName )
: sc_module(zName)
{
	//SC_METHOD(thread); 
}


///////////////////////////////////////////////////////////////////////////////
//
//	Destructeur
//
///////////////////////////////////////////////////////////////////////////////
Data::~Data()
{
}


unsigned int Data::GetWord(unsigned int wordPosition)
{
	ifstream readFromFile("../TP3-H13/src - AT/Database.txt",ios::in);

    // Creating a string variable.
    std::string temp;

	int counter = 0;
    while(!readFromFile.eof()){ // I cant figure out what to add here so that the code will find and store the destination.
        getline(readFromFile, temp);
		if(counter == wordPosition)
			break;
		else
			counter++;
    }


	if(temp.length() > 0)
	{	
		wordLength = std::stoi(temp.substr(0, temp.find(" ")));
		word = temp.substr(temp.find(" ")+1, temp.length()-1);
		return wordLength;
	}

	return 0;
}

unsigned char Data::GetChar(unsigned int charPosition)
{
	if(charPosition < word.length())
		return word[charPosition];
	return ' ';
}