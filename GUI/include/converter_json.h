//***************************************************************************//
//  This file contains converter for JSON files and allow to read settings   //
//  and files for index and requests for serach from config.json and         //
//  requests.json. Also, it is made to save search results to answers.json.  //
//  Class based on Singleton pattern, so you can create only one instance of //
//  it. To create instance or get access to it use static method             //
//  GetInstance()                                                            //
//***************************************************************************//
#ifndef CONVERTER_JSON_HEADER
#define CONVERTER_JSON_HEADER

#include <string>
#include <vector>
#include <map>
#include "..\\include\\file_index.h"
#include <QObject>
#include <QList>
#include <QDir>

enum {
    MIN_RESPONSE    = 5
};

//List of configurations
struct ConfigList
{    
    QString enegine_name,           //Engine's name
            engine_ver;             //Engine's version
    int max_responses;              //Limit of responses for request
    QList<QString> files;           //List of file's content
};

//Status of the converter - 0 is no errors
enum ConverterStatus
{
    CONFIG_MISSED           = 1,    //Missing configurations' file
    CONFIG_FIELD_MISSED     = 2,    //Missing field "config" in configurations' file
    SEARCH_FILES_MISSED     = 4,    //Missing field "files" in configurations' file
    ENGINE_NAME_MISSED      = 8,    //Missing engine's name in configurations' file
    ENGINE_VERSION_MISSED   = 16,   //Missing engine's version in configurations' file
    MAX_RESPONSES_MISSED    = 32,   //Missing maximum response's count in configurations' file
    REQUESTS_MISSED         = 64,   //Missing requests' file
    REQUESTS_EMPTY          = 128,  //Missing requests in requests' file
    NO_CONFIG_ERRORS        = 63,   //No errors of the configurations' file
    NO_REQUESTS_ERRORS      = 192,  //No errors of the requests' file
    NO_ERRORS               = 0     //No errors
};

//Used to reset part of engine status
//enum ConverterStatusReset
//{
//    RESET_CONFIG_STATUS     = 192,  //To reset all configurations' statuses
//    RESET_REQUESTS_STATUS   = 63,   //To reset all requests' statuses
//    RESET_SEARCH_FILES      = 251,  //To reset status of files for serach
//    RESET_ALL               = 0     //To reset all statuses
//};

namespace Loader
{
    void LoadFileContent(QList<QString>&,const QList<QString>&);
}


class ConverterJSON : public QObject
{
    Q_OBJECT
public:
    static ConverterJSON& GetInstance();
    ~ConverterJSON();

    /**
    * @brief Gets a file's content
    * @return Returns a list with file's content
    * that is listed in config.json file
    */
    QList<QString> GetTextDocuments();

	/**
    * @brief Method reads the field "max_responses" to get a limit
    * of the answers' number for each request
    * @return Returns a number of the response limit
	*/
	int GetResponsesLimit();

	/**
    * @brief Method of gets requests from the requests.json file
    * @return Returns a list of requests from the requests.json file
	*/
    QList<QString> GetRequests();

	/**
    * @brief Puts to the answers.json file the result of search requests
	*/
    void PutAnswers(QList<QList<RelativeIndex>>
		answers);

    /**
    * @brief Puts settings to the config.json file
    */
    void PutConfig(const ConfigList,QString);

    /**
     * @brief Saves requests to the requests.json
     */
    void PutRequests(const QList<QString>);

    /**
     * @brief Gets the name of the search engine
     * @return Returns name of the search engine from the config.json
     */
    QString GetEngineName();

    /**
     * @brief Gets the version of the search engine
     * @return Returns version of the search engine from the config.json
     */
    QString GetEngineVersion();

    /**
     * @brief Gets the list of files' content
     * @return Returns list of files' content from files
     * are included in the config.json
     */
    QList<QString> GetFilesPaths();

    /**
     * @brief Gets path to requests.json file
     * @return Returns the path of the requests.json file
     */
    QString GetRequestsPath();

//Removed to engine_core.h
//    /**
//     * @brief Gets the status of search engine
//     * @return Returns a byte with engine's status that are described
//     * in the enum ConverterStatus
//     */
//    char GetEngineStatus();

//    /**
//     * @return Returns true if the engine is initialised
//     * by the config.json and requests.json
//     */
//    bool IsInitialized();

//    /**
//     * @return Returns true if the engine is initialised
//     * by the config.json
//     */
//    bool IsConfigInitialized();

//    /**
//     * @return Returns true if the engine is initialised
//     * by the requests.json
//     */
//    bool IsRequestsInitialized();

    char ConfigCorrectionCheck();             //Checking the config.json file for errors, returns a status
    char RequestsCorrectionCheck();           //Checking the requests.json file for errors, returns a status
public slots:
//Removed to engine_core.h
    void ChangeConfigPath(QString);           //Changes path to config.json
    void ChangeRequestsPath(QString);         //Changes path to requests.json
    void ChangeAnswersPath(QString);          //Changes path to answers.json
//    void Initialize();                      //Initialise the engine
//    void SetMaxRequests(int);               //Sets response limit when it is changed

signals:
    void ConfigLoaded(char);                  //Is emited when configuration is loaded from config.json
    void RequestsLoaded(char);                //Is emited when requests are loaded from requests.json
/**/void AnswersSaved();                      //Is emited when answers is successfully saved
/**/void AnswersSaveError();                  //Is emited when answers is not saved
    void ConfigUpdated(char);                 //Is emited when configurations is succrsfully changed
    void RequestsUpdated(char);               //Is emited when requests is succrsfully changed
    void AnswersUpdated();                    //Is emited when answers is succrsfully changed
    void FileOpenFailure(QString);            //Is emited when can not open a file

private:
    ConverterJSON() = default;                //Removing constructor for the Singletone pattern
    QString MakeRequestNumber(std::size_t);   //Generates string of a request for writing to answers.json
    bool LoadConfigs();                       //Loads configuration from the configurations' file
    bool LoadRequests();                      //Loads requests from  requests' file
    static ConverterJSON* instance;           //Incstance of the converter
    QString config_file_path = "./config.json";       //Configuration file`s path
    QString requests_file_path = "./requests.json";   //Requests file`s path
    QString answers_file_path = "./answers.json";     //Answers file`s path
    bool config_loaded = false;               //True if configurations loaded already
    bool requests_loaded = false;             //True if requests loaded already
    QJsonDocument* configuration = nullptr;   //Configurations as JSON document that are loaded from the file
    QJsonDocument* requests = nullptr;        //Requests as JSON document that are loaded from the file

//Removed to engine_core.h

//    void LoadSearchFiles();                 //Loads pathes of files for index from the config.json file
//    void LoadSearchFiles(QJsonDocument&);   //Loads pathes of files for index from receivwed JSON document
//    void InitializeConfig();                //Loads configuration from the config.json file
//    void InitializeRequests();              //Loading data from the requests.json file
//    void InitializeCheck();                 //Checking initialization
//    QString engine_name;                    //Search engine name
//    QString engine_version;                 //Search eengine version
//    int max_responses = 5;                  //Maximum number of responses
//    QList<QString> requests;                //List of requests
//    QList<QString> fileList;                //List of files for search
//    QList<QString> file_paths;              //List of files` paths for search
//    char engine_status = 0;                 //Search engine state
};
#endif
