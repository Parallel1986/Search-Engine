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

enum FileErrors
{
    NO_ERR,
    READ_ERR,
    ACSESS_ERR,
    NOT_A_FILE,
    NOT_EXIST,
    WRITE_ERR
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
    void LoadFileContent(QStringList& result,const QStringList& source);  //Loads content of files from *source* to *result*
    FileErrors checkFilePath(QString&);        //Checks correction of the path
}


class ConverterJSON : public QObject
{
    Q_OBJECT
public:
//    static ConverterJSON& getInstance();
    ConverterJSON() = default;
    ~ConverterJSON();

    /**
    * @brief Gets a file's content
    * @return Returns a list with file's content
    * that is listed in config.json file
    */
    QStringList getTextDocuments();

	/**
    * @brief Method reads the field "max_responses" to get a limit
    * of the answers' number for each request
    * @return Returns a number of the response limit
	*/
    int getResponsesLimit();

	/**
    * @brief Method of gets requests from the requests.json file
    * @return Returns a list of requests from the requests.json file
	*/
    QStringList getRequests();

	/**
    * @brief Puts to the answers.json file the result of search requests
	*/
    void putAnswers(QList<QList<RelativeIndex>>
		answers);

    /**
    * @brief Puts settings to the config.json file
    */
    void putConfig(const ConfigList,QString);

    /**
     * @brief Saves requests to the requests.json
     */
    void putRequests(const QList<QString>);

    /**
     * @brief Gets the name of the search engine
     * @return Returns name of the search engine from the config.json
     */
    QString getEngineName();

    /**
     * @brief Gets the version of the search engine
     * @return Returns version of the search engine from the config.json
     */
    QString getEngineVersion();

    /**
     * @brief Gets the list of files' content
     * @return Returns list of files' content from files
     * that are included in the config.json
     */
    QStringList getFilesPaths();

    /**
     * @brief Gets path to requests.json file
     * @return Returns the path of the requests.json file
     */
    QString getRequestsPath();

    /**
     * @brief Checks configurations' file for errors
     * @return char in format of ConvewrterStatus
     */
    char configCorrectionCheck();

    /**
     * @brief Checking requests' file for errors
     * @return char in format of ConvewrterStatus
     */
    char requestsCorrectionCheck();


    QString getConfigsPath() const;
    QString getRequestsPath() const;
    QString getAnswersPath() const;

public slots:

    void changeConfigPath(QString);           //Changes path to config.json
    void changeRequestsPath(QString);         //Changes path to requests.json
    void changeAnswersPath(QString);          //Changes path to answers.json

signals:
    void configPathChanged(QString);          //Is emited when path to configurations' file is changed successfully
    void configLoaded(char);                  //Is emited when configuration is loaded from config.json
    void requestsPathChanged(QString);        //Is emited when requests' file is loaded successfully
    void requestsLoaded(char);                //Is emited when requests are loaded from requests.json
    void answersPathChanged(QString);         //Is emited when answers' path changed successfully
/**/void answersSaved();                      //Is emited when answers is successfully saved
/**/void answersSaveError();                  //Is emited when answers is not saved
/*?*/void configUpdated(char);                 //Is emited when configurations is succrsfully changed
/*?*/void requestsUpdated(char);               //Is emited when requests is succrsfully changed
    void fileOpenFailure(QString);            //Is emited when can not open a file

private:
//    ConverterJSON() = default;                //Removing constructor for the Singletone pattern
    QString makeRequestNumber(std::size_t);   //Generates string of a request for writing to answers.json
    bool loadConfigs();                       //Loads configuration from the configurations' file
    bool loadRequests();                      //Loads requests from  requests' file
//    static ConverterJSON* instance;           //Instance of the converter
    QString config_file_path = "config.json";       //Configuration file`s path
    QString requests_file_path = "requests.json";   //Requests file`s path
    QString answers_file_path = "answers.json";     //Answers file`s path
    bool config_loaded = false;               //True if configurations loaded already
    bool requests_loaded = false;             //True if requests loaded already
    QJsonDocument* configuration = nullptr;   //Configurations as JSON document that are loaded from the file
    QJsonDocument* requests = nullptr;        //Requests as JSON document that are loaded from the file
};

//ConverterJSON* ConverterJSON::instance = nullptr;
#endif
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

//    void Initialize();                      //Initialise the engine
//    void SetMaxRequests(int);               //Sets response limit when it is changed
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
