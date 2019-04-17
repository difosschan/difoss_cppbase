#include "file_util.h"
#include "eintr_wrapper.h"
#include "mem_util.h"

extern "C" {
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> // strerror()
#include <errno.h>  // errno
#include <limits.h>
#include <stdlib.h> // free
#include <libgen.h> // dirname(), basename()
};

// for debug
#include <iostream>
#include "bit_util.h"
#include "macro.h"

using namespace std;

#define BUFFER_SIZE 4096

bool FileUtil::Rename(const char* oldpath, const char* newpath) {
    return 0 == ::rename(oldpath, newpath);
}

bool FileUtil::Move(const char* srcFile, const char* dstFile, bool cover /* = true */) {
    if (FileUtil::IsSame(srcFile, dstFile))
        return true;

    int r = access(dstFile, F_OK);
    if (0 == r) { // dst file alread exist.
        if (cover) { // remove it when 'cover' is true.
            ::remove(dstFile);
        } else {
            return false;
        }
    }

    FILE* src = fopen(srcFile, "rb"); // read and write
    if (src == NULL) {
        fprintf(stderr, "[%s:%d] source file %s cannot open, reason:(%d) %s.\n",
               __FILE__, __LINE__, srcFile, errno, strerror(errno));
        return false;
    }

    FILE* dst = fopen(dstFile, "wb"); // write, create if it's not exist.
    if (dst == NULL) {
        fprintf(stderr, "[%s:%d] destinate file %s cannot open, reason:(%d) %s.\n",
               __FILE__, __LINE__, dstFile, errno, strerror(errno));
        fclose(src);
        return false;
    }
    static char buf[BUFFER_SIZE];
    size_t readNum = 0;
    while (1) {
        readNum = fread( (void*)buf, sizeof(char), BUFFER_SIZE, src);
        fwrite(buf, sizeof(char), readNum, dst);
        if (readNum < BUFFER_SIZE) {
            break;
        }
    }
    fclose(src);
    fclose(dst);

    if (0 != unlink(srcFile)) {
        fprintf(stderr, "[%s:%d] source file %s unlink error, reason:(%d) %s.\n",
               __FILE__, __LINE__, srcFile, errno, strerror(errno));
    }

    return true;

}

void FileUtil::CloseFD(int fd) {
    const int errno_copy = errno;
    if (IGNORE_EINTR(close(fd)) < 0) {
        perror("close");
        errno = errno_copy;
    }
}

bool FileUtil::IsSame(const char* fileOrDir, const char* anotherFileOrPath) {
    if (strcmp(fileOrDir, anotherFileOrPath) == 0)
        return true;

    struct stat st1, st2;
    if (0 != stat(fileOrDir, &st1)) return false;
    if (0 != stat(anotherFileOrPath, &st2)) return false;

    return st1.st_ino == st2.st_ino;
}

bool FileUtil::RealPath(const char* fileOrDir, std::string& resolved) {
    char* p = realpath(fileOrDir, NULL);
    if (NULL == p) {
        fprintf(stderr, "[%s:%d] file [%s] realpath error, reason:(%d) %s.\n",
               __FILE__, __LINE__, fileOrDir, errno, strerror(errno));
        return false;
    }

    resolved = p;
    if (p)
        free(p);
    return true;
}

std::string FileUtil::DirName(const char* fileOrDir) {
    string result;
    if (NULL == fileOrDir)
        return result;
    size_t len = strlen(fileOrDir);
    if (len == 0)
        return result;

    MemUtil::PtrHolder<char> in(new char[len + 1]);
    strncpy(in.get(), fileOrDir, len + 1);
    char* p = dirname(in.get());
    if (p)
        result.assign(p, strlen(p));
    return result;
}

std::string DirName(const std::string& fileOrDir) {
    return DirName(fileOrDir.c_str());
}

std::string FileUtil::BaseName(const char* fileOrDir) {
    string result;
    if (NULL == fileOrDir)
        return result;
    size_t len = strlen(fileOrDir);
    if (len == 0)
        return result;
    MemUtil::PtrHolder<char> in(new char[len + 1]);
    strncpy(in.get(), fileOrDir, len + 1);
    char* p = basename(in.get());
    if (p)
        result.assign(p, strlen(p));
    return result;
}
std::string FileUtil::BaseName(const std::string& fileOrDir) {
    return FileUtil::BaseName(fileOrDir.c_str());
}

const char* FileUtil::FileName(const char* fullDirAndFileName) {
    if (NULL == fullDirAndFileName || 0 == strlen(fullDirAndFileName))
        return "";
    size_t len = strlen(fullDirAndFileName);
    if (0 == len)
        return "";

    const char * pLastDirSep = strrchr(fullDirAndFileName, '/');
    if (NULL == pLastDirSep)
        return "";

    return pLastDirSep + 1;
}

std::string FileUtil::FileName(const std::string& fullDirAndFileName) {
    string str;
    const char* p = FileName(fullDirAndFileName.c_str());
    if (p)
        str = p;
    return str;
}

size_t FileUtil::FileSize(const char* filename) {
    size_t size = 0;
    struct stat statBuf;
    if (stat(filename, &statBuf) >= 0)
        size = statBuf.st_size;
    return size;
}

size_t FileUtil::FileSize(ifstream& fs) {
    ifstream::pos_type cur = fs.tellg();

    fs.seekg(0, std::ios_base::beg);
    ifstream::pos_type beg = fs.tellg();

    fs.seekg(0, std::ios_base::end);
    ifstream::pos_type end = fs.tellg();

    // relocate the file pointer to orginal position which stored by variable 'cur'.
    fs.seekg(cur, std::ios_base::beg);

    return static_cast<size_t>(end - beg);
}
