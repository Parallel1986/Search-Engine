//***************************************************************************//
//  This file comtains the main core of search engine. It links together all //
// other modules of the engine and contains engine's configurations.         //
//***************************************************************************//
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include <QObject>

#include "./converter_json.h"
#include "./inverted_index.h"
#include "./file_index.h"


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
    char GetEngineStatus();

    /**
     * @return Returns true if the engine is initialised
     * by the config.json and requests.json
     */
    bool IsInitialized();

    /**
     * @return Returns true if the engine is initialised
     * by the config.json
     */
    bool IsConfigInitialized();

    /**
     * @return Returns true if the engine is initialised
     * by the requests.json
     */
    bool IsRequestsInitialized();

signals:
    void ConfigPathChanged(QString);
    void OpenConfigRequest();
    void RequestsPathChanged (QString);
    void AnswersPathChanged (QString);
    void SearchResult(QList<QList<RelativeIndex>>);

public slots:
    void Search();
    void CheckConfigPath();
    void SetConfigPath(QString);                //Sets path to config.json when it is changed
    void SetRequestsPath(QString);              //Sets path to requests.json when it is changed
    void SetAnswersPath(QString);               //Sets path to answers.json when it is changed
    void Initialize();                          //Initialise the engine
    void SetMaxRequests(int);                   //Sets response limit when it is changed

private:
    void ConfigError();
    void RequestsError();
    void InitializeConfig();                    //Loads configuration from the config.json file
    void InitializeRequests();                  //Loading data from the requests.json file
    void InitializeCheck();                     //Checking initialization
    ConverterJSON* converter = nullptr;
    InvertedIndex* index = nullptr;
    SearchServer* server = nullptr;
    QList<QString> requests_list;
    QList<QList<RelativeIndex>> search_result;
    QString engine_name;                        //Search engine name
    QString engine_version;                     //Search eengine version
    int max_responses = 5;                      //Maximum number of responses
    QList<QString> requests;                    //List of requests
    QList<QString> file_list;                   //List of files for search
    QList<QString> file_paths;                  //List of files` paths for search
    char engine_status = 0;                     //Search engine state
};

#endif // ENGINE_CORE_H

//Excluded
//    void LoadRequests();                        //Loads requests from the requests' file
//    void LoadConfigs();                         //Loads configuration from the config.json file
//    void LoadConfigs(QJsonDocument&);           //Loads configuration from a file from received JSON document
//    void LoadSearchFiles();                     //Loads pathes of files for index from the config.json file
//    void LoadSearchFiles(QJsonDocument&);       //Loads pathes of files for index from receivwed JSON document
