// MAIN
#include <sstream>

#include "EasyDB.hpp"
#include "CMDHandler.hpp"
#include "BPT_Table.hpp"

using namespace EasyTables;

using namespace std;

namespace EasyTables {

    // Handles to commands.
    vector<Command> commands = {};

	// This will be removed later.
	// Handles to all tables.
    vector<BplusTree> trees = {};

    // Tokenize string pased on delimiter.
    vector<string> tokenize(const string& input, char delimiter = ' ') {
        vector<string> tokens;
        string token;
        stringstream ss(input);

        while (getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }

        return tokens;
    }
}


int main()
{

    /*
	REGISTER COMMNADS HERE
    */
    //commands.push_back(Command("debug", {}, false, "Prints information.")); // commented out in real release
    commands.push_back(Command("help", {}, false, "A list of available commands."));
    commands.push_back(Command("CREATE", { "TABLE" }, true, "Create Tables."));
    commands.push_back(Command("INSERT", { "TABLE" }, true, "Insert elements into Tables."));
	commands.push_back(Command("DELETE", { "TABLE" }, true, "Delete elements from Tables."));
    commands.push_back(Command("GET", { "TABLE" }, true, "Get a value from a Table."));
    commands.push_back(Command("DROP", { "TABLE" }, true, "Delete a Table."));
    
    // Console loop.
    while (true) {
        string x = "";
        getline(cin, x);
        vector<string> input = tokenize(x);
        if (input.empty()) continue;
        if (!cmd(input)) {
            sendMessage("An unknown error has occurred. Try 'help' for a list of commands.");
        }
    }

    return 0;
}
