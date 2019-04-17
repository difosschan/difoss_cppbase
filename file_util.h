#pragma once
#include <string>
#include <fstream>

namespace FileUtil {
    bool Rename(const char* oldpath, const char* newpath);
    bool Move(const char* oldfile, const char* newfile, bool cover = true);
    void CloseFD(int fd);
    bool IsSame(const char* fileOrDir, const char* anotherFileOrPath);

    //! 获得文件（或目录）的真实路径（绝对路径）
    // @param[in]   fileOrDir 文件或文件夹（相对路径甚至含有多个目录分隔符或者.或者..）。
    // @param[out]  resolved  文件的真实路径（绝对路径），若输入参数是一个软链接，则返回目标文件的路径。
    bool RealPath(const char* fileOrDir, std::string& resolved);

    //! 获得文件（或目录）所在的目录
    std::string DirName(const char* fileOrDir);
    std::string DirName(const std::string& fileOrDir);

    //! 获得文件（或目录）的文件名（或目录名）
    std::string BaseName(const char* fileOrDir);
    std::string BaseName(const std::string& fileOrDir);

    //! 获得文件名（不含路径）
    //! \return 如果找不到目录分隔符“/”则返回 NULL
    const char* FileName(const char* fullDirAndFileName);
    std::string FileName(const std::string& fullDirAndFileName);

    size_t FileSize(const char* filename);
    size_t FileSize(std::ifstream& fs);
} // namespace FileUtil
