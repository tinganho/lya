
#include "json.hpp"
#include "Utils.cpp"
#include "Configurations.h"
#include <string>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>

using namespace std;
using namespace L10ns;
using json = nlohmann::json;

string output =
    "#include \"Types.cpp\""
    "\n"
    "using namespace std;"
    "\n"
    "namespace D {\n";

vector<string> keys = {};

bool is_unique(string key) {
    for (auto const& k : keys) {
        if (k == key) {
            return false;
        }
    }
    return true;
}

string format_diagnostic_key(string key) {
    string k = boost::regex_replace(key, boost::regex("\\s+"), "_");
    k = boost::regex_replace(output, boost::regex("\\.|\\'|\\\""), "");
    keys.push_back(k);
    return k;
}

int main() {
    string json = read_file(PROJECT_DIR "Source/Program/Diagnostics.json");
    auto diagnostics = json::parse(json);
    for (json::iterator it = diagnostics.begin(); it != diagnostics.end(); ++it) {
        string key = format_diagnostic_key(it.key());
        output += "    auto " + key + " = new Diagnostic(\"" + it.key() + "\");\n";
        if (!is_unique(key)) {
            throw invalid_argument("Duplicate formatted key: " + key + ".");
        }
        keys.push_back(key);
    }

    output += "}";

    write_file(PROJECT_DIR "Source/Program/Diagnostics.cpp", output);
    cout << output << endl;
}
