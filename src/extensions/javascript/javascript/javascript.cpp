
#include <vector>
#include <codecvt>
#include <tuple>
#include "extension_server/extension_server.h"
#include "javascript/extractor.h"
#include "javascript/compiler.h"

using namespace Lya::services;
using namespace Lya::lib::types;
using namespace Lya::javascript_extension;

std::tuple<std::vector<LocalizationLocation>, std::vector<Diagnostic>> extract(const string &file, const vector<string> &function_names, uint64_t start_line) {
    JavaScriptLocalizationExtractor extractor(file, function_names, JavaScriptLanguage::JavaScript);
    return extractor.extract(start_line);
}

std::tuple<std::vector<LocalizationMessage>, std::vector<Diagnostic>> compile(const std::vector<std::string>& localization_files) {
	Compiler compiler;
	return compiler.compile(localization_files);
}

int main() {
    ExtensionServer extension_server("localhost:8888", extract, compile);
    extension_server.start_server(false);
    return 0;
}
