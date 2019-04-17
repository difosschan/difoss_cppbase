#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <string>
#include <vector>
#include <map>

namespace DirUtil {
    bool Ls(const char* directory, std::vector<std::string>* result, size_t* count, const mode_t* mode = NULL, bool fill_path = false);
    bool LsDir(const char* directory, std::vector<std::string>& dirs, bool fill_path = false);
    bool LsFile(const char* directory, std::vector<std::string>& files, bool fill_path = false);
    bool DirCount(const char* directory, size_t& count);
    bool FileCount(const char* directory, size_t& count);
    std::string Pwd();

    // linux only cause 'inode'
    typedef std::map<ino_t, std::string>  OrderedResult_t;
    bool LsOrdered(const char* directory, OrderedResult_t* result, size_t* count, const mode_t* mode = NULL, bool fill_path = false);
    bool LsDirOrdered(const char* directory, OrderedResult_t& dirs, bool fill_path = false);
    bool LsFileOrdered(const char* directory, OrderedResult_t& files, bool fill_path = false);
    
} // namespace DirUtil
