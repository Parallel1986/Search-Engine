# Search-Engine
Search engine for finding words or phrases in set of documents. It is requires existing of config.json and requests.json files for proper work, except you are using manual mode or GUI.

Config.json file must contain two fields: "configuration" and "files". "configuration" field must contain max_pesponse field with positive value. "files" fild must contain list of files for search

![config.json](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-18_15-21-04.png)

Requests.json file must contain field "requests" with list of requests

![requests.json](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-18_15-21-50.png)

Results of search will be saved in answers.json file in net format: feild answers that contains list of fields named as "Request###", where ### is number of request in a list of requests from requests.json file. Each Request field will contain list of results as two fields,"doc_id" and "rank". doc_id is a documents number in the list of files and rank presents relevance of the document. If request wasn`t found, Request field will contain only one field "result" with value false.

![answers.json](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-18_15-28-46.png)

# Requirements
Search engine requires:
- C++ 17 standard compiler
- CMake not less than 3.5 version
- Qt 5.15 version (Widgets, Core)

# Installation
Firs of all, to install search engine you need to clone git repository by command:
```
git -clone https://github.com/Parallel1986/Search-Engine
```
### Unix users
Execute this commands:
```
cmake -B <build-directory> -G 'Unix Makefiles'
cd <build-directory>
make
```
```<build-directory>``` is a path to build's directory
### MinGW users
```
Execute this commands:
cmake -B <build-directory> -G 'MinGW Makefiles'
cd <build-directory>
mingw32-make
```
```<build-directory>``` is a path to build's directory
### Ninja users
Execute this commands:
```
cmake -B <build-directory> -G 'Ninja'
cd <build-directory>
ninja
```
```<build-directory>``` is a path to build's directory
### Windows users
If You use MSVC compiler then You should generate solution file for MS Visual studio by next command:
```
cmake -B <solution-directory> -G 'Visual Studio <version>'
```
```<solution-directory>``` is path where solution will be build

```<version>``` is version of your compiler. It could be:
- 17 2022
- 16 2019
- 15 2017
After the solution will be created, you should build executable file by using Your Visual studio

Also, may be required to deploy Qt files using windeployqt

# Avilable modes of search
Search engine supports four modes of search:
## Standard.
Search engine loads settings from config.json file, loads requests from requests.json and then saves search results to answers.json
## No config mode.
Search engine does not use configurations' file. Instead, it use response limit and files given with command line. If these arguments won't be given, search engine will be terminated with error code -1
## No requests mode.
Search engine does not use requests file. Instead, it use requests that are given with command line. If these arguments won't be given, search engine will be terminated with error code -1
## Manual mode.
This mode combines previous two modes, search engine does not use configurations' and requests' files, it is required to set response limit and files for search and requests via command line.
# Comand line commands
Command line supports next type of commands:

```-c <path_to_file>``` to set specific configuration file, required file's path with next argument.

```-r <path_to_file>``` to set specific requests file, required file's path with next argument.

```-a <path_to_file>``` to set specific answers file, required file's path with next argument.

```-mr <response_limit>``` to set limit of responses, required number with next argument.

```-fa <path_to_file>``` to add file for search to the end of the files' list, required file's path with next argument (multiple paths are allowed).

```-ra <request>``` to add request to end of the list, required request with next argument (multiple requests are allowed).

```-cg``` to generate configuration file with given response limit and files.

```-mm``` to switch to manual mode.

```-nc``` to do not use configurations' file.

```-nr``` to do not use requests' file.

```-ui``` to use graphical user interface.

# GUI
Search engine also has graphical user interface. It allows user to interact with engine settings and requests before commiting search.

![GUI](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-33.png)

First section presents paths to configuration, requests and answers files. Also, these files could be changed by pressing "Open..." button

![Paths section](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-31.png)

"Generate" button allows to generate minimal configuration file with empty list of files for search. Response limit will be set to current(default value is 5) and engine`s name will be "Autogen Name" as version "Autogen V0.1.0".

![Generate config button](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-30.png)

Next section is configuration. It shows loaded from configuration file settings, such as engine`s name, version, response limit. Also it has a mode selector that allows to change engine`s mode. Save button allows You to save current settings to the configuration file(by default config.json) includinf current files` list.

![Settings section](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-32.png)

File section presents list of files for search that are loaded from configuration file. You can add new ones or delete existing by using "Add..." and "Delete" buttons respectively.

![File section](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-34.png)

Request section, like the file section, contains list of requests that are loaded from requests` file (by default requests.json). You can add new ones or delete existing by using "Add..." and "Delete" buttons respectively.

![Requests section](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-35.png)

The last section contains search results and "Search", "Save" and "Save as text" buttons. "Search" button will be inactive in case if some of the critical components are missing, such as requests or files for search or inapropriate configuration/requests files are chosen. "Save" button will save search results to answers file (by default answers.json) and "Save as text" button will save results as a text file in format more comfortable for reading. Both these button will become active as sonn as search will be commited.

![Search section](https://github.com/Parallel1986/Search-Engine/blob/main/Readme_files/2023-10-17_16-38-36.png)
