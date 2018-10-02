#include <stdio.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include <fstream>
#include <vector>
#include <unistd.h>

using json = nlohmann::json;

std::string configFilename = "profile.json";
std::string source, sourceName;
std::string destination, destinationName;
std::string rsyncFlags;
bool isExclude = false;
bool isDebug = false;
std::vector<std::string> paramFolder;
std::vector<std::string> folder;

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

    rsyncFlags = _j["rsyncflags"];

    if(isDebug)
    {
        std::cout << "rsyncflags:\n\t" << rsyncFlags << std::endl;
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
            source = buildPathFromJson(currentJson, sourceName);
        }
        else if (currentJson["name"] == destinationName)
        {
            destination = buildPathFromJson(currentJson, destinationName);
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
    
    if(isDebug)
    {
        std::cout << "source:\n\t" << source << std::endl;
        std::cout << "destination:\n\t" << destination << std::endl;
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

    if(isDebug)
    {
        std::cout << "folder: " << std::endl;
        for(int run = 0; run < folder.size(); run++)
        {
            std::cout << "\t" << folder[run] << std::endl;
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

std::string buildRsyncCommandArguments()
{
    std::string command;
    command = rsyncFlags + " " + source + " " + destination;

    for(int run = 0; run < folder.size(); run++)
    {
        if(folder[run].find(" ") >= 0)
        {
            folder[run] = "\"" + folder[run] + "\"";
        }

        if(isExclude)
        {
            command += " --exclude=" + folder[run];
        }
        else
        {
            command += " --include=" + folder[run];
            command += " --include=" + folder[run] + "/**";
        }
    }

    if(!isExclude)
    {
        command += " --exclude=*";
    }

    if(isDebug)
    {
         std::cout << "command:\n\t" << "rsync " << command << std::endl;
    }

    return command;
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

char** vectorToCString(std::vector<std::string> _v)
{

}

int main(int argc, char* argv[])
{
    getParams(argc, argv);
    readJsonConfigFile();
    std::string args = buildRsyncCommandArguments();

    // std::string args = "word1 word2 \"super duper one word\"";
    std::vector<std::string> argss = splitString(args, ' ', true);

    std::vector<char*> cstrings;   
    cstrings.reserve(argss.size());

    for(auto& s: argss)
        cstrings.push_back(&s[0]);

    // execve("rsync", cstrings.data(), NULL);
    char *temp[] = {"--help"};
    execve("rsync", temp, NULL);
    
    // exit(1);
}

// # rsync -auvz --progress /mnt/y/tmp 
// #   --exclude='$RECYCLE.BIN' 
// #   --exclude='System Volume Information' 
// #   -e ssh drtosh@drtosh.de:/home/drtosh/stuff/tmp