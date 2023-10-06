# Search-Engine
This readme is work in progress
Seearch engine for finding words or phrases in set of documents

# Installation
To install search engine clone git repository by command:
```
git -clone https://github.com/Parallel1986/Search-Engine
```
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
```
-c <path_to_file>	 - to set specific configuration file, required file's path with next argument
-r <path_to_file>	 - to set specific requests file, required file's path with next argument
-a <path_to_file>	 - to set specific answers file, required file's path with next argument
```
```
-mr <response_limit> - to set limit of responses, required number with next argument
-fa <path_to_file>	 - to add file for search to the end of the files' list, required file's path with next argument (multiple paths are allowed)
-ra <request>		 - to add request to end of the list, required request with next argument (multiple requests are allowed)
```
```
-cg 				 - to generate configuration file with given response limit and files
```
```
-mm 				 - to switch to manual mode
-nc					 - to do not use configurations' file
-nr					 - to do not use requests' file
```
```
-ui					 - to use graphical user interface
```
#GUI
Search engine also has graphical user interface. It allows user to interact with engine settings and requests before commiting search.
Via GUI user can change configurations' file, requests' file, answers' file, set response limit and manually add files for search or requests.
Also it will present search results and allows to save it as standard Json file or as a text file.
