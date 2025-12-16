#include "configparser.h"
#include "../debug/Logger.h"

namespace ConfigParser {


ConfigParser::ConfigParser(string fileName)
    :_fileName(fileName)
{

}

bool ConfigParser::parseConfigFile()
{
    std::ifstream fileIn(_fileName.c_str());
    if(!fileIn.is_open())
    {
        LOG_ERROR("Open parse config file %s failed", _fileName.c_str())
        return false;
    }

    string section;
    while(!fileIn.eof())
    {
        string lineStr;
        getline(fileIn, lineStr);
        if(lineStr.empty())
        {
            continue;
        }

        if((lineStr.find('[') != string::npos) && (lineStr.find(']') != string::npos))
        {
           int startPos = lineStr.find('[');
           int endPos = lineStr.find(']');
           int len = endPos - startPos -1;
           section.clear();
           section = lineStr.substr(startPos+1, len);
        }
        else
        {
            if(section.empty())
            {
                continue;
            }
            if(lineStr.find('=') != string::npos)
            {
                int pos = lineStr.find('=');
                string key = lineStr.substr(0, pos);
                string value = lineStr.substr(pos+1);
                _configValue[section][key] = value;
            }
            else
            {
                continue;
            }
        }
    }

    LOG_DEBUG("Parse config file success: map size:%d", _configValue.size());
    return true;
}

string ConfigParser::Get(const string section, const string key)
{
    if(_configValue.find(section) == _configValue.end())
    {
        return "";
    }
    if(_configValue[section].find(key) == _configValue[section].end())
    {
        return "";
    }

    return _configValue[section][key];
}

bool ConfigParser::Set(const string section, const string key, const string value)
{
    if(_configValue.find(section) == _configValue.end())
    {
        return false;
    }
    if(_configValue[section].find(key) == _configValue[section].end())
    {
        return false;
    }

    _configValue[section][key] = value;

    LOG_DEBUG("Set config value success");
    return true;
}

bool ConfigParser::writeValueToFile()
{
    std::ofstream fileOut(_fileName.c_str());
    if(!fileOut.is_open())
    {
        LOG_ERROR("Open write config file %s failed", _fileName.c_str());
        return false;
    }

    for(auto& section : _configValue)
    {
        std::string sectionLine = '[' + section.first + ']' + '\n';
        fileOut.write(sectionLine.c_str(), sectionLine.size());
        for(auto& pair: _configValue[section.first])
        {
           std::string pairLine = pair.first + '=' + pair.second + '\n';
           fileOut.write(pairLine.c_str(), pairLine.size());
        }
    }

    LOG_DEBUG("Write value to file success");
    return true;
}

std::vector<string> ConfigParser::GetAllSections()
{
    std::vector<string> sections;
    for (auto const& element : _configValue) {
        sections.push_back(element.first);
    }
    return sections;
}

bool ConfigParser::IsSection(const string section)
{
    return _configValue.find(section) != _configValue.end();
}

}
