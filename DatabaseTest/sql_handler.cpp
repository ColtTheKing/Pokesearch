#include "sql_handler.h"

const std::string SQLHandler::DB_NAME = "pokemon_search";
bool SQLHandler::showStats = false;
bool SQLHandler::showAbilities = true;
std::vector<std::string> SQLHandler::joinsDone;

const Command SQLHandler::MATCH_COMMANDS[] = {
	{
		"dex",
		"p.dex_num = '*'",
		"Search for a pokemon by its national dex number"
	},
	{
		"type",
		"(p.type_1 = '*' OR p.type_2 = '*')",
		"Search for pokemon of a certain type"
	},
	{
		"ability",
		"(p.ability_1 = '*' OR p.ability_2 = '*' OR p.ability_h = '*')",
		"Search for pokemon with a certain ability"
	},
	{
		"move",
		"l.move_name = '*'",
		"Search for pokemon with a certain move"
	},
};

const Command SQLHandler::STAT_COMMANDS[] = {
	{
		"base_hp",
		"hp_base ^ *",
		"Search for pokemon with a base hp stat greater than, less than, or equal to a certain number"
	},
	{
		"base_attack",
		"atk_base ^ *",
		"Search for pokemon with a base attack stat greater than, less than, or equal to a certain number"
	},
	{
		"base_defense",
		"def_base ^ *",
		"Search for pokemon with a base defense stat greater than, less than, or equal to a certain number"
	},
	{
		"base_sattack",
		"satk_base ^ *",
		"Search for pokemon with a base special attack stat greater than, less than, or equal to a certain number"
	},
	{
		"base_sdefense",
		"sdef_base ^ *",
		"Search for pokemon with a base special defense stat greater than, less than, or equal to a certain number"
	},
	{
		"base_speed",
		"speed_base ^ *",
		"Search for pokemon with a base speed stat greater than, less than, or equal to a certain number"
	},
};

const Join SQLHandler::JOIN_COMMANDS[] = {
	{ "move", "pokemon", "learnset", "p", "l", "name", "pokemon_name" },
};

void SQLHandler::PrintCommandDescriptions() {
	std::cout << "The following are used by typing the command followed by ':' and the search term:" << std::endl;
	std::cout << "=================================================================================" << std::endl;

	for (size_t i = 0; i < NUM_MATCH_COMMANDS; i++)
		std::cout << "    " << MATCH_COMMANDS[i].cmd << ": " << MATCH_COMMANDS[i].desc << std::endl;

	std::cout << std::endl;
	std::cout << "The following are used by typing the command followed by '>', '>=', '<', '<=', or '=' and the search value:" << std::endl;
	std::cout << "===========================================================================================================" << std::endl;

	for (size_t i = 0; i < NUM_STAT_COMMANDS; i++)
		std::cout << "    " << STAT_COMMANDS[i].cmd << ": " << STAT_COMMANDS[i].desc << std::endl;

	std::cout << std::endl;
	std::cout << "The following commands will change what information is shown about the pokemon found:" << std::endl;
	std::cout << "===========================================================================================================" << std::endl;

	std::cout << "    " << "!showstats: Toggle on the hp, atk, def, spatk, spdef, and speed of the pokemon" << std::endl;
	std::cout << "    " << "!hidestats: Toggle off the hp, atk, def, spatk, spdef, and speed of the pokemon" << std::endl;
	std::cout << "    " << "!showabilities: Toggle on the ability information of the pokemon" << std::endl;
	std::cout << "    " << "!hideabilities: Toggle off the ability information of the pokemon" << std::endl;
	std::cout << "    " << "!quit: Exit the application" << std::endl;
}

void SQLHandler::PrintStat(const string& stat, const Value& value) {
	std::cout << stat << std::setw(3) << std::setfill(' ') << value;
}

void SQLHandler::PrintStats(const Row& row) {
	//base stats
	std::cout << "Base Stats:" << std::endl;
	PrintStat("  Hp: ", row[7]);
	PrintStat("    Atk: ", row[8]);
	PrintStat("   Def: ", row[9]);
	std::cout << std::endl;
	PrintStat("SAtk: ", row[10]);
	PrintStat("   SDef: ", row[11]);
	PrintStat(" Speed: ", row[12]);
	std::cout << std::endl;
}

void SQLHandler::PrintAbilities(const Row& row) {
	//abilities
	std::cout << "Ability: " << StringFormat::Capitalize((std::string)row[4]);
	if (!row[5].isNull())
		std::cout << " / " << StringFormat::Capitalize((std::string)row[5]);
	std::cout << std::endl;
	if (!row[6].isNull())
		std::cout << "Hidden Ability: " << StringFormat::Capitalize((std::string)row[6]) << std::endl;
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
	std::cout << std::endl;

	if (showAbilities)
		PrintAbilities(row);

	if (showStats)
		PrintStats(row);

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
		bool commandMatches = cmd.compare(MATCH_COMMANDS[i].cmd) == 0;
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
		bool commandMatches = cmd.compare(STAT_COMMANDS[i].cmd) == 0;
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

void SQLHandler::AddJoin(std::string& curJoinPart, size_t joinInd) {
	Join j = JOIN_COMMANDS[joinInd];

	// Don't add a join if the same one was already done
	for (size_t i = 0; i < joinsDone.size(); i++)
		if (joinsDone[i] == j.table2)
			return;

	curJoinPart += " INNER JOIN " + DB_NAME + "." + j.table2 + " as " + j.table2Short
		+ " ON " + j.table1Short + "." + j.key1 + " = " + j.table2Short + "." + j.key2;
}

void SQLHandler::ProcessMatchCommand(std::string& joinPart, std::string& wherePart, const std::string& token) {
	// Split the search criteria into command and parameter components
	std::vector<std::string> bits = StringFormat::Tokenize(token, ':');

	// If the command isn't properly constructed print an error
	if (bits.size() != 2) {
		std::cout << "Invalid parameter" << std::endl;
		AddWhereName(wherePart, token);
		return;
	}
	
	for (size_t i = 0; i < NUM_JOIN_COMMANDS; i++) {
		bool commandMatches = bits[0].compare(JOIN_COMMANDS[i].cmd) == 0;
		if (commandMatches)
			AddJoin(joinPart, i);
	}

	AddWhereMatchCommand(wherePart, bits[0], bits[1]);
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
	std::string selectPart =
		"SELECT p.name, p.dex_num, p.type_1, p.type_2, p.ability_1, p.ability_2, p.ability_h, "
		"p.hp_base, p.atk_base, p.def_base, p.satk_base, p.sdef_base, p.speed_base";
	std::string fromPart = " FROM pokemon_search.pokemon as p";
	std::string joinPart = "";
	std::string wherePart = "";

	// Split the input into separate search criteria
	std::vector<std::string> tokens = StringFormat::Tokenize(criteria, ' ');

	// Recombine tokens within quotation marks
	StringFormat::CombineTokensInQuotes(tokens, ' ');

	// Process each search criteria individually
	for (size_t i = 0; i < tokens.size(); i++) {
		// Check if this is a match command using ':'
		size_t cmdIdPos = tokens[i].find(':');
		if (cmdIdPos != std::string::npos) {
			ProcessMatchCommand(joinPart, wherePart, tokens[i]);
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
	return selectPart + fromPart + joinPart + wherePart;
}

SqlResult SQLHandler::PerformSql(Session& sess, const std::string& criteria) {
	std::string sqlCode = ConstructSql(criteria);

	// Clear the list of joins for the next query
	joinsDone.clear();

	SqlResult result = sess.sql(sqlCode).execute();

	return result;
}