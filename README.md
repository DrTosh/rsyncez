#RsyncEz [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/nlohmann/json/master/LICENSE.MIT)

## How To Use
## Default Json File
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
                    "System Volume Information"
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