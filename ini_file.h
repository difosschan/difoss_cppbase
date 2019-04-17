#pragma once

#include <string>
#include <map>

class IniFile
{
public:
    typedef std::string     Section_t;
    typedef std::string     Key_t;
    typedef std::string     Value_t;
    typedef std::multimap< Key_t, Value_t >             SectionMap_t;
    typedef std::multimap< Section_t, SectionMap_t >     IniMap_t;
    
    typedef SectionMap_t::iterator  SectionIter_t;
    typedef IniMap_t::iterator      IniMapIter_t;
    typedef SectionMap_t::const_iterator  SectionCiter_t;
    typedef IniMap_t::const_iterator      IniMapCiter_t;

public:
    // @note 
    // @param[in] section_duplicate allow dupliacate section name in one ini file.
    // @param[in] section_duplicate allow dupliacate section name in the same section.
    IniFile(bool section_duplicate = false, bool key_duplicate = false);
    
    // @note open ini format file.
    bool open(const char* pinipath);
    
    // @note read the value of '[section] key'.
    std::string read(const char* section, const char* key) const;
    
    // @note other way to read().
    // @retval return true if found.
    bool read(const char* section, const char* key, std::string& value) const;
    
    // @note count of [section]
    size_t count(const char* section) const;
    // @note get all [section]
    std::pair<IniMapCiter_t, IniMapCiter_t> get(const char* section) const;
    // @note print
    std::ostream& operator << (std::ostream&) const;
    
protected:
    bool do_open(const char* pinipath);

    IniMap_t _inimap;
    bool _dup_section;
    bool _dup_key;
};

std::ostream& operator << (std::ostream&, const IniFile&);

