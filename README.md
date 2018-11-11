# RsyncEz [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/drtosh/rsyncez/master/LICENSE.MIT)

## About
Rsyncez is for simple use of rsync.
It Translates some commands to an original rsync command and executes it. All Output from rsync is piped through to your current command window.

## Linux 
All u gonna need is rsync installed. If u try to run rsyncez without, it will prompt a hint.

## Window 
To use it on Windows u will need some Linux subsystem like ubuntu bash or mingw for older windows versions.

## How To Use
RsyncEz commands are simplier and shorter than rsync commands. Its especially advantageous if you want to push or pull quickly a single folder from your data root. You need to specify two endpoints, which means, a folder on both sides which represents the data root where the files to sync are located. Aliases for Endpoint are stored in the default configuration file.

    rsyncez <Endpoint> <Endpoint> <Folderlist>

### Default Json File
Filelocation is "~/.rsyncez.json". A default one will be created automatically if there is no one. For a new default config file call rsyncez with the parameter "-c".

    {
        "bundle": [
            {
                "items": [
                    "folder1",
                    "folder2"
                ],
                "name": "small"
            },
            {
                "items": [
                    "$RECYCLE.BIN",
                    "System Vasd  olume Information"
                ],
                "name": "ntfs"
            }
        ],
        "endpoint": [
            {
                "name": "local",
                "path": "/home/username/files",
                "type": "local"
            },
            {
                "name": "server",
                "path": "address.com:/home/username/files",
                "type": "ssh"
            }
        ],
        "rsyncflags": [
            "-auvz",
            "--progress"
        ]
    }

### Example call
    rsyncez local server small

pushs folder1 and folder2, which are stored in bundle "small" (in default config file), over ssh to the given server location

    rsyncez server local small

pulls folder1 and folder2, which are stored in bundle "small" (in default config file), over ssh to the local location

### Example call with exclude
The Flag "-x" for RsyncEz transforms the call into a exclude only one. Means all folder in the folder of the endpoint were pushed or pulled except the given ones. The following example is used for NTFS device backup. It excludes the hidden folder "$RECYCLE.BIN" and "System Volume Information" which are stored in bundle "ntfs" (in default config file). 

    rsyncez local server -x ntfs

### Rsync Flags
flags inserted into the "rsyncflags" section will passed through to rsync it self.

### RsyncEz Flags
    -x    exclude folder instead of including
    -c    create default config file
    -b    only generates bash command without execution
    -h    shows help