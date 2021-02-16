#include "string_format.h"

//Capitalizes the first letter of each word in the string
std::string StringFormat::Capitalize(const std::string& str) {
	std::string capitalized = str;

	if (capitalized.length() == 0)
		return capitalized;

	//capitalized the first letter
	capitalized[0] = toupper(capitalized[0]);

	//capitalize the first letter of any subsequent words
	for (size_t i = 0; i < capitalized.length(); i++) {
		//if there's a space, capitalize the next letter
		size_t nextInd = i + 1;
		bool shouldCapitalize = capitalized[i] == ' ' && nextInd < capitalized.length();
		if (shouldCapitalize)
			capitalized[nextInd] = toupper(capitalized[nextInd]);
	}

	return capitalized;
}

//Capitalizes all letters in the string
std::string StringFormat::AllCaps(const std::string& str) {
	std::string capitalized = str;

	for (size_t i = 0; i < capitalized.length(); i++)
		capitalized[i] = toupper(capitalized[i]);

	return capitalized;
}

//Splits the string into tokens based on the delimiter and then returns the tokens
std::vector<std::string> StringFormat::Tokenize(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	bool foundToken = false;
	size_t tokenStart;

	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == delimiter) {
			//if we have reached the first delimiter after the token
			if (foundToken) {
				//add a new token to the list
				tokens.push_back(str.substr(tokenStart, i - tokenStart));
				foundToken = false;
			}
		}
		else if (!foundToken) {
			tokenStart = i;
			foundToken = true;
		}
	}

	//if the string finished on a token, add it as well
	if (foundToken)
		tokens.push_back(str.substr(tokenStart, str.length() - tokenStart));

	return tokens;
}