#include "ini_file.h"
#include "str_util.h"

#include <algorithm>
#include <vector>
#include <fstream>
using namespace std;

//////////////////////////////////////////////////////////////////////////

IniFile::IniFile(bool section_duplicate /* = false */,
                 bool key_duplicate /* = false */)
: _dup_section(section_duplicate)
, _dup_key(key_duplicate)
{}

bool IniFile::open(const char* pinipath)
{
    return do_open(pinipath);
}

std::string
IniFile::read(const char* section, const char* key) const
{
    IniMap_t::const_iterator it = _inimap.find(section);
    if(it == _inimap.end())
        return "";

    const SectionMap_t& one_sect = it->second;
    SectionMap_t::const_iterator sub_it = one_sect.find(key);
    if (sub_it == one_sect.end())
        return "";

    return sub_it->second;
}

bool
IniFile::read(const char* section, const char* key, string& value) const {
    IniMap_t::const_iterator it = _inimap.find(section);
    if(it == _inimap.end())
        return false;

    const SectionMap_t& one_sect = it->second;
    SectionMap_t::const_iterator sub_it = one_sect.find(key);
    if (sub_it == one_sect.end())
        return false;

    value = sub_it->second;
    return true;
}

bool IniFile::do_open(const char* pinipath)
{
    // 打开文件
    ifstream fin(pinipath);
    if(!fin.is_open())
        return false;
    
    string lastSectStr;
    bool isSkipCurSect = true;
    IniMap_t::iterator curSectIter = _inimap.end();
    
    // 逐行获取并解释
    while (!fin.eof()) {
        string one_line;
        getline(fin, one_line);
        StrUtil::TrimSpace(one_line);
        // 解释
        {
            string cur_sect; // 记录当前section名
            
            // 去掉行注释后面的内容
            string str_valid = one_line.substr(0, one_line.find("#"));  // 用"#"注释
            str_valid = str_valid.substr(0, str_valid.find("//"));  // 用"//"注释

            // 解释section
            size_t first = str_valid.find('[');
            size_t last  = str_valid.rfind(']');
            
            if (first != string::npos && last != string::npos && first != last+1) {
                lastSectStr = cur_sect;
                cur_sect = str_valid.substr(first+1, last-first-1); // 取出section名
                
                IniMap_t::iterator dup_sect_iter = _inimap.find(cur_sect);
                if (false == _dup_section) {
                    if (dup_sect_iter != _inimap.end()) {
                        // 不允许重复section，且发现当前section已存在于_inimap中。
                        isSkipCurSect = true;
                        continue;
                    }
                }
                // 允许重复section；或者未发现section重复。
                curSectIter = _inimap.insert(dup_sect_iter, make_pair< Section_t, SectionMap_t >(cur_sect, SectionMap_t()));
                isSkipCurSect = false;
                continue;
            }
            
            if (isSkipCurSect)
                continue;
            
            // if (cur_sect.empty())
                // continue;
            if (curSectIter == _inimap.end())
                continue;
            
            // 解释配置项(key=value)
            if ((first = str_valid.find('=')) == string::npos)
                continue;
            
            string strtmp1 = str_valid.substr(0, first);
            string strtmp2 = str_valid.substr(first+1, string::npos);
            first = strtmp1.find_first_not_of(" \t");
            last  = strtmp1.find_last_not_of(" \t");
            if (first == string::npos || last == string::npos)
                continue;
            string key = strtmp1.substr(first, last-first+1);
            first = strtmp2.find_first_not_of(" \t");
            if(((last = strtmp2.find("\t#", first )) != string::npos) ||
                ((last = strtmp2.find(" #", first )) != string::npos) ||
                ((last = strtmp2.find("\t//", first )) != string::npos)||
                ((last = strtmp2.find(" //", first )) != string::npos))
            {
                strtmp2 = strtmp2.substr(0, last-first);
            }
            last = strtmp2.find_last_not_of(" \t");
            if(first == string::npos || last == string::npos)
                continue;
            string value = strtmp2.substr(first, last-first+1);
            
            // 插入key,value
            SectionMap_t::iterator iter = curSectIter->second.find(key);
            if (false == _dup_key) {
                // 不允许重复key
                if (iter != curSectIter->second.end()) {
                    // 但key重复了
                    continue;
                }
            }
            curSectIter->second.insert(iter, make_pair< Key_t, Value_t >(key, value));
        }
    }
    
    // 逐行解释
    return !_inimap.empty(); 
}

size_t IniFile::count(const char* section) const {
    return _inimap.count(section);
}

std::pair<IniFile::IniMapCiter_t, IniFile::IniMapCiter_t>
IniFile::get(const char* section) const {
    return _inimap.equal_range(section);
}


std::ostream& IniFile::operator<< (std::ostream& out) const {
    IniFile::IniMap_t::const_iterator sector_iter = _inimap.begin();
    for (;sector_iter != _inimap.end(); ++sector_iter)
    {
        out << "[" << sector_iter->first.c_str() <<  "]" << endl;

        const IniFile::SectionMap_t&  one_sector = sector_iter->second;
        IniFile::SectionMap_t::const_iterator key_iter = one_sector.begin();
        for (;key_iter != one_sector.end(); ++key_iter)
        {
            out << "  " << key_iter->first.c_str()
                << " = " << key_iter->second.c_str() << endl;
        }
    }
    return out;
}
//////////////////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, const IniFile& me) {
    me.operator<<(os);
    return os;
}

