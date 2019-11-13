# tonoka
Windows [![Build Status](https://home.draconpern.com:8081/buildStatus/icon?job=tonoka.win32.release)](https://home.draconpern.com:8081/job/tonoka.win32.release/) OS X [![Build Status](https://home.draconpern.com:8081/buildStatus/icon?job=tonoka.osx.release)](https://home.draconpern.com:8081/job/tonoka.osx.release/) Linux [![Build Status](https://home.draconpern.com:8081/buildStatus/icon?job=tonoka.linux.debug)](https://home.draconpern.com:8081/job/tonoka.linux.debug/)

Cross-platform software for performing PACS migration using C-STORE

- Supports Windows Vista and above including Windows 10
- Supported on OS X
- Works on Linux
- Reads dicom files directly from hard drive without needing old PACS software
- Fast multithreaded operations
- Stop migration midway and pick up where you left off
- Perform migration over time by getting new studies
- No dll's need to be distributed.
- Native, no Java required.

## Download
Binary http://www.draconpern.com/software/tonoka

Source https://github.com/DraconPern/tonoka

## Development notes
The program is http://utf8everywhere.org/
The data in sqlite is always utf8.
wxWidgets is utf32 on Windows and utf8 everywhere else.

## Requirements
- CMake http://www.cmake.org/download/
- XCode on OS X
- Visual Studio 2012 or higher on Windows
- gcc on Linux

## Third party dependency
- wxWidgets http://www.wxwidgets.org/ please extract under ./wxWidgets
- DCMTK http://dicom.offis.de/ please use snapshot or git, and extract under ./dcmtk
- boost http://www.boost.org/ please extract under ./boost
- Visual Leak Detector https://vld.codeplex.com/ installed for debug release for Windows
- zlib please extract under ./zlib on Windows
- openjpeg http://www.openjpeg.org please extract under ./openjpeg
- fmjpeg2koj https://github.com/DraconPern/fmjpeg2koj please extract under ./fmjpeg2koj

## Author
Ing-Long Eric Kuo <draconpern@hotmail.com>

## License
This software is licensed under the GPL.
