#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H
#include "../global.h"
#include <map>
#include <vector>
#include <fstream>

using namespace std;

namespace ConfigParser
{

using Value = map<string, map<string, string>>;

class ConfigParser
{
public:
    ConfigParser(string fileName);

public:
    bool parseConfigFile();
    string Get(const string section, const string key);
    bool Set(const string section, const string key, const string value);
    bool writeValueToFile();

    std::vector<string> GetAllSections();
    // [新增] 获取指定 Section 下的所有键值对
    std::map<string, string> GetSection(const string section);
    bool IsSection(const string section);

private:
    Value  _configValue;
    string  _fileName;
};

}

#endif // CONFIGPARSER_H
