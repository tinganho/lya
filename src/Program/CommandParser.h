
#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <vector>
#include <string>
#include "Types.h"

using namespace std;
using namespace Lya::Types;

namespace Lya {
namespace CommandParser {

extern const vector<Command> commands;
extern const vector<Flag> default_flags;

const vector<Flag>& get_command_flags(CommandKind kind);

void set_command_flag(Session& session, const Flag* flag, char* value);

Session parse_command_args(int argc, char* argv[]);

} // CommandParser
} // Lya

#endif // COMMAND_PARSER_H
