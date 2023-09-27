//***************************************************************************//
//  This file contains the main core of search engine. It links together all //
// other modules of the engine and contains engine's configurations.         //
//***************************************************************************//
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include <QObject>

#include "./converter_json.h"
#include "./inverted_index.h"
#include "./file_index.h"

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
     * @brief Adds requests to the end of requests' list
     */
    void addRequest(QString);

    /**
     * @brief Adds path to file for search in the end of files' list
     */
    void addSearchFile(QString);

    /**
     * @brief Create configurations' file with given settings
     */
    void generateConfigFile(QStringList, int);

    /**
     * @brief Sets a mode of the search engine
     */
    void setMode(EngineMode);

signals:
/**/void configPathChanged(QString);            //Is emited when path to configurations' file is changed
/**/void openConfigRequest();                   //Is emited?
/**/void requestsPathChanged (QString);         //Is emited when path to requests' file is changed
/**/void answersPathChanged (QString);          //Is emited when path to answers' file is chamged
    void searchResult(QList<QList<RelativeIndex>>);     //Is emited when search result is ready

public slots:
    void search();                              //To make search with existing conditions
/**/void checkConfigPath(char);                 //Checks correction of configurations' file
/**/void checkRequestsPath(char);               //Checks correction of requests' file
    void setConfigPath(QString);                //Sets path to config.json when it is changed
    void setRequestsPath(QString);              //Sets path to requests.json when it is changed
    void setAnswersPath(QString);               //Sets path to answers.json when it is changed
    void initialize();                          //Initialise the engine
    void setMaxRequests(int);                   //Sets response limit when it is changed

private:
/**/void configError();
/**/void requestsError();
    void initializeConfig();                    //Loads configuration from the config.json file
    void initializeRequests();                  //Loading data from the requests.json file
    void initializeCheck();                     //Checking initialization
    ConverterJSON* converter = nullptr;         //Pointer to converter class
    InvertedIndex* index = nullptr;             //Pointer to inverted index class
    SearchServer* server = nullptr;             //Pointer to search server class
    QList<QList<RelativeIndex>> search_result;  //Results of the search
    QString engine_name;                        //Search engine name
    QString engine_version;                     //Search engine version
    int max_responses = MIN_RESPONSE;           //Maximum number of responses
    QList<QString> requests;                    //List of requests
    QList<QString> file_list;                   //List of files` content for search
    QList<QString> files_paths;                 //List of files` paths for search
    char engine_status = ConverterStatus::NO_ERRORS;    //Search engine state
    EngineMode mode = EngineMode::STANDARD;     //Mode of engine
};

#endif // ENGINE_CORE_H

//Excluded
//    void LoadRequests();                        //Loads requests from the requests' file
//    void LoadConfigs();                         //Loads configuration from the config.json file
//    void LoadConfigs(QJsonDocument&);           //Loads configuration from a file from received JSON document
//    void LoadSearchFiles();                     //Loads pathes of files for index from the config.json file
//    void LoadSearchFiles(QJsonDocument&);       //Loads pathes of files for index from receivwed JSON document
