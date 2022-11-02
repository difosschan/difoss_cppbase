#include "bit_util.h"
#include "macro.h"

using namespace std;

namespace BitUtil {

    void DumpHex(const char *bs, size_t bsLen, ostream &os, size_t pl) {
        if (bs == nullptr || pl == 0) {
            return;
        }
        os << "(len=" << bsLen << ")\n";
        int lineCnt = (bsLen + pl - 1) / pl;
        char tmp[8] = {0};
        for (auto i = 0; i < lineCnt; ++i) {
            string hexStr, visStr;
            for (size_t j = 0; j < pl; ++j) {
                size_t chIdx = i * pl + j;
                bool isIn = chIdx < bsLen;
                char ch = isIn ? bs[chIdx] : ' ';
                if (isIn) {
                    snprintf(tmp, sizeof(tmp), "%02X ", ch & 0xFF);
                    hexStr += tmp;
                    visStr += IsByteVisible(ch) ? ch : '.';
                } else {
                    hexStr += "   ";
                    visStr += " ";
                }
            }
            os << hexStr << " " << visStr.substr(0, visStr.size() - 1) << endl;
        }

        return;
    }

    string DumpHex(const char *bs, size_t bsLen, size_t pl) {
        stringstream ss;
        DumpHex(bs, bsLen, ss, pl);
        return ss.str();
    }

    std::string DumpHex(const string& data, size_t pl) {
        return DumpHex(data.data(), data.length(), pl);
    }

    bool IsByteVisible(char c) {
        return c > 31 && c < 127;
    }
}