#include <stdio.h>
#include <iostream>
#include <string>

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
        "sync update connection [name] -s [source] -s" <<

}

void getParams()
{

}

int main(int argc, char* argv[])
{
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
// echo rsync -auvz --progress \
//     $source \
//     $includes \
//     $exludes \
//     $destination


// # rsync -auvz --progress /mnt/y/tmp \
// #   --exclude='$RECYCLE.BIN' \
// #   --exclude='System Volume Information' \
// #   -e ssh drtosh@drtosh.de:/home/drtosh/stuff/tmp