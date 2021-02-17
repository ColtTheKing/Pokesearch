#include "sql_handler.h"

const Command SQLHandler::MATCH_COMMANDS[] = {
		{"dex",	    "dex_num = '*'"},
		{"type",    "(type_1 = '*' OR type_2 = '*')"},
		{"ability", "(ability_1 = '*' OR ability_2 = '*' OR ability_h = '*')"},
};

const Command SQLHandler::STAT_COMMANDS[] = {
		{"max_hp", "hp_max ^ *"},
		{ "max_attack", "atk_max ^ *" },
		{ "max_defense", "def_max ^ *" },
		{ "max_sattack", "satk_max ^ *" },
		{ "max_sdefense", "sdef_max ^ *" },
		{ "max_speed", "speed_max ^ *" },
		{ "base_hp", "hp_base ^ *" },
		{ "base_attack", "atk_base ^ *" },
		{ "base_defense", "def_base ^ *" },
		{ "base_sattack", "satk_base ^ *" },
		{ "base_sdefense", "sdef_base ^ *" },
		{ "base_speed", "speed_base ^ *" },
};

void SQLHandler::PrintStat(const Value& stat) {
	std::cout << std::setw(3) << std::setfill(' ') << stat;
}

void SQLHandler::PrintRow(const Row& row) {
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

void SQLHandler::AddWhereName(std::string& curWherePart, const std::string& name) {
	if (curWherePart.length() == 0)
		curWherePart += " WHERE name LIKE '%" + name + "%'";
	else
		curWherePart += " AND name LIKE '%" + name + "%'";
}

void SQLHandler::AddWhereMatchCommand(std::string& curWherePart, const std::string& cmd, const std::string& param) {
	for (size_t i = 0; i < NUM_MATCH_COMMANDS; i++) {
		bool commandMatches = cmd.compare(MATCH_COMMANDS[i].key) == 0;
		if (commandMatches) {
			std::string statement = MATCH_COMMANDS[i].format;

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

void SQLHandler::AddWhereStatCommand(std::string& curWherePart, const std::string& cmd, const std::string& op, const std::string& param) {
	for (size_t i = 0; i < NUM_STAT_COMMANDS; i++) {
		bool commandMatches = cmd.compare(STAT_COMMANDS[i].key) == 0;
		if (commandMatches) {
			std::string statement = STAT_COMMANDS[i].format;

			// Replace * symbols with the parameter to search with
			size_t paramInd = statement.find('*');
			while (paramInd != std::string::npos) {
				statement.replace(paramInd, 1, param);

				paramInd = statement.find('*');
			}

			// Replace ^ with the operator to search with
			paramInd = statement.find('^');
			while (paramInd != std::string::npos) {
				statement.replace(paramInd, 1, op);

				paramInd = statement.find('^');
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

void SQLHandler::ProcessMatchCommand(std::string& wherePart, const std::string& token) {
	// Split the search criteria into command and parameter components
	std::vector<std::string> bits = StringFormat::Tokenize(token, ':');

	if (bits.size() == 2) {
		AddWhereMatchCommand(wherePart, bits[0], bits[1]);
	}
	else {
		std::cout << "Invalid parameter" << std::endl;
		AddWhereName(wherePart, token);
	}
}

void SQLHandler::ProcessStatCommand(std::string& wherePart, const std::string& token, size_t cmdIdPos) {
	std::string op;

	// Split the search criteria into command, operator, and parameter components
	bool opIsTwoChars = (token[cmdIdPos] == '<' || token[cmdIdPos] == '>')
		&& cmdIdPos + 1 < token.length() && token[cmdIdPos + 1] == '=';
	if (opIsTwoChars) {
		op = token.substr(cmdIdPos, 2);
	}
	else {
		op = token[cmdIdPos];
	}

	std::vector<std::string> bits = StringFormat::Tokenize(token, op);

	if (bits.size() == 2) {
		AddWhereStatCommand(wherePart, bits[0], op, bits[1]);
	}
	else {
		std::cout << "Invalid parameter" << std::endl;
		AddWhereName(wherePart, token);
	}
}

std::string SQLHandler::ConstructSql(const std::string& criteria) {
	std::string selectFromPart =
		"SELECT name, dex_num, type_1, type_2, ability_1, ability_2, ability_h, "
		"hp_base, atk_base, def_base, satk_base, sdef_base, speed_base, "
		"hp_max, atk_max, def_max, satk_max, sdef_max, speed_max "
		"FROM pokemon_search.pokemon";
	std::string wherePart = "";

	// Split the input into separate search criteria
	std::vector<std::string> tokens = StringFormat::Tokenize(criteria, ' ');

	// Recombine tokens within quotation marks
	StringFormat::CombineTokensInQuotes(tokens);

	// Process each search criteria individually
	for (size_t i = 0; i < tokens.size(); i++) {
		// Check if this is a match command using ':'
		size_t cmdIdPos = tokens[i].find(':');
		if (cmdIdPos != std::string::npos) {
			ProcessMatchCommand(wherePart, tokens[i]);
			continue;
		}

		// Check if this is a stat command using one of '=', '>=', '<=', '>', or '<'
		cmdIdPos = tokens[i].find_first_of("=<>");
		if (cmdIdPos != std::string::npos) {
			ProcessStatCommand(wherePart, tokens[i], cmdIdPos);
			continue;
		}

		// If neither if statement fired, then it must be a simple name search
		AddWhereName(wherePart, tokens[i]);
	}

	//std::cout << selectFromPart << wherePart << std::endl;
	return selectFromPart + wherePart;
}

SqlResult SQLHandler::PerformSql(Session& sess, const std::string& criteria) {
	std::string sqlCode = ConstructSql(criteria);

	SqlResult result = sess.sql(sqlCode).execute();

	return result;
}