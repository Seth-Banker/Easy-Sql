// EasySQL.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include "CMDHandler.hpp"
#include "BPT_Table.hpp"

// TODO: Reference additional headers your program requires here.

namespace EasySQL {
	extern vector<Command> commands;
	extern vector<BplusTree> trees;
}
