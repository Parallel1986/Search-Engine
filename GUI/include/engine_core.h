//***************************************************************************//
//  This file contains the main core of search engine. It links together all //
// other modules of the engine and contains engine's configurations.         //
//***************************************************************************//
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include <QObject>
#include <QtConcurrent/QtConcurrent>

#include "./converter_json.h"
#include "./inverted_index.h"
#include "./file_index.h"

//Structure for accordance of files' IDs and its paths
struct FileIDTable
{
    friend FileErrors Loader::checkFilePath(QString&);
    FileIDTable(int count) : size(count), id(new int[count]),
    file_path(new QString[count]), err(new FileErrors[count]){}
    ~FileIDTable()
    {
        if (id != nullptr)
            delete[] id;
        if (err != nullptr)
            delete[] err;
    }
    int size = 0;           //Size of files' array
    int* id = nullptr;      //Array of file's ID
    QString *file_path = nullptr;  //Array of file's path
    FileErrors* err = nullptr;;              //Array of erros acquired if file could not be opened

    void clear()            //Clears the structure
    {
        if (id != nullptr)
        {
            delete[] id;
            id = nullptr;
        }
        if (file_path != nullptr)
        {
            delete[] file_path;
            file_path = nullptr;
        }
        if (err != nullptr)
        {
            delete[] err;
            err = nullptr;
        }
    }
private:
    FileIDTable(){};
    FileIDTable(const FileIDTable&){};
};

struct FileIDFrame
{
    int id;
    QString file_path;
    FileErrors err;
};

//Structure for accordance of requests and its IDs
struct RequestIDTable
{
    RequestIDTable(int count) : size(count), id(new int[count]), requests(new QString[count]){}
    ~RequestIDTable()
    {
        if (id != nullptr)
            delete[] id;
        if (requests != nullptr)
            delete[] requests;
    }
    int size = 0;           //size of requests' array
    int* id = nullptr;      //Array of requests' IDs
    QString* requests;    //Array of requests

    void clear()            //Clears the structure
    {
        if (id != nullptr)
        {
            delete[] id;
            id = nullptr;
        }
        if (requests != nullptr)
        {
            delete[] requests;
        }
    }
private:
    RequestIDTable(){};
    RequestIDTable(const RequestIDTable&);
};

enum EngineMode
{
    STANDARD,       //Standard mode
    AUTOCONFIG,     //Autoconfigure configurations' file
    MANUAL,         //Mode without configurations' and requests' file
    NO_CONFIG,      //Mode without configurations' file
    NO_REQUESTS     //Mode without requests' file
};

//Main core class
class EngineCore : public QObject
{
    Q_OBJECT
public:
    EngineCore();

    virtual ~EngineCore();

    /**
     * @brief Gets the status of search engine
     * @return Returns a byte with engine's status that are described
     * in the enum ConverterStatus
     */
    char getEngineStatus();

    /**
     * @return Returns true if the engine is initialised
     * by the config.json and requests.json
     */
    bool isInitialized();

    /**
     * @return Returns true if the engine is initialised
     * by the config.json
     */
    bool isConfigInitialized();

    /**
     * @return Returns true if the engine is initialised
     * by the requests.json
     */
    bool isRequestsInitialized();

    /**
     * @brief Create configurations' file with given settings
     */
    void generateConfigFile(QStringList, int);

    /**
     * @brief Gets list of requests
     * @return list of requests
     */
    QStringList getRequestsList();

    /**
     * @brief Gets list of files for search
     * @return list of files
     */
    QStringList getFiles();

    /**
     * @brief Allows to get accordance files' table
     * @return table of accordance files' paths and its IDs
     */
    FileIDTable* getFilesIDTable();

    /**
     * @brief Allows to get accordance requests' table
     * @return table of accordance requests and its IDs
     */
    RequestIDTable* getRequestsIDTable();

    /**
     * @brief Checking if GUI is used
     * @return true if GUI is active
     */
    bool isUseUI() const;

    void setUI();

signals:
    void reloadFilePaths(QStringList);          //Is emited when files' paths updated
    void configPathChanged(QString);            //Is emited when path to configurations' file is changed
///**/void openConfigRequest();                   //Is emited?
    void requestsPathChanged (QString);         //Is emited when path to requests' file is changed
/**/void answersPathChanged (QString);          //Is emited when path to answers' file is chamged
    void searchResult(QList<QList<RelativeIndex>>); //Is emited when search result is ready
    void requestsLoaded(QStringList);           //Is emited when requests are changed
/**/void configsLoaded(ConfigList);             //Sends configurations
/**/void configsError(char);                    //Is emited when error in configuration happens
//    void searchFilesLoaded(QStringList);        //Is emited when files for search are changed
    void updateStatus(char);                    //Is emited when engine status is changed

public slots:
    void addSearchFile(QString);        //Adds path to file for search in the end of files' list
    void removeSearchFile(QString);     //Removes file for search from files' list
    void setMode(EngineMode);           //Sets a mode of the search engine
    void addRequest(QString);           //Adds requests to the end of requests' list
    void removeRequest(QString);        //Removes request from the requests' list
    void search();                      //To make search with existing conditions
/*?*/void checkConfigPath(char);        //Checks correction of configurations' file
/*?*/void checkRequestsPath(char);      //Checks correction of requests' file
    void setConfigPath(QString);        //Sets path to config.json when it is changed
    void setRequestsPath(QString);      //Sets path to requests.json when it is changed
    void setAnswersPath(QString);       //Sets path to answers.json when it is changed
    void initialize();                  //Initialise the engine
    void setMaxRequests(int);           //Sets response limit when it is changed
    void saveResult();                  //Save search result
/**/void saveResultAsText();            //Save search result as text file
    void configPathUpdated(QString);    //Process changing of configurations' file path
    void requestsPathUpdated(QString);  //Process changing of requests' file path
    void answersPathUpdated(QString);   //Process changing of answers' file path

private:
    void makeFilesIDTable(QStringList&);            //Creates table of accordance files' paths and its IDs
    void makeRequestsIDdTable(QStringList&);        //Creates table of accordance requests and its IDs
/**/void configError();                 //Proccess configurations' error
/**/void requestsError();               //Proccess requests' error
    void initializeConfig();            //Loads configuration from the config.json file
    void initializeRequests();          //Loading data from the requests.json file
    void initializeCheck();             //Checking initialization

    ConverterJSON* converter = nullptr;         //Pointer to converter class
    InvertedIndex* index = nullptr;             //Pointer to inverted index class
    SearchServer* server = nullptr;             //Pointer to search server class
    QList<QList<RelativeIndex>> search_result;  //Results of the search
    QString engine_name = "Search Engine";      //Search engine name
    QString engine_version = "0.2.1";           //Search engine version
    int max_responses = MIN_RESPONSE;           //Maximum number of responses
    FileIDTable* files_id = nullptr;            //Table of files' IDs and error codes
    RequestIDTable* requests_id = nullptr;      //Table of requests IDs
    QStringList requests;                       //List of requests
    QStringList requests_add;                   //Lists of additional requests
    QStringList files_content;                  //List of files` content for search
    QStringList files_paths;                    //List of files` paths for search
    QStringList files_paths_add;                //List of additional files' pathes for search
    char engine_status = ConverterStatus::NO_ERRORS;    //Search engine state
    EngineMode mode = EngineMode::STANDARD;     //Mode of engine
    bool useUI = true;                         //Sets to use GUI
};

#endif // ENGINE_CORE_H
