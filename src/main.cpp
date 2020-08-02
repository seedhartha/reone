#include "core/log.h"
#include "program.h"

using namespace reone;

int main(int argc, char **argv) {
    try {
        return Program(argc, argv).run();
    }
    catch (const std::exception &e) {
        error(e.what());
        return 1;
    }
}
