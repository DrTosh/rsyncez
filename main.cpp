#include <stdio.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

std::string source;
std::string destination;
std::string includes;
std::string excludes;
std::string includeFolder;
std::string clientpath = "/mnt/y/";
std::string serverpath = "-e ssh drtosh@drtosh.de:/home/drtosh/stuff/";

enum SYNCMODE { smPUSH, smPULL };

void usage()
{
    std::cout <<  "usage: sync [push|pull] [connection] [folder]\n" <<
        "sync add connection [name] [source] [type] [destination] [type]" <<
        "sync delete connection [name]" <<
        "sync update connection [name] -s [source] -s" << std::endl;
}

void getParams()
{

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

    if(_j["type"] == "ssh")
    {
        result = "-e ssh " + result;
    }

    return result;
}

int main(int argc, char* argv[])
{
    std::ifstream file("profile.json");

    json j;
    file >> j;

    // std::cout << j["test1"] << std::endl;
    std::string sourceName = "localdir";
    std::string destinationName = "serverdir";

    for(int run = 0; j["endpoint"][run] != nullptr; run++)
    {
        json currentJson = j["endpoint"][run];

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
    
    std::cout << source << std::endl;
    std::cout << destination << std::endl;

    exit(1);


    printf("[1] push to server\n");
    printf("[2] pull from server\n");
    printf("select a sync method: ");
    
    int sync_method = getchar();

    if(sync_method < 1 || sync_method > 2)
    {
        printf("\nno valid option selected\n");
        exit(-1);
    }

    printf("\n");

    if(sync_method == 1)
    {
        printf("push from %s to %s\n", clientpath.c_str(), serverpath.c_str());
        source = clientpath;
        destination = serverpath;
    } 
    else
    {
        printf("pull from %s to %s", serverpath.c_str(), clientpath.c_str());
        source = serverpath;
        destination = clientpath;
    }

    printf("[1] select some folder");
    printf("[2] deselect some folder");
    printf("[3] full sync");

    int target_method = getchar();   

    if(target_method < 1 || target_method > 3)
    {
        printf("no valid option selected");
        exit(-1);
    }

    std::string folder;
    scanf("%s", &folder);
    printf("%s", folder.c_str());

    for(int run = 0; run < 0; run++)
    {
        // $includeFolder=$includeFolder'--include=$run --include=$run/**'
        //     $excludeFolder=$excludeFolder'--exclude=$run'
    }
}



// if [ $target_method -eq 1 ]
// then
//     echo start sync folders: $folder
//     $includes=$includeFolder
//     $exludes="--exclude=*"
// elif [ $target_method -eq 2 ]
// then
//     echo start sync folders: $folder
//     $exludes="--exclude='$RECYCLE.BIN' --exclude='System Volume Information'"$excludeFolder
// else
//     echo start full sync
//     $exludes="--exclude='$RECYCLE.BIN' --exclude='System Volume Information'"
// fi

// exit -1

// ## full sync

// ## just a few folder
// echo rsync -auvz --progress 
//     $source 
//     $includes 
//     $exludes 
//     $destination


// # rsync -auvz --progress /mnt/y/tmp 
// #   --exclude='$RECYCLE.BIN' 
// #   --exclude='System Volume Information' 
// #   -e ssh drtosh@drtosh.de:/home/drtosh/stuff/tmp