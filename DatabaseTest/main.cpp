#include <mysqlx/xdevapi.h>
#include <iostream>
#include <string>
#include <iomanip>
#include "string_format.h"

using namespace mysqlx;

struct Command {
	std::string key;
	std::string format;
};

Command COMMANDS[] = {
	{"dex",	    "dex_num = '*'"},
	{"type",    "(type_1 = '*' OR type_2 = '*')"},
	{"ability", "(ability_1 = '*' OR ability_2 = '*' OR ability_h = '*')"}
};
size_t NUM_COMMANDS = 3;

Session StartSession(std::string password) {
	try {
		Session sess(33060, "root", password);
		return sess;
	}
	catch (const Error & err) {
		std::cout << "The database session could not be opened: " << err << std::endl;
		exit(1);
	}
}

Schema MakeSchema(Session& sess) {
	try {
		Schema db = sess.getSchema("pokemon_search");
		return db;
	}
	catch (const Error & err) {
		std::cout << "The schema could not be opened: " << err << std::endl;
		exit(1);
	}
}

void PrintStat(const Value& stat) {
	std::cout << std::setw(3) << std::setfill(' ') << stat;
}

void PrintRow(const Row& row) {
	std::cout << "================================" << std::endl;

	//dex number and name
	std::cout << std::resetiosflags(std::ios::adjustfield);
	std::cout << std::setiosflags(std::ios::left);
	std::cout << std::setw(25) << std::setfill(' ') << StringFormat::AllCaps((std::string)row[0]);
	std::cout << std::resetiosflags(std::ios::adjustfield);
	std::cout << std::setiosflags(std::ios::right);
	std::cout << " No." << std::setw(3) << std::setfill('0') << row[1] << std::endl;

	//types
	std::cout << "Type: " << StringFormat::Capitalize((std::string)row[2]);
	if (!row[3].isNull())
		std::cout << " / " << StringFormat::Capitalize((std::string)row[3]);
	
	//abilities
	std::cout << std::endl << "Ability: " << StringFormat::Capitalize((std::string)row[4]);
	if (!row[5].isNull())
		std::cout << " / " << StringFormat::Capitalize((std::string)row[5]);
	std::cout << std::endl;
	if (!row[6].isNull())
		std::cout << "Hidden Ability: " << StringFormat::Capitalize((std::string)row[6]) << std::endl;

	//base stats
	std::cout << "Base Stats:" << std::endl << "  Hp: ";
	PrintStat(row[7]);
	std::cout << "    Atk: ";
	PrintStat(row[8]);
	std::cout << "   Def: ";
	PrintStat(row[9]);
	std::cout << std::endl << "SAtk: ";
	PrintStat(row[10]);
	std::cout << "   SDef: ";
	PrintStat(row[11]);
	std::cout << "   Spd: ";
	PrintStat(row[12]);
	std::cout << std::endl;

	//max stats
	std::cout << "Max Stats:" << std::endl << "  Hp: ";
	PrintStat(row[13]);
	std::cout << "    Atk: ";
	PrintStat(row[14]);
	std::cout << "   Def: ";
	PrintStat(row[15]);
	std::cout << std::endl << "SAtk: ";
	PrintStat(row[16]);
	std::cout << "   SDef: ";
	PrintStat(row[17]);
	std::cout << "   Spd: ";
	PrintStat(row[18]);
	std::cout << std::endl;

	std::cout << "================================" << std::endl;
}

void AddWhereName(std::string& curWherePart, const std::string& name) {
	if (curWherePart.length() == 0)
		curWherePart += " WHERE name LIKE '%" + name + "%'";
	else
		curWherePart += " AND name LIKE '%" + name + "%'";
}

void AddWhereCommand(std::string& curWherePart, const std::string& cmd, const std::string& param) {
	for (size_t i = 0; i < NUM_COMMANDS; i++) {
		bool commandMatches = cmd.compare(COMMANDS[i].key) == 0;
		if (commandMatches) {
			std::string statement = COMMANDS[i].format;

			size_t paramInd = statement.find('*');

			while (paramInd != std::string::npos) {
				statement.replace(paramInd, 1, param);

				paramInd = statement.find('*');
			}

			if (curWherePart.length() == 0)
				curWherePart += " WHERE " + statement;
			else
				curWherePart += " AND " + statement;
			return;
		}
	}

	std::cout << "No matching command" << std::endl;
	curWherePart += " WHERE name LIKE '%:%'";
}

std::string ConstructSql(const std::string& criteria) {
	std::string selectFromPart =
		"SELECT name, dex_num, type_1, type_2, ability_1, ability_2, ability_h, "
		"hp_base, atk_base, def_base, satk_base, sdef_base, speed_base, "
		"hp_max, atk_max, def_max, satk_max, sdef_max, speed_max "
		"FROM pokemon_search.pokemon";
	std::string wherePart = "";

	// Split the input into separate search criteria
	std::vector<std::string> tokens = StringFormat::Tokenize(criteria, ' ');

	for (size_t i = 0; i < tokens.size(); i++) {
		//std::cout << tokens[i] << std::endl;

		// Check if this is a command or if it's just text to search by name
		if (tokens[i].find(':') != std::string::npos) {
			// Split the search criteria into commandand parameter components
			std::vector<std::string> bits = StringFormat::Tokenize(tokens[i], ':');

			if (bits.size() == 2) {
				// If there is a quotation mark at the start of the parameter
				if (bits[1][0] == '"') {
					bits[1].erase(0, 1);

					// Group together consecutive tokens until the end quote is found
					while (bits[1][bits[1].length() - 1] != '"') {
						if (++i < tokens.size()) {
							bits[1] += " " + tokens[i];
						}
						else {
							std::cout << "End quotation was not found" << std::endl;
							break;
						}
					}

					if (bits[1][bits[1].length() - 1] == '"')
						bits[1].erase(bits[1].length() - 1, 1);
				}

				AddWhereCommand(wherePart, bits[0], bits[1]);
			}
			else {
				std::cout << "Parameter " << i << " is invalid" << std::endl;
				AddWhereName(wherePart, tokens[i]);
			}
		}
		else {
			AddWhereName(wherePart, tokens[i]);
		}
	}

	return selectFromPart + wherePart;
}

SqlResult PerformSql(Session& sess, const std::string& criteria) {
	std::string sqlCode = ConstructSql(criteria);

	SqlResult result = sess.sql(sqlCode).execute();

	return result;
}

int main() {
	std::cout << "Please enter the database password: ";
	std::string password;
	getline(std::cin, password);

	Session sess = StartSession(password);
	Schema db = MakeSchema(sess);

	try {
		Table myTable = db.getTable("pokemon");

		std::string search;

		while (true) {
			std::cout << "Enter search criteria: ";
			getline(std::cin, search);

			if (search.compare("quit") == 0)
				break;

			SqlResult result = PerformSql(sess, search);

			if (result.count() == 0)
				std::cout << "NO RESULTS WERE FOUND" << std::endl << std::endl;
			else {
				Row row;
				while ((row = result.fetchOne())) {
					PrintRow(row);
					std::cout << std::endl;
				}
			}
		}

		exit(0);
	}
	catch (const Error & err) {
		std::cout << "An error occurred: " << err << std::endl;
		exit(1);
	}
}