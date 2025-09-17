#pragma once
#include <vector>
#include <string>

using namespace std;

namespace EasySQL {

    extern string nl;

    struct Command {
        string name;
        vector<string> subcommands;
        bool hasArgs;
        string description;

        // Constructor
        Command(const string& n,
            const vector<string>& subs,
            bool args,
            const string& desc)
            : name(n), subcommands(subs), hasArgs(args), description(desc) {
        }
    };

    //extern vector<Command> commands;

	extern bool sendMessage(string);

	extern bool cmd(vector<string>&);
}