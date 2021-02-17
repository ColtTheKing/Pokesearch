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
	size_t tokenStart = 0;

	while (tokenStart < str.length()) {
		size_t nextDelim = str.find(delimiter, tokenStart);

		if (nextDelim == std::string::npos)
			break;

		tokens.push_back(str.substr(tokenStart, nextDelim - tokenStart));

		tokenStart = nextDelim + 1;
	}

	// Add the last token
	tokens.push_back(str.substr(tokenStart, str.length() - tokenStart));

	return tokens;
}

//Splits the string into tokens based on the delimiter and then returns the tokens
std::vector<std::string> StringFormat::Tokenize(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t tokenStart = 0;

	while (tokenStart < str.length()) {
		size_t nextDelim = str.find(delimiter, tokenStart);

		if (nextDelim == std::string::npos)
			break;

		tokens.push_back(str.substr(tokenStart, nextDelim - tokenStart));

		tokenStart = nextDelim + 1;
	}

	// Add the last token
	tokens.push_back(str.substr(tokenStart, str.length() - tokenStart));

	return tokens;
}