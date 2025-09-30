#pragma once
#include <vector>
#include <string>

using namespace std;

namespace EasyTables {

    // formatting tool
    extern string nl;


    // Handle for information about a command.
    struct Command {
        string name;
        vector<string> subcommands;
        bool hasArgs;
        string description;

        Command(const string& n,
            const vector<string>& subs,
            bool args,
            const string& desc)
            : name(n), subcommands(subs), hasArgs(args), description(desc) {
        }
    };

	// Sends a message to console, helps with autoformatting.
	extern bool sendMessage(string);

	// Sanitize command for execution in cmdExe().
	extern bool cmd(vector<string>&);
}