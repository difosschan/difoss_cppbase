#include "dir_util.h"
#include <unistd.h>

extern "C" {
#include <stdlib.h>
#include <string.h>
};

#undef _DEBUG // 临时取消调试信息

#ifdef _DEBUG
#include "macro.h"
#endif // _DEBUG

#include <string>
using namespace std;

bool DirUtil::Ls(const char* directory, std::vector<std::string>* result, size_t* count,
                 const mode_t* mode /* = NULL */, bool fill_path /* = false */) {

    size_t dir_len = directory ? strlen(directory) : 0;
    
    string dir_str = (0 == dir_len) ? "." : directory;
    // 保证路径后面只有一个斜杠"/"
    dir_str += '/';
    dir_str.erase(dir_str.rfind('/'));
    dir_str += '/';
    
    DIR* dir;
    struct dirent* ptr = NULL;
    if ((dir = opendir(dir_str.c_str())) == NULL) {
        return false;
    }
    
#ifdef _DEBUG
    cout << "In DirUtil::Ls()" << endl;
#endif // _DEBUG

    struct stat buf;
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0)
            continue;
        
        string full_filename = dir_str + ptr->d_name;

        // check file type and authority
        if (mode) {
            memset(&buf, 0, sizeof(buf));
            stat(full_filename.c_str(), &buf);
            if ((*mode & buf.st_mode) != *mode)
                continue;
        }
#ifdef _DEBUG
        printf("-- find file: %s\n", ptr->d_name);
#endif // _DEBUG

        if (result) {
            if (fill_path)
                result->push_back(full_filename);
            else
                result->push_back(ptr->d_name);
        }
        if (count)
            ++(*count);
    }

    closedir(dir);
    return true;
}

bool DirUtil::LsDir(const char* directory, std::vector<std::string>& dirs, bool fill_path /* = false */) {
    mode_t mode = S_IFDIR;
    return Ls(directory, &dirs, NULL, &mode, fill_path);
}
bool DirUtil::LsFile(const char* directory, std::vector<std::string>& files, bool fill_path /* = false */) {
    mode_t mode = S_IFREG;
    return Ls(directory, &files, NULL, &mode, fill_path);
}

bool DirUtil::DirCount(const char* directory, size_t& count) {
    mode_t mode = S_IFDIR;
    if (! Ls(directory, NULL, &count, &mode))
        return false;
    return true;
}

bool DirUtil::FileCount(const char* directory, size_t& count) {
    mode_t mode = S_IFREG;
    if (! Ls(directory, NULL, &count, &mode))
        return false;
    return true;
}

string DirUtil::Pwd() {
    char* pch = get_current_dir_name();
    string s(pch);
    free(pch);
    return s;
}

bool DirUtil::LsOrdered(
    const char* directory, DirUtil::OrderedResult_t* result, size_t* count,
    const mode_t* mode /* = NULL */, bool fill_path /* = false */) {

    size_t dir_len = directory ? strlen(directory) : 0;
    
    string dir_str = (0 == dir_len) ? "." : directory;
    // 保证路径后面只有一个斜杠"/"
    dir_str += '/';
    dir_str.erase(dir_str.rfind('/'));
    dir_str += '/';
    
    DIR* dir;
    struct dirent* ptr = NULL;
    if ((dir = opendir(dir_str.c_str())) == NULL) {
        return false;
    }
    
#ifdef _DEBUG
    cout << "In DirUtil::LsOrdered()" << endl;
#endif // _DEBUG

    struct stat buf;
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(".", ptr->d_name) == 0 || strcmp("..", ptr->d_name) == 0)
            continue;
        
        string full_filename = dir_str + ptr->d_name;
        
        // check file type and authority
        if (mode) {
            memset(&buf, 0, sizeof(buf));
            stat(full_filename.c_str(), &buf);
            if ((*mode & buf.st_mode) != *mode)
                continue;
        }
#ifdef _DEBUG
        cout << "-- find file: " << ptr->d_name << ", inode: " << ptr->d_ino << endl;
#endif // _DEBUG

        if (result) {
            if (fill_path)
                result->insert(make_pair(ptr->d_ino, full_filename));
            else
                result->insert(make_pair(ptr->d_ino, ptr->d_name));
        }
        if (count)
            ++(*count);
    }
    
    closedir(dir);
    return true;
}

bool DirUtil::LsDirOrdered(const char* directory, DirUtil::OrderedResult_t& dirs, bool fill_path /* = false */) {
    mode_t mode = S_IFDIR;
    return LsOrdered(directory, &dirs, NULL, &mode, fill_path);
}

bool DirUtil::LsFileOrdered(const char* directory, DirUtil::OrderedResult_t& files, bool fill_path /* = false */) {
    mode_t mode = S_IFREG;
    return LsOrdered(directory, &files, NULL, &mode, fill_path);
}
