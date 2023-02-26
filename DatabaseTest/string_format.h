#pragma once
#include <string>
#include <vector>

class StringFormat {
public:
	static std::string Capitalize(const std::string& str);
	static std::string AllCaps(const std::string& str);
	static std::vector<std::string> Tokenize(const std::string& str, char delimiter);
	static std::vector<std::string> Tokenize(const std::string& str, const std::string& delimiter);
	static void CombineTokensInQuotes(std::vector<std::string>& tokens, char delimiter);
};