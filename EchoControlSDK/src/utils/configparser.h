#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H
#include "../global.h"
#include <map>
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

private:
    Value  _configValue;
    string  _fileName;
};

}

#endif // CONFIGPARSER_H
