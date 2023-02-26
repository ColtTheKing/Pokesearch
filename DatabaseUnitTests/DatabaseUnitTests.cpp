#include "pch.h"
#include "CppUnitTest.h"
#include "../DatabaseTest/string_format.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DatabaseUnitTests
{
	TEST_CLASS(StringFormatterUnitTests)
	{
	public:
		TEST_METHOD(TestCapitalize1) {
			std::string input = "a wish upon a star";
			std::string output = "A Wish Upon A Star";
			Assert::AreEqual(StringFormat::Capitalize(input), output);
		}

		TEST_METHOD(TestCapitalize2) {
			std::string input = " a wish upon a star";
			std::string output = " A Wish Upon A Star";
			Assert::AreEqual(StringFormat::Capitalize(input), output);
		}

		TEST_METHOD(TestCapitalize3) {
			std::string input = "a  wish  upon  a  star";
			std::string output = "A  Wish  Upon  A  Star";
			Assert::AreEqual(StringFormat::Capitalize(input), output);
		}

		TEST_METHOD(TestCapitalize4) {
			std::string input = "a$wish upon a star";
			std::string output = "A$wish Upon A Star";
			Assert::AreEqual(StringFormat::Capitalize(input), output);
		}

		TEST_METHOD(TestAllCaps1) {
			std::string input = "a wish upon a star";
			std::string output = "A WISH UPON A STAR";
			Assert::AreEqual(StringFormat::AllCaps(input), output);
		}

		TEST_METHOD(TestAllCaps2) {
			std::string input = " a wish upon a star";
			std::string output = " A WISH UPON A STAR";
			Assert::AreEqual(StringFormat::AllCaps(input), output);
		}

		TEST_METHOD(TestAllCaps3) {
			std::string input = "a  wish  upon  a  star";
			std::string output = "A  WISH  UPON  A  STAR";
			Assert::AreEqual(StringFormat::AllCaps(input), output);
		}

		TEST_METHOD(TestAllCaps4) {
			std::string input = "a$wish upon a star";
			std::string output = "A$WISH UPON A STAR";
			Assert::AreEqual(StringFormat::AllCaps(input), output);
		}

		TEST_METHOD(TestTokenize1) {
			std::string input = "a wish upon a star";
			std::vector<std::string> output = { "a", "wish", "upon", "a", "star" };

			auto result = StringFormat::Tokenize(input, ' ');
			Assert::AreEqual(result, output);
		}

		TEST_METHOD(TestTokenize2) {
			std::string input = "_a_wish_upon_a_star_";
			std::vector<std::string> output = { "a", "wish", "upon", "a", "star" };

			auto result = StringFormat::Tokenize(input, '_');
			Assert::AreEqual(result, output);
		}

		TEST_METHOD(TestTokenize3) {
			std::string input = "  a    wish upon  a star    ";
			std::vector<std::string> output = { "a", "wish", "upon", "a", "star" };

			auto result = StringFormat::Tokenize(input, ' ');
			Assert::AreEqual(result, output);
		}

		TEST_METHOD(TestTokenize4) {
			std::string input = "wowawowwishwowwowuponwowawowstarwowwow";
			std::vector<std::string> output = { "a", "wish", "upon", "a", "star" };

			auto result = StringFormat::Tokenize(input, "wow");
			Assert::AreEqual(result, output);
		}

		TEST_METHOD(TestCombineTokensInQuotes1) {
			std::vector<std::string> input = { "a", "\"wish", "upon", "a", "star\"" };
			std::vector<std::string> output = { "a", "wish upon a star" };

			StringFormat::CombineTokensInQuotes(input, ' ');
			Assert::AreEqual(input, output);
		}

		TEST_METHOD(TestCombineTokensInQuotes2) {
			std::vector<std::string> input = { "a", "\"wish\"", "upon", "a", "star" };
			std::vector<std::string> output = { "a", "wish", "upon", "a", "star" };

			StringFormat::CombineTokensInQuotes(input, ' ');
			Assert::AreEqual(input, output);
		}

		TEST_METHOD(TestCombineTokensInQuotes3) {
			std::vector<std::string> input = { "a", "wi\"sh", "up\"on", "a", "star" };
			std::vector<std::string> output = { "a", "wish upon", "a", "star" };

			StringFormat::CombineTokensInQuotes(input, ' ');
			Assert::AreEqual(input, output);
		}
	};
}

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<>
			static std::wstring ToString<std::vector<std::string>>(const std::vector<std::string>& t) {
				std::string temp = "";
				for (auto str : t)
					temp += str;
				RETURN_WIDE_STRING(temp.c_str());
			}
		}
	}
}