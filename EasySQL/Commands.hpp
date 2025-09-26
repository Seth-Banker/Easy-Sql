#pragma once
#include "EasySQL.hpp"
#include "BPT_Table.hpp"

namespace EasySQL {

	NType strToNType(const string& typeStr) {
		if (typeStr == "INT") return INT;
		else if (typeStr == "DBL") return DBL;
		else if (typeStr == "STR") return STR;
		else if (typeStr == "BOOL") return BOOL;
		else return NA;
	};

	string nTypeToStr(const NType& type) {
		switch (type) {
		case INT: return "INT";
		case DBL: return "DBL";
		case STR: return "STR";
		case BOOL: return "BOOL";
		default: return "NA";
		}
	};

	// type enforcement.
	bool isValidType(const string& str, const NType& type)
	{
		if (strToNType(str) == type) return true; else return false;
	};

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

	// Table needs to have type enforcement. (this currently doesnt exist)
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
						// check equates to type not is type
						for (int a = 0; a < values.size(); a++) {
							if (whatType(values[a]) != trees[i].types[a]) {
								sendMessage("Entered type does not match Table Column. '" + values[a] + "' is type : " + nTypeToStr(whatType(values[a])));
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
				sendMessage("'INSERT TABLE' requires at least 2 arguments, a Table Name, a Value (e.g. INSERT TABLE goodTable 123).");
				return true;
			}
		}
		catch (...) {
			return false;
		}
    }

	// Look for key -> value 1 -> value 2 -> etc
	bool c_find(vector<string> args) {
		try {
			for (int i = 0; i < trees.size(); i++) {
				if (trees[i].name == args[0]) {
					//if(args.size <)
				}
			}

			// last escape
			sendMessage("Table '" + args[0] + "' does not exist.");
			return true;
		}
		catch (...) {
			return false;
		}
	}

	bool c_delete(vector<string> args) {
		try {
			for (int i = 0; i < trees.size(); i++) {
				if (trees[i].name == args[0]) {
					string title = args[0];
					vector<string> newArgs = args; // erase table name from args


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

	bool c_debug() {
		try {
			string result = "List of trees :";
			if (trees.size() > 0) {
				for (int i = 0; i < trees.size(); i++) {
					result += "\n - " + trees[i].name + " | ";
					for (int a = 0; a < trees[i].types.size(); a++) {
						result += nTypeToStr(trees[i].types[a]) + " ";
					}
				}

				sendMessage(result);

				trees[0].printTree(trees[0].root);
			}
			else {
				sendMessage("No trees currently exist.");
			}
		}
		catch (...) {
			return false;
		}
	}
}