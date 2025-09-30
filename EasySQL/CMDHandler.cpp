#include <vector>
#include <string>
#include <iostream>
#include "CMDHandler.hpp"
#include "Commands.hpp"
#include "BPT_Table.hpp"

using namespace std;

namespace EasyTables {

	// Formatting tool
	string nl = "\n";

	// Send message to console with formatting.
	bool sendMessage(string input) {
		cout << input << nl;
		return true;
	}

	/*
	Attach functionality to commands.
	Input is sanitized in cmd().
	*/
	bool cmdExe(vector<string> args = {}) {
		// this needs to be restructured later
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
			else if (args[0] == "INSERT" && args[1] == "TABLE") {
				args.erase(args.begin(), args.begin() + 2);
				if (c_insert(args)) {
					return true;
				}
				else return false;
			}
			else if (args[0] == "debug") {
				/*
				if (c_debug()) {
					return true;
				}
				else return false;
				*/
			}
			else if (args[0] == "DELETE" && args[1] == "TABLE") {
				args.erase(args.begin(), args.begin() + 2);
				if (c_delete(args)) {
					return true;
				}
				else return false;
			}
			else if (args[0] == "GET" && args[1] == "TABLE") {
				args.erase(args.begin(), args.begin() + 2);
				if (c_get(args)) {
					return true;
				}
				else return false;
			}
			else if (args[0] == "DROP" && args[1] == "TABLE") {
				args.erase(args.begin(), args.begin() + 2);
				if (c_drop(args)) {
					return true;
				}
				else return false;
			}
			

			// if function escapes to here then we never found a matching command.
			return false;
		}
		catch (...) {
			return false;
		}
	}

	/*
	Filter command if its even valid.

	Possible formats:
	<command>
	<command> <args>
	<command> <subcommand>
	<command> <subcommand> <args>
	*/
	bool cmd(vector<string>& tokens) {
		try {
			for (int i = 0; i < commands.size(); i++) {

				// Found command.
				if (commands[i].name == tokens[0]) {

					// Quick check if sub-commands exists and if the user even supplied a token for one.
					if (!commands[i].subcommands.empty() && tokens.size() > 1) {

						// Now we actually look for the proper sub-command now that we know the command has a sub-command and the user supplied a token for it.
						for (int a = 0; a < commands[i].subcommands.size(); a++) {

							// Found specific sub-command.
							if (commands[i].subcommands[a] == tokens[1]) {

								// Execute based if sub-command has no args.
								if (!commands[i].hasArgs) {
									if (cmdExe(tokens)) {
										return true;
									}
									else {
										return false;
									}
								}
								else
								// Execute if sub-command has args and args are supplied.
								if (commands[i].hasArgs && tokens.size() > 2) {
									if (cmdExe(tokens)) {
										return true;
									}
									else {
										return false;
									}
								}
								// The sub-command requires args but none were supplied.
								else {
									sendMessage("'" + tokens[0] + " " + tokens[1] + "' requires arguments.");
									return true;
								}
							}
						}

						// If function escapes to here then we never found a matching sub-command.
						sendMessage("'" + tokens[1] + "' is not a valid sub-command for '" + tokens[0] + "'. Try 'help' for a list of commands and their sub-commands.");
						return true;
					}
					// A sub-command is required but none was supplied.
					else if (!commands[i].subcommands.empty() && tokens.size() < 2) {
						sendMessage("'" + tokens[0] + "' requires a sub-command. Try 'help' for a list of commands and their sub-commands.");
						return true;
					}
					// If the command has no sub-commands.
					else if (commands[i].subcommands.empty()) {

						// Execute if no args required.
						if (!commands[i].hasArgs) {
							if (cmdExe(tokens)) {
								return true;
							}
							else {
								return false;
							}
						}
						else
						// Execute if args are required and supplied.
						if (commands[i].hasArgs && tokens.size() > 1) {
							if (cmdExe(tokens)) {
								return true;
							}
							else {
								return false;
							}
						}
						// Args are required but none were supplied.
						else {

							sendMessage("'" + tokens[0] + "' requires arguments. Try 'help' for a list of commands.");
							return true;
						}
					}
				}
			}

			// If function escapes to here then we never found a matching command.
			sendMessage("Uknown command '" + tokens[0] + "'. Try 'help' for a list of commands.");
			return true;
		}
		catch (...) {
			return false;
		}
	}
}