#include <stdio.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include <fstream>
#include <vector>
#include <unistd.h>
#include <iomanip>

using json = nlohmann::json;

std::string configFilename = ".rsyncez.json";
std::string source;
std::string destination;

bool isExclude = false;
bool isBash = false;

std::vector<std::string> paramFolder;
std::vector<std::string> folder;
std::vector<std::string> rsyncArgs;

void usage()
{
    std::cout <<  "usage: sync [source] [destination] [ directory | bundle]" << std::endl <<
                  "  -x : directories are exluded, rest ist synced" << std::endl <<
                  "  -c : generate default config file" << std::endl <<
                  "  -b : only generate bash command" << std::endl <<
                  "  -h : show help" << std::endl;
}

void writeDefaultConfigFile()
{
    json j = {
        { "rsyncflags", {"-auvz", "--progress"} },
        { "endpoint", {
            {
                { "name", "local" },
                { "path", "/home/drtosh/stuff" },
                { "type", "local" }
            },
            {
                { "name", "server" },
                { "path", "drtosh.de:/home/drtosh/stuff" },
                { "type", "ssh" }
            }
        }},
        { "bundle", {
            {
                { "name", "small" },
                { "items", {"folder1", "folder2"} }
            },
            {
                { "name", "ntfs" },
                { "items", {"$RECYCLE.BIN", "System Volume Information"} }
            }
        }}
    };

    std::ofstream file(configFilename, std::fstream::out);
    file << std::setw(4) << j << std::endl;

    std::cout << "new config file generated please see and edit: " + configFilename << std::endl;
}

void getParams(int _argc, char* _argv[])
{
    int fixedParamIdx = 1;

    for(int run = 1; run < _argc; run++)
    {
        if(_argv[run][0] == '-')
        {
            for(int fly = 1; fly < std::string(_argv[run]).length(); fly++)
            {
                switch(_argv[run][fly])
                {
                    case 'h': usage(); exit(1); break;
                    case 'c': writeDefaultConfigFile(); exit(1); break;
                    case 'x': isExclude = true; break;
                    case 'b': isBash = true; break;
                    default:  
                        std::cerr << _argv[run][fly] << " is no valid flag" << std::endl;
                        usage();
                        exit(-1); 
                    break; 
                }
            }
        }
        else
        {
            if(fixedParamIdx == 1)
            {
                fixedParamIdx++;
                source = _argv[run];
            }
            else if (fixedParamIdx == 2)
            {
                fixedParamIdx++;
                destination = _argv[run];
            }
            else 
            {
                paramFolder.push_back(_argv[run]);
            }
        }
    }

    if(fixedParamIdx < 3)
    {
        std::cerr << "too few arguments given" << std::endl;
        usage();
        exit(-1);
    }
}

void addFolderBundleFromJson(json _j)
{
    for(int run = 0; _j[run] != nullptr; run++)
    {
        folder.push_back(_j[run]);
    }
}

void readRsyncFlagsFromJson(json _j)
{
    if(_j["rsyncflags"] == nullptr)
    {
        std::cerr << "rsyncflags are not defined in configfile" << std::endl;
        exit(-1);
    }

    for(int run = 0; _j["rsyncflags"][run] != nullptr; run++)
    {
        rsyncArgs.push_back(_j["rsyncflags"][run]);
    }
}

void buildPathFromJson(json _j, std::string _name)
{
    std::string path;

    if((_j["path"] == nullptr) || (_j["path"] == ""))
    {
        std::cerr << "path of " << _j["name"] << " is not defined in config file" << std::endl;
        exit(-1);
    } 

    if((_j["type"] == nullptr) || (_j["type"] == ""))
    {
        std::cerr << "type of " << _j["name"] << " is not defined in config file" << std::endl;
        exit(-1);
    }

    path = _j["path"];
    if(path[path.length() -1] != '/')
    {
        path += "/";
    }

    if(_j["type"] == "ssh")
    {
       rsyncArgs.push_back("-e");
       rsyncArgs.push_back("ssh");
    }

    rsyncArgs.push_back(path);
}

void readSourceAndDestinationFromJson(json _j)
{
    for(int run = 0; _j["endpoint"][run] != nullptr; run++)
    {
        json currentJson = _j["endpoint"][run];
        if(currentJson["name"] == source)
        {
            buildPathFromJson(currentJson, source);
        }
    }

    // two for loops to respect the given order
    for(int run = 0; _j["endpoint"][run] != nullptr; run++)
    {
        json currentJson = _j["endpoint"][run];
        if(currentJson["name"] == destination)
        {
            buildPathFromJson(currentJson, destination);
        }
    }

    if (source == "")
    {
        std::cerr << "source \"" << source << "\" not found in endpoints, please check config file" << std::endl;
        exit(-1);
    }

    if (destination == "")
    {
        std::cerr << "destination \"" << destination << "\" not found in endpoints, please check config file" << std::endl;
        exit(-1);
    }
}

void readFolderBundleFromJson(json _j)
{
    if(_j["bundle"] == nullptr)
    {
        std::cerr << "bundle not defined in config file" << std::endl;
        exit(-1);
    }

    _j = _j["bundle"];
    for(int run = 0; run < paramFolder.size(); run++)
    {
        bool found = false;
        for(int fly = 0; _j[fly] != nullptr; fly++)
        {
            if(_j[fly]["name"] == paramFolder[run])
            {
                found = true;
                addFolderBundleFromJson(_j[fly]["items"]);
            }
        }

        if(!found)
        {
            folder.push_back(paramFolder[run]);
        }
    }
}

void readJsonConfigFile()
{
    std::ifstream file(configFilename);
    json j;
    file >> j;

    rsyncArgs.push_back("rsync");
    readRsyncFlagsFromJson(j);
    readSourceAndDestinationFromJson(j);
    readFolderBundleFromJson(j);
}

void buildRsyncCommandArguments()
{
    for(int run = 0; run < folder.size(); run++)
    {
        if(isExclude)
        {
            rsyncArgs.push_back("--exclude=" + folder[run]);
        }
        else
        {
            rsyncArgs.push_back("--include=" + folder[run]);
            rsyncArgs.push_back("--include=" + folder[run] + "/**");
        }
    }

    if(!isExclude)
    {
         rsyncArgs.push_back("--exclude=*");
    }
}

std::vector<std::string> splitString(std::string _text, char _delimiter, bool saveQuoted)
{
    std::vector<std::string> result;
    std::string current;
    bool inQuote = false;

    for(int run = 0; run < _text.length(); run++)
    {
        if(_text[run] == '"')
        {
            inQuote = !inQuote;
        }

        if(_text[run] == _delimiter && (!saveQuoted || !inQuote))
        {
            if(current != "")
            {
                result.push_back(current);
            }            
            current = "";
        }
        else
        {
            current += _text[run];
        }
    }

    if(current != "")
    {
        result.push_back(current);
    }   

    return result;
}

void executeRsync()
{
    std::vector<char*> cstrings;   
    cstrings.reserve(rsyncArgs.size() + 1);

    for(int run = 0; run < rsyncArgs.size(); run++)
    {
        cstrings.push_back(&rsyncArgs[run][0]);
    }

    cstrings.push_back(NULL);

    std::cerr << execvp("rsync", cstrings.data()) << std::endl;
    std::cerr << "failed to launch rsync errno: " << errno << std::endl;
    exit(-1);
}

void showBashCommand()
{
    std::string command;
    for(int run = 0; run < rsyncArgs.size(); run++)
    {
        if(std::count(rsyncArgs[run].begin(), rsyncArgs[run].end(), ' '))
        {
            command += " \"" + rsyncArgs[run] + "\"";
        }
        else
        {
            command += " " + rsyncArgs[run];
        }
    }

    std::cout << command << std::endl;  
}

int main(int argc, char* argv[])
{
    configFilename = getenv("HOME") + std::string("/") + configFilename;
    std::ifstream file(configFilename);
    if(!file.good())
    {
        writeDefaultConfigFile();
        exit(1);
    }

    getParams(argc, argv);
    
    readJsonConfigFile();
    buildRsyncCommandArguments();
    if(isBash)
    {
        showBashCommand();
    }

    if(!isBash)
    {
        executeRsync();   
    }
}