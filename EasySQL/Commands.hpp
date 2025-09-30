#pragma once
#include "EasySQL.hpp"
#include "BPT_Table.hpp"

namespace EasyTables {

	// Convert string to NType enum for easier comparison.
	NType strToNType(const string& typeStr) {
		if (typeStr == "INT") return INT;
		else if (typeStr == "DBL") return DBL;
		else if (typeStr == "STR") return STR;
		else if (typeStr == "BOOL") return BOOL;
		else return NA;
	};

	// Convert NType enum to string for messages or in some cases storage.
	string nTypeToStr(const NType& type) {
		switch (type) {
		case INT: return "INT";
		case DBL: return "DBL";
		case STR: return "STR";
		case BOOL: return "BOOL";
		default: return "NA";
		}
	};

	// Check if a string represents a valid NType. Usually used in commands.
	bool isValidType(const string& str, const NType& type)
	{
		if (strToNType(str) == type) return true; else return false;
	};

	// Determine the NType of a given string value. Used for type enforcement.
	NType whatType(const string& str) {
		if (str == "TRUE" || str == "FALSE") {
			return BOOL;
		}
		else if (str.find('.') != std::string::npos) {
			try {
				double value = stod(str);
				return DBL;
			}
			catch (...) {};
		}
		else {
			try {
				int value = stoi(str);
				return INT;
			}
			catch (...) {
				return STR;
			}
		}
	};

	// Prints all information about available commands.
	bool c_help()
	{
		try {

			string result = "List of available commands :";

			for (int i = 0; i < commands.size(); i++) {
				result += "" + nl + nl + "Command : " + commands[i].name;
				result += "" + nl + "Description : " + commands[i].description;

				if (!commands[i].subcommands.empty()) {
					result += "" + nl + "Sub-Commands :";
					for (int a = 0; a < commands[i].subcommands.size(); a++) {
						result += "" + nl + "- " + commands[i].subcommands[a];
					}
				}
			}

			sendMessage(result);

			return true;
		}
		catch (...) {
			return false;
		}
	}

	// Create a new table with a name and column types.
	bool c_create(vector<string> args) {
		try {
			if (args.size() > 2) {

				vector<NType> types;

				for (int i = 1; i < args.size(); i++) {
					if (!(strToNType(args[i]) == NA)) {
						types.push_back(strToNType(args[i]));
					}
					else {
						sendMessage("'" + args[i] + "' is an invalid type. Valid types are INT, DBL, STR, BOOL.");
						return true;
					}
				}

				// nodes split at 5, 4 is full
				trees.emplace_back(args[0], 5, types);
				sendMessage("Created table '" + args[0] + "'.");
				return true;
			}
			else {
				sendMessage("'CREATE TABLE' requires at least 3 arguments, a Name and at least 2 column types (e.g. CREATE TABLE goodTable INT STR).");
				return true;
			}
		}
		catch (...) {
			return false;
		}
	}

	// Insert row into table.
	bool c_insert(vector<string> args) {
		try {
			if (args.size() > 1) {
				string tableName = args[0];
				bool tableFound = false;
				for (int i = 0; i < trees.size(); i++) {
					if (trees[i].name == tableName) {

						// Pass all arguments after the table name as a vector to insert
						vector<string> values(args.begin() + 1, args.end());

						// Type enforcement check
						for (int a = 0; a < values.size(); a++) {
							if (whatType(values[a]) != trees[i].types[a]) {
								sendMessage("Cannot INSERT Row into Table because '" + values[a] + "' is type '" + nTypeToStr(whatType(values[a])) + "' but Column Type is '" + nTypeToStr(trees[i].types[a]) + "'.");
								return true;
							}
						}

						// if type is valid it escapes to here.

						trees[i].insert(values); // Assuming the second parameter is a placeholder or not used

						sendMessage("Inserted into table '" + tableName + "'.");
						return true;
					}
				}

				// table name escape
				sendMessage("Table '" + tableName + "' does not exist. Use 'CREATE TABLE " + tableName + " <column types>' to create it.");
				return true;
			}
			else {
				sendMessage("'INSERT TABLE' requires at least 2 arguments, a Table Name, and Values (e.g. INSERT TABLE goodTable 1 2.3 FALSE hello).");
				return true;
			}
		}
		catch (...) {
			return false;
		}
	}

	// Delete row from table. Requres full row match.
	bool c_delete(vector<string> args) {
		try {
			for (int i = 0; i < trees.size(); i++) {
				if (trees[i].name == args[0]) {
					string title = args[0];
					vector<string> newArgs = args;

					newArgs.erase(newArgs.begin());

					string result = "";
					for (int a = 0; a < newArgs.size(); a++) {
						result += newArgs[a] + " ";
					}

					if (trees[i].delete_entry(newArgs)) {
						sendMessage("Deleted ' " + result + "' from '" + title + "'.");
						return true;
					}

					sendMessage("Could not find row ' " + result + "'.");
					return true;
				}
			}
			return true;
		}
		catch (...) {
			return false;
		}
	}

	// Get row by key from table.
	bool c_get(vector<string> args) {
		try {
			for (int i = 0; i < trees.size(); i++) {

				if (trees[i].name == args[0]) {

					vector<string> row = trees[i].searchByKey(args[1]);

					if (!row.empty()) {

						string result = "Row : ";
						for (int i = 0; i < row.size(); i++) {
							result += row[i] + " ";
						}

						sendMessage(result);
						return true;
					}
					else {
						sendMessage("Could not find key '" + args[1] + "' in table '" + args[0] + "'.");
						return true;
					}
				}
			}
		}
		catch (...) {
			return false;
		}
	}

	// Permanently delete table.
	bool c_drop(vector<string> args) {
		try {
			for (int i = 0; i < trees.size(); i++) {
				if (trees[i].name == args[0]) {
					trees.erase(trees.begin() + i);
					sendMessage("Dropped table '" + args[0] + "'.");
					return true;
				}
			}
			sendMessage("Table '" + args[0] + "' does not exist.");
			return true;
		}
		catch (...) {
			return false;
		}
	}
}
