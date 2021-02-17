#include <mysqlx/xdevapi.h>
#include <iostream>
#include <string>
#include "sql_handler.h"

using namespace mysqlx;

static Session StartSession(std::string password) {
	try {
		Session sess(33060, "root", password);
		return sess;
	}
	catch (const Error & err) {
		std::cout << "The database session could not be opened: " << err << std::endl;
		exit(1);
	}
}

static Schema MakeSchema(Session& sess) {
	try {
		Schema db = sess.getSchema("pokemon_search");
		return db;
	}
	catch (const Error & err) {
		std::cout << "The schema could not be opened: " << err << std::endl;
		exit(1);
	}
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

			SqlResult result = SQLHandler::PerformSql(sess, search);

			if (result.count() == 0)
				std::cout << "NO RESULTS WERE FOUND" << std::endl << std::endl;
			else {
				Row row;
				while ((row = result.fetchOne())) {
					SQLHandler::PrintRow(row);
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