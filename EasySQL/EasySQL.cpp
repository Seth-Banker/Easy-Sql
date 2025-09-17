// MAIN
#include <sstream>

#include "EasySQL.hpp"
#include "CMDHandler.hpp"
#include "BPT_Table.hpp"

using namespace EasySQL;

using namespace std;

namespace EasySQL {

    vector<Command> commands = {};

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

/*TODO 
data types need to be enforced.
commands create, find, findrange, insert, delete, delete range need to be added.
*/


int main()
{
    commands.push_back(Command("help", {}, false, "A list of available commands."));
    commands.push_back(Command("CREATE", { "TABLE" }, true, "Create containers (e.g. Database, Table)"));
    commands.push_back(Command("INSERT", { "TABLE" }, true, "Insert elements into containers (e.g. Database, Table)"));

    /*
    int record_len = 3;
    BplusTree bplustree(record_len);
    bplustree.insert("bart", "33");
    bplustree.insert("test", "21");
    bplustree.insert("meow", "31");
    bplustree.printTree(bplustree.root);
    if (bplustree.find("bart", "33")) {
        cout << "Found" << endl;
    }
    else {
        cout << "Not found" << endl;
    }
    */
    

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
