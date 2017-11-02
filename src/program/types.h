
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace Lya::Types {

	enum Character {
	    NullCharacter = 0,
	    MaxAsciiCharacter = 0x7F,

	    LineFeed = 0x0A,              // \n
	    CarriageReturn = 0x0D,        // \r
	    LineSeparator = 0x2028,
	    ParagraphSeparator = 0x2029,
	    NextLine = 0x0085,

	    // Unicode 3.0 space characters
	    Space = 0x0020,   // " "
	    NonBreakingSpace = 0x00A0,   //
	    EnQuad = 0x2000,
	    EmQuad = 0x2001,
	    EnSpace = 0x2002,
	    EmSpace = 0x2003,
	    ThreePerEmSpace = 0x2004,
	    FourPerEmSpace = 0x2005,
	    SixPerEmSpace = 0x2006,
	    FigureSpace = 0x2007,
	    PunctuationSpace = 0x2008,
	    ThinSpace = 0x2009,
	    HairSpace = 0x200A,
	    ZeroWidthSpace = 0x200B,
	    NarrowNoBreakSpace = 0x202F,
	    IdeographicSpace = 0x3000,
	    MathematicalSpace = 0x205F,
	    Ogham = 0x1680,

	    _ = 0x5F,
	    $ = 0x24,

	    _0 = 0x30,
	    _1 = 0x31,
	    _2 = 0x32,
	    _3 = 0x33,
	    _4 = 0x34,
	    _5 = 0x35,
	    _6 = 0x36,
	    _7 = 0x37,
	    _8 = 0x38,
	    _9 = 0x39,

	    a = 0x61,
	    b = 0x62,
	    c = 0x63,
	    d = 0x64,
	    e = 0x65,
	    f = 0x66,
	    g = 0x67,
	    h = 0x68,
	    i = 0x69,
	    j = 0x6A,
	    k = 0x6B,
	    l = 0x6C,
	    m = 0x6D,
	    n = 0x6E,
	    o = 0x6F,
	    p = 0x70,
	    q = 0x71,
	    r = 0x72,
	    s = 0x73,
	    t = 0x74,
	    u = 0x75,
	    v = 0x76,
	    w = 0x77,
	    x = 0x78,
	    y = 0x79,
	    z = 0x7A,

	    A = 0x41,
	    B = 0x42,
	    C = 0x43,
	    D = 0x44,
	    E = 0x45,
	    F = 0x46,
	    G = 0x47,
	    H = 0x48,
	    I = 0x49,
	    J = 0x4A,
	    K = 0x4B,
	    L = 0x4C,
	    M = 0x4D,
	    N = 0x4E,
	    O = 0x4F,
	    P = 0x50,
	    Q = 0x51,
	    R = 0x52,
	    S = 0x53,
	    T = 0x54,
	    U = 0x55,
	    V = 0x56,
	    W = 0x57,
	    X = 0x58,
	    Y = 0x59,
	    Z = 0x5a,

	    Ampersand = 0x26,             // &
	    Asterisk = 0x2A,              // *
	    At = 0x40,                    // @
	    Backslash = 0x5C,             // \
	    Backtick = 0x60,              // `
	    Bar = 0x7C,                   // |
	    Caret = 0x5E,                 // ^
	    CloseBrace = 0x7D,            // }
	    CloseBracket = 0x5D,          // ]
	    CloseParen = 0x29,            // )
	    Colon = 0x3A,                 // :
	    Comma = 0x2C,                 // ,
	    Dot = 0x2E,                   // .
	    DoubleQuote = 0x22,           // "
	    Equals = 0x3D,                // =
	    Exclamation = 0x21,           // !
	    GreaterThan = 0x3E,           // >
	    Hash = 0x23,                  // #
	    LessThan = 0x3C,              // <
	    Minus = 0x2D,                 // -
	    OpenBrace = 0x7B,             // {
	    OpenBracket = 0x5B,           // [
	    OpenParen = 0x28,             // (
	    Percent = 0x25,               // %
	    Plus = 0x2B,                  // +
	    Question = 0x3F,              // ?
	    Semicolon = 0x3B,             // ;
	    SingleQuote = 0x27,           // '
	    Slash = 0x2F,                 // /
	    Tilde = 0x7E,                 // ~

	    Backspace = 0x08,             // \b
	    FormFeed = 0x0C,              // \f
	    ByteOrderMark = 0xFEFF,
	    Tab = 0x09,                   // \t
	    VerticalTab = 0x0B,           // \v
	};

	enum class CommandKind {
	    None,
	    Init,
	    Sync,
	    Log,
	    Set,
	    Extension_RunTests,
	    Extension_AcceptBaselines,
	};

	enum class FlagKind {
	    None,
	    Help,
	    Version,
	    Language,
	    Id,
	    Value,
	    RootDir,
	    Grep,
	    NoServer,
		StartLine,
		Test,
	};

	struct DiagnosticTemplate {
	    string message_template;
	};

	struct Span {
		unsigned int start;
		unsigned int end;
	};

	struct SpanLocation {
		unsigned int line;
		unsigned int column;
		unsigned int length;
		unsigned int position;
	};

	struct Param {
		string name;
		bool is_list;
		shared_ptr<string> type;

		Param(string _name, bool _is_list):
				name(move(_name)),
				is_list(_is_list) {}

		Param(string _name, bool _is_list, string _type):
				name(move(_name)),
				is_list(_is_list),
				type(make_shared<string>(_type)) {}
	};

	struct Localization {
		string id;
		vector<Param> params;
		SpanLocation location;
	};

	struct Location {
		unsigned int line;
		unsigned int column;
	};

	typedef std::map<string, vector<Localization>> FileToLocalizations;

	struct Diagnostic {
	    string message;
		SpanLocation location;
	};

	struct Argument {
	    string name;
	    string description;

	    Argument(string _name, string _description) :
	        name(move(_name)),
	        description(move(_description)) { }
	};

	struct Flag : Argument {
	    string alias;
	    bool has_value;
	    FlagKind kind;
	    string value;

	    Flag(FlagKind kind, const char name[], const char alias[], const char description[], bool _has_value) :
	        Argument(string(name), string(description)),
	        alias(string(alias)),
	        has_value(_has_value),
	        kind(kind),
	        value("") { }
	};

	struct Command : Argument {
	    vector<Flag> flags;
	    CommandKind kind;
	    string info;
	    Command(CommandKind _kind, const char _name[], const char _description[], const char _info[]) :
	        Argument(string(_name), string(_description)),
	        kind(_kind),
	        info(string(_info)) { }

	    Command(CommandKind _kind, const char _name[], const char _description[], const char _info[], vector<Flag> _flags) :
	        Argument(string(_name), string(_description)),
	        flags(move(_flags)),
	        kind(_kind),
	        info(string(_info)) { }
	};

	class Session {
	public:
	    bool start_server;
	    bool is_requesting_help;
	    bool is_requesting_version;
	    string root_dir;
		uint64_t start_line;
	    CommandKind command;
	    vector<Diagnostic> diagnostics;
	    string programming_language;
		unique_ptr<string> test;

	    Session():
	        start_server(true),
	        is_requesting_help(false),
	        is_requesting_version(false),
	        command(CommandKind::None) { }

		Session (const Session& o)
		{
			if (this != &o) {
				start_server = o.start_server;
				is_requesting_help = o.is_requesting_help;
				is_requesting_version = o.is_requesting_version;
				start_line = o.start_line;
				root_dir = o.root_dir;
				command = o.command;
				programming_language = o.programming_language;
				if (o.test != nullptr) {
					test = make_unique<string>(*o.test);
				}
			}
		}

		Session &operator=(Session &&o) noexcept
		{
			if (this != &o)
			{
				test = move(o.test);
			}
			return *this;
		}

	    void add_diagnostic(const Diagnostic& diagnostic) {
	        diagnostics.push_back(diagnostic);
	    }
	};

} // Lya::Types

#endif // TYPES_H