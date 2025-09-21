#pragma once
#include "EasySQL.hpp"
#include "BPT_Table.hpp"

namespace EasySQL {

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


				//result += nl;
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
			if (args.size() > 1) {

				// nodes split at 5, 4 is full
				trees.emplace_back(args[0], 5);
				sendMessage("Created table '" + args[0] + "'.");
				return true;
			}
			else {
				sendMessage("'CREATE TABLE' requires at least 2 arguments, a Name and at least 1 column type (e.g. CREATE TABLE goodTable INT). You only submitted 1 argument : '" + args[0] + "'.");
				return true;
			}
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
					result += "\n - " + trees[i].name;
				}

				sendMessage(result);
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