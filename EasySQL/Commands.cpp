#include <string>
#include <vector>
#include "EasySQL.hpp"

using namespace std;

namespace EasySQL {

	bool c_help()
	{
		try {

			string result = "List of available commands :" + nl;

			for (int i = 0; i < commands.size(); i++) {

				result += nl;
				result += "Command : " + commands[i].name + nl;
				result += "Description : " + commands[i].description + nl;

				if (!commands[i].subcommands.empty()) {
					result += "Sub-Commands :" + nl;
					for (int a = 0; a < commands[i].subcommands.size(); a++) {
						result += "- " + commands[i].subcommands[a];
					}
				}
				

				result += nl;
			}

			sendMessage(result);

			return true;
		}
		catch (...) {
			return false;
		}
	}

	bool c_create(vector<string> args) {
		return false;
	}
}