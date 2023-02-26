#pragma once
#include <mysqlx/xdevapi.h>
#include <iomanip>
#include <iostream>
#include <string>
#include "string_format.h"

using namespace mysqlx;

struct Command {
	std::string cmd;
	std::string format;
	std::string desc;
};

struct Join {
	std::string cmd;
	std::string table1;
	std::string table2;
	std::string table1Short;
	std::string table2Short;
	std::string key1;
	std::string key2;
};

class SQLHandler {
private:
	static void PrintStat(const string& stat, const Value& value);
	static void PrintStats(const Row& row);
	static void PrintAbilities(const Row& row);
	static void AddWhereName(std::string& curWherePart, const std::string& name);
	static void AddWhereMatchCommand(std::string& curWherePart, const std::string& cmd, const std::string& param);
	static void AddWhereStatCommand(std::string& curWherePart, const std::string& cmd, const std::string& op, const std::string& param);
	static void AddJoin(std::string& curJoinPart, size_t joinInd);
	static void ProcessMatchCommand(std::string& joinPart, std::string& wherePart, const std::string& token);
	static void ProcessStatCommand(std::string& wherePart, const std::string& token, size_t cmdIdPos);
	static std::string ConstructSql(const std::string& criteria);
public:
	const static Command MATCH_COMMANDS[];
	const static size_t NUM_MATCH_COMMANDS = 4;
	const static Command STAT_COMMANDS[];
	const static size_t NUM_STAT_COMMANDS = 6;
	const static Join JOIN_COMMANDS[];
	const static size_t NUM_JOIN_COMMANDS = 1;
	const static std::string DB_NAME;

	static bool showStats, showAbilities;
	static std::vector<std::string> joinsDone;

	static void PrintCommandDescriptions();
	static void PrintRow(const Row& row);
	static SqlResult PerformSql(Session& sess, const std::string& criteria);
};