#include <vector>
#include <string>
#include <iostream>
#include "CMDHandler.hpp"
#include "Commands.hpp"

using namespace std;

namespace EasySQL {

	string nl = "\n";

	bool sendMessage(string input) {
		cout << input << nl;
		return true;
	}

	bool cmdExe(vector<string> args = {}) {
		try {

			if (args[0] == "help") {
				if (c_help()) {
					return true;
				}
				else return false;
			}
			else if (args[0] == "CREATE" && args[1] == "TABLE") {
				args.erase(args.begin(), args.begin()+2);
				if (c_create(args)) {
					return true;
				}
				else return false;
			}
			else if (args[0] == "debug") {
				if (c_debug()) {
					return true;
				}
				else return false;
			}

		}
		catch (...) {
			return false;
		}
	}

	// mmm pasta!
	// filter command if its even valid.
	// <command>
	// <command> <args>
	// <command> <subcommand>
	// <command> <subcommand> <args>
	bool cmd(vector<string>& tokens) {
		try {
			for (int i = 0; i < commands.size(); i++) {
				// found command
				if (commands[i].name == tokens[0]) {

					// sub commands exist and subcommand token is inputted
					if (!commands[i].subcommands.empty() && tokens.size() > 1) {
						for (int a = 0; a < commands[i].subcommands.size(); a++) {

							// we found sub command
							if (commands[i].subcommands[a] == tokens[1]) {

								// no args
								if (!commands[i].hasArgs) {
									if (cmdExe(tokens)) {
										return true;
									}
									else {
										return false;
									}
								}
								else
									// args
								if (commands[i].hasArgs && tokens.size() > 2) {
									if (cmdExe(tokens)) {
										return true;
									}
									else {
										return false;
									}
								}
								//args but args not supplied
								else {
									sendMessage("'" + tokens[0] + " " + tokens[1] + "' requires arguments.");
									return true;
								}
							}
						}

						// sub command escape
						sendMessage("'" + tokens[1] + "' is not a valid sub-command for '" + tokens[0] + "'. Try 'help' for a list of commands and their sub-commands.");
						return true;
					}
					// no sub command supplied
					else if (!commands[i].subcommands.empty() && tokens.size() < 2) {
						sendMessage("'" + tokens[0] + "' requires a sub-command. Try 'help' for a list of commands and their sub-commands.");
						return true;
					}
					// no sub commands
					else if (commands[i].subcommands.empty()) {

						// no args
						if (!commands[i].hasArgs) {
							if (cmdExe(tokens)) {
								return true;
							}
							else {
								return false;
							}
						}
						else

							// has args
						if (commands[i].hasArgs && tokens.size() > 1) {
							if (cmdExe(tokens)) {
								return true;
							}
							else {
								return false;
							}
						}
						// has args none supplied
						else {

							sendMessage("'" + tokens[0] + "' requires arguments. Try 'help' for a list of commands.");
							return true;
						}
					}
				}
			}

			//last escape
			sendMessage("Uknown command '" + tokens[0] + "'. Try 'help' for a list of commands.");
			return true;
		}
		catch (...) {
			return false;
		}
	}
}