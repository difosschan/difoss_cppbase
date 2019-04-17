#include "process.h"
#include "str_util.h"
extern "C" {
#   include <unistd.h>
};

using namespace std;

/* static */ bool Process::IsAlive(Process::PID_t pid) {
    string procStatusFilename = "/proc/";
    procStatusFilename += StrUtil::ToString(pid);
    procStatusFilename += "/status";
    if (0 != access(procStatusFilename.c_str(), R_OK)) {
        return false;
    }
    return true;
}
