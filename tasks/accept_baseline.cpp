
#include "utils.cpp"
#include "configurations.h"

using namespace lya::utils;

int main() {
	remove_folder(PROJECT_DIR "src/program/tests/references");
    copy_folder(PROJECT_DIR "src/program/tests/currents", PROJECT_DIR "src/program/tests/references");
}
