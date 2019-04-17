#include "bit_util.h"
#include "macro.h"

void BitUtil::ChexDump(const char* buff, size_t len, FILE* file)
{
    if (buff == NULL) {
        fprintf(file, "(BitUtil::ChexDump nothing cuz buff = NULL)\n");
        return;
    }
    if (len == 0) {
        fprintf(file, "(BitUtil::ChexDump nothing cuz len = 0)\n");
        return;
    }

    size_t i = 0;
    for(; i < len; ++i)
    {
        if (i != 0 && i % 16 == 0) 
        {
            fprintf(file, "   ");
            for(size_t j = static_cast<size_t>(i - 16); j < i; ++j) {
                int buff_j = buff[j];
                if (buff_j > 31 && buff_j < 128)
                    fprintf(file, "%c", static_cast<char>(buff[j]));
                else
                    fprintf(file, ".");
            }
            fprintf(file, "\n");
        }

        fprintf(file, "%02X ", (buff[i]) & 0xFF);
    }

    // the last line:
    // fill with blackspace to indent in last line.
    for (size_t j = i % 16; j < 16; ++j) {
        fprintf(file, "   ");
    }
    fprintf(file, "  ");
    for(size_t j = i % 16 ? i - i % 16 : i - 16 ; j < i; ++j) {
        int buff_j = buff[j];
        if (buff_j > 31 && buff_j < 128)
            fprintf(file, "%c", static_cast<char>(buff[j]));
        else
            fprintf(file, ".");
    }

    fprintf(file, "\n");
    fflush(file);
}

void BitUtil::HexDump(const char* buff, size_t len, std::ostream& os)
{
    if (buff == NULL) {
        os << "(BitUtil::HexDump nothing cuz buff = NULL)\n";
        return;
    }
    if(len == 0) {
        os << "(BitUtil::HexDump nothing cuz len = 0)\n";
        return;
    }

    size_t i = 0;
    for(; i < len; ++i)
    {
        if(i != 0 && i % 16 == 0)
        {
            os << "  ";
            for(size_t j = static_cast<size_t>(i - 16); j < i; ++j) {
                int buff_j = buff[j];
                if (buff_j > 31 && buff_j < 128)
                    os << static_cast<char>(buff[j]);
                else
                    os << ".";
            }
            os << "\n";
        }
        os.width(2);
        os.fill('0');
        
        os <<  std::hex << std::uppercase << int(buff[i] & 0xFF) << std::nouppercase << std::dec << ' ' ;
    }
    // the last line:
    // fill with blackspace to indent in last line.
    for (size_t j = i % 16; j < 16; ++j) {
        os << "   ";
    }

    os.fill(' ');
    
    os << "  ";
    for(size_t j = i % 16 ? i - i % 16 : i - 16 ; j < i; ++j) {
        int buff_j = buff[j];
        if (buff_j > 31 && buff_j < 128)
            os << static_cast<char>(buff[j]);
        else
            os << ".";
    }

    os << "\n";
    std::flush(os);
}

