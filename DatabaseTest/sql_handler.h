#pragma once
#include <mysqlx/xdevapi.h>
#include <iomanip>
#include <iostream>
#include <string>
#include "string_format.h"

using namespace mysqlx;

struct Command {
	std::string key;
	std::string format;
};

class SQLHandler {
private:
	static void PrintStat(const Value& stat);
	static void PrintStats(const Row& row);
	static void PrintAbilities(const Row& row);
	static void AddWhereName(std::string& curWherePart, const std::string& name);
	static void AddWhereMatchCommand(std::string& curWherePart, const std::string& cmd, const std::string& param);
	static void AddWhereStatCommand(std::string& curWherePart, const std::string& cmd, const std::string& op, const std::string& param);
	static void ProcessMatchCommand(std::string& wherePart, const std::string& token);
	static void ProcessStatCommand(std::string& wherePart, const std::string& token, size_t cmdIdPos);
	static std::string ConstructSql(const std::string& criteria);
public:
	const static Command MATCH_COMMANDS[];
	const static size_t NUM_MATCH_COMMANDS = 3;
	const static Command STAT_COMMANDS[];
	const static size_t NUM_STAT_COMMANDS = 12;

	static bool showStats, showAbilities;

	static void PrintRow(const Row& row);
	static SqlResult PerformSql(Session& sess, const std::string& criteria);
};