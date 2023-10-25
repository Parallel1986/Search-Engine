//***************************************************************************//
// This file contains the main core of search engine. It links together all  //
// other modules of the engine and contains engine's configurations.         //
//***************************************************************************//
#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include <iostream>
#include <QObject>
#include <QtConcurrent/QtConcurrent>

#include "converter_json.h"
#include "inverted_index.h"
#include "file_index.h"
#include "exceptions.h"

//Structures for accordance of files' IDs and its paths
struct FileIDFrame
{
    int id;
    QString file_path;
    FileErrors err;
};

struct FileIDTable
{
    friend FileErrors Loader::checkFilePath(QString&);
    FileIDTable(int count) : size(count), frame(new FileIDFrame[count]){}
    ~FileIDTable()
    {
        if (frame != nullptr)
            delete[] frame;
    }

    int size = 0;                   //Size of files' array
    FileIDFrame* frame = nullptr;
private:
    FileIDTable(){}
    FileIDTable(const FileIDTable&){}
};

//Structures for accordance of requests and its IDs
struct RequestFrame
{
    int id;
    QString request;
};

struct RequestIDTable
{
    RequestIDTable(int count) : size(count), frame(new RequestFrame[count]){}
    ~RequestIDTable()
    {
        if (frame != nullptr)
            delete[] frame;
    }

    int size = 0;           //size of requests' array
    RequestFrame* frame = nullptr;

private:
    RequestIDTable(){}
    RequestIDTable(const RequestIDTable&){}
};

enum EngineMode
{
    STANDARD,
    AUTOCONFIG,
    MANUAL,         //Mode without configurations' and requests' file
    NO_CONFIG,      //Mode without configurations' file
    NO_REQUESTS     //Mode without requests' file
};

class EngineCore : public QObject
{
    Q_OBJECT
public:
    EngineCore();
    virtual ~EngineCore();

    QStringList getRequestsList() const;
    QStringList getFiles() const;
    const FileIDTable* getFilesIDTable() const;
    const RequestIDTable* getRequestsIDTable() const;
    char getEngineStatus() const;

    bool isInitialized() const;
    bool isConfigInitialized() const;
    bool isRequestsInitialized() const;
    bool isUseUI() const;

    void generateConfigFile(QStringList, int);
    void setUI();

signals:
    void reloadFilePaths(QStringList);
    void configPathChanged(QString);
    void requestsPathChanged (QString);
    void answersPathChanged (QString);
    void searchResult(QList<QList<RelativeIndex>>, const FileIDTable*,const RequestIDTable*);
    void requestsLoaded(QStringList);
    void configsLoaded(ConfigList);
    void updateStatus(char);
    void showError(FileError);

public slots:
    void addSearchFile(QString);
    void removeSearchFile(QString);
    void setMode(EngineMode);
    void addRequest(QString);
    void removeRequest(QString);
    void setConfigPath(QString);
    void setRequestsPath(QString);
    void setAnswersPath(QString);
    void initialize();
    void setMaxRequests(int);
    void saveResult();
    void saveResultAsText();
    void configPathUpdated(QString);
    void requestsPathUpdated(QString);
    void answersPathUpdated(QString);
    void processError(FileError);
    void search();

private:
    void makeFilesIDTable(QStringList&);
    void makeRequestsIDdTable(QStringList&);
    void initializeConfig();
    void initializeRequests();
    void initializeCheck();

    ConverterJSON* converter = nullptr;
    InvertedIndex* index = nullptr;
    SearchServer* server = nullptr;
    FileIDTable* files_id = nullptr;
    RequestIDTable* requests_id = nullptr;
    QList<QList<RelativeIndex>> search_result;
    QString engine_name = "Search Engine";
    QString engine_version = "0.2.1";
    int max_responses = MIN_RESPONSE;
    QStringList requests;
    QStringList requests_add;
    QStringList files_content;
    QStringList files_paths;
    QStringList files_paths_add;
    char engine_status = ConverterStatus::NO_ERRORS;
    EngineMode mode = EngineMode::STANDARD;
    bool useUI = false;
};

#endif // ENGINE_CORE_H
