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

#include <map>
#include "../include/file_index.h"
#include "../include/exceptions.h"
#include <QObject>
#include <QList>
#include <QDir>

#define DEFAULT_CONFIGS "config.json"
#define DEFAULT_REQUESTS "requests.json"
#define DEFAULT_ANSWERS "answers.json"

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
    QString enegine_name,
        engine_version;
    int max_responses;
    QList<QString> files;
};

//Status of the converter
enum ConverterStatus
{
    CONFIG_MISSED           = 1,
    CONFIG_FIELD_MISSED     = 2,
    SEARCH_FILES_MISSED     = 4,
    ENGINE_NAME_MISSED      = 8,
    ENGINE_VERSION_MISSED   = 16,
    MAX_RESPONSES_MISSED    = 32,
    REQUESTS_MISSED         = 64,
    REQUESTS_EMPTY          = 128,
    NO_CONFIG_ERRORS        = 63,
    RESET_CONFIG_ERRORS     = 192,
    NO_REQUESTS_ERRORS      = 192,
    RESET_REQUEST_ERRORS    = 63,
    NO_ERRORS               = 0
};

namespace Loader
{
    void LoadFileContent(QStringList& result,const QStringList& source);
    FileErrors checkFilePath(QString&);
}

class ConverterJSON : public QObject
{
    Q_OBJECT
public:
    ConverterJSON() = default;
    ~ConverterJSON();

    void putAnswersToJSON(QList<QList<RelativeIndex>> answers);
    void putSettingsToConfig(const ConfigList,QString);
    void putRequestsToJSON(const QList<QString>);

    char configCorrectionCheck() const;
    char requestsCorrectionCheck() const;

    int getResponsesLimit();
    QStringList getTextOfDocuments();
    QStringList getRequests();
    QStringList getFilesPaths();
    QString getEngineName();
    QString getEngineVersion();
    QString getConfigsPath() const;
    QString getRequestsPath() const;
    QString getAnswersPath() const;

public slots:
    void changeConfigPath(QString);
    void changeRequestsPath(QString);
    void changeAnswersPath(QString);

signals:
    void configPathChanged(QString);
    void requestsPathChanged(QString);
    void answersPathChanged(QString);
    void sendError(FileError);

private:
    QString makeRequestStringNumber(std::size_t) const;
    bool loadConfigs();
    bool loadRequests();
    bool isPathToFile(QString path) const;
    bool isPathToDirectory(QString path) const;

    QString config_file_path = DEFAULT_CONFIGS;
    QString requests_file_path = DEFAULT_REQUESTS;
    QString answers_file_path = DEFAULT_ANSWERS;
    bool config_is_loaded = false;
    bool requests_is_loaded = false;
    QJsonDocument* configuration = nullptr;
    QJsonDocument* requests = nullptr;
};
#endif
