#include <stdio.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include <fstream>
#include <vector>
#include <unistd.h>

using json = nlohmann::json;

std::string configFilename = "profile.json";
std::string sourceName;
std::string destinationName;

bool isExclude = false;
bool isDebug = false;
std::vector<std::string> rsyncFlags;
std::vector<std::string> paramFolder;
std::vector<std::string> folder;
std::vector<std::string> rsyncArgs;

void usage()
{
    std::cout <<  "usage: sync [source] [destination] [folder | folderbundle]" << std::endl <<
                  "  -x : folder and folderbundles are exluded, rest ist synced" << std::endl <<
                  "  -d : debug mode for more informations:" << std::endl;
}

void getParams(int _argc, char* _argv[])
{
    if(_argc < 3)
    {
        std::cerr << "too few arguments given" << std::endl;
        usage();
        exit(-1);
    }    

    sourceName = _argv[1];
    destinationName = _argv[2];

    for(int run = 3; run < _argc; run++)
    {
        if(std::string(_argv[run]) == "-x")
        {
            isExclude = true;
        }
        else if(std::string(_argv[run]) == "-d")
        {
            isDebug = true;
        }
        else 
        {
            paramFolder.push_back(_argv[run]);
        }
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
        rsyncFlags.push_back(_j["rsyncflags"][run]);
    }
}

std::string buildPathFromJson(json _j, std::string _name)
{
    std::string result;

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

    result = _j["path"];
    if(result[result.length() -1] != '/')
    {
        result += "/";
    }

    if(_j["type"] == "ssh")
    {
        result = "-e ssh " + result;
    }

    return result;
}

void readSourceAndDestinationFromJson(json _j)
{
    for(int run = 0; _j["endpoint"][run] != nullptr; run++)
    {
        json currentJson = _j["endpoint"][run];

        if(currentJson["name"] == sourceName)
        {
            hier weiter machen source und destination in args list einfuegen
            buildPathFromJson(currentJson, sourceName);
        }
        else if (currentJson["name"] == destinationName)
        {
            buildPathFromJson(currentJson, destinationName);
        }
    }

    if (source == "")
    {
        std::cerr << "source \"" << sourceName << "\" not found in endpoints, please check config file" << std::endl;
        exit(-1);
    }

    if (destination == "")
    {
        std::cerr << "destination \"" << destinationName << "\" not found in endpoints, please check config file" << std::endl;
        exit(-1);
    }
}

void readFolderBundleFromJson(json _j)
{
    if(_j["folderbundle"] == nullptr)
    {
        std::cerr << "folderbundle not defined in config file" << std::endl;
        exit(-1);
    }

    _j = _j["folderbundle"];
    for(int run = 0; run < paramFolder.size(); run++)
    {
        bool found = false;
        for(int fly = 0; _j[fly] != nullptr; fly++)
        {
            if(_j[fly]["name"] == paramFolder[run])
            {
                found = true;
                addFolderBundleFromJson(_j[fly]["folder"]);
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

    readRsyncFlagsFromJson(j);
    readSourceAndDestinationFromJson(j);
    readFolderBundleFromJson(j);
}

void buildRsyncCommandArguments()
{
    rsyncArgs.push_back("rsync");
    
    for(int run = 0; run < rsyncFlags.size(); run++)
    {
        rsyncArgs.push_back(rsyncFlags[run]);    
    }

    rsyncArgs.push_back(source);
    rsyncArgs.push_back(destination);

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
    cstrings.reserve(rsyncArgs.size());

    for(int run = 0; run < rsyncArgs.size(); run++)
    {
        cstrings.push_back(&rsyncArgs[run][0]);
    }

    execvp("rsync", cstrings.data());
    std::cerr << "failed to launch rsync errno: " << errno << std::endl;
    exit(-1);
}

void showDebugInfo()
{
    std::string command = "rsync";

    std::cout << "rsyncFlags:" << std::endl;
    for(int run = 0; run < rsyncFlags.size(); run++)
    {
        command += " " + rsyncFlags[run];
        std::cout << "\t" << rsyncFlags[run] << std::endl;
    }

    command += " " + source;
    command += " " + destination;
    std::cout << "source:\n\t" << source << std::endl;
    std::cout << "destination:\n\t" << destination << std::endl;

    std::cout << "folder: " << std::endl;
    for(int run = 0; run < folder.size(); run++)
    {
        if(folder[run].find(" ") > -1)
        {
            command += " \"" + folder[run] + "\"";
        }
        else
        {
            command += " " + folder[run];
        }
        std::cout << "\t" << folder[run] << std::endl;
    }

    std::cout << "rsyncArgs:" << std::endl;
    for(int run = 0; run < rsyncArgs.size(); run++)
    {
        std::cout << "\t" << rsyncArgs[run] << std::endl;
    }

    std::cout << "command:\n\t" << command << std::endl;

}

int main(int argc, char* argv[])
{
    getParams(argc, argv);
    readJsonConfigFile();
    buildRsyncCommandArguments();
    if(isDebug)
    {
        showDebugInfo();
    }
    executeRsync();   
}

// # rsync -auvz --progress /mnt/y/tmp 
// #   --exclude='$RECYCLE.BIN' 
// #   --exclude='System Volume Information' 
// #   -e ssh drtosh@drtosh.de:/home/drtosh/stuff/tmp