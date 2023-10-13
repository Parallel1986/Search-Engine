#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\\converter_json.h"
#include "..\\include\\file_index.h"

#include <fstream>
#include <exception>
#include <iostream>
#include <sstream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QtConcurrent/QtConcurrent>

//Destructor
ConverterJSON::~ConverterJSON()
{
    if (configuration)
    {
        delete configuration;
        configuration = nullptr;
    }
    if (requests)
    {
        delete requests;
        requests = nullptr;
    }
}

//Change path to a configurations' file
void ConverterJSON::changeConfigPath(QString new_path)
{
    if (config_file_path != new_path)
    {
        QFileInfo new_target(new_path);
        if (new_target.exists())
        {
            if (new_target.isFile())
            {
                if (new_target.isReadable())
                {
                    config_file_path = new_path;
                    config_loaded = false;
                    emit configPathChanged(config_file_path);
                }
                else
                {
                    emit sendError(EngineError(ExceptionType::ReadFileError,new_path,
                                    "Error while trying to read a file"));
                }
            }
            else if (new_target.isDir())
            {
                if (new_target.isReadable())
                {
                    config_file_path = new_path + "/config.json";
                    config_loaded = false;
                    emit configPathChanged(config_file_path);
                }
            }
            else
                emit sendError(EngineError(ExceptionType::OpenDirectoryError,new_path,
                                "Error while trying to open a directory"));
        }
        else
            emit sendError(EngineError(ExceptionType::FileNotExistError,new_path,
                            "File does not exist"));
    }
}

//Change path to a requests' file
void ConverterJSON::changeRequestsPath(QString new_path)
{
    if (requests_file_path != new_path)
    {
        QFileInfo new_target(new_path);
        if (new_target.exists())
        {
            if (new_target.isFile())
            {
                if (new_target.isReadable())
                {
                    requests_file_path = new_path;
                    requests_loaded = false;
                    emit requestsPathChanged(requests_file_path);
                }
                else
                    emit sendError(EngineError(ExceptionType::ReadFileError,new_path,
                                    "Error while trying to read a file"));
            }
            else if (new_target.isDir())
            {
                if (new_target.isReadable())
                {
                    requests_file_path = new_path + "/requests.json";
                    requests_loaded = false;
                    emit requestsPathChanged(requests_file_path);
                }
            }
            else
                emit sendError(EngineError(ExceptionType::OpenDirectoryError,new_path,
                                    "Error while trying to open a directory"));
        }
        else
            emit sendError(EngineError(ExceptionType::FileNotExistError,new_path,
                                    "Error while trying to read a file"));
    }
}

//Change path to a answers' file
void ConverterJSON::changeAnswersPath(QString new_path)
{
    if (answers_file_path != new_path)
    {
        QFileInfo new_target(new_path);
        if (new_target.exists())
        {
            if (new_target.isFile())
            {
                if (new_target.isWritable())
                {
                    answers_file_path = new_path;
                    emit answersPathChanged(answers_file_path);
                }
                else
                    emit sendError(EngineError(ExceptionType::WriteFileError,new_path,
                                    "Error while trying to write to a file"));
            }
            else if (new_target.isDir())
            {
                if (new_target.isReadable()
                    &&new_target.isWritable())
                {
                    answers_file_path = new_path + "/answers.json";
                    void answersPathChanged(QString);
                }
            }
            else
                emit sendError(EngineError(ExceptionType::WriteDirectoryError,new_path,
                                    "Error while trying to write to a directory"));
        }
        else if (new_target.isWritable())
        {
            if (new_target.isDir())
            {
                answers_file_path = new_path + "/answers.json";
                void answersPathChanged(QString);
            }
            else if (new_target.isFile())
            {
                answers_file_path = new_path;
                void answersPathChanged(QString);
            }
        }
        else
            emit sendError(EngineError(ExceptionType::WriteDirectoryError,new_path,
                            "Error while trying to write to a directory"));
    }
}

//Get documents' content
QStringList ConverterJSON::getTextDocuments()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        QStringList documents_text;
        Loader::LoadFileContent(documents_text,getFilesPaths());
        return documents_text;
    }
    return QList<QString>();
}

//Get responses' limit
int ConverterJSON::getResponsesLimit()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["max_responses"].toInt();
    }
    return MIN_RESPONSE;
}

//Get list of requests
QStringList ConverterJSON::getRequests()
{
    if (requests_loaded
    ||(!requests_loaded && loadRequests()))
    {
        QList<QString> req;
        auto json_requests_field = requests->object()["requests"].toArray();
        if (!json_requests_field.isEmpty())
        {
            for (auto it = json_requests_field.begin();
            it != json_requests_field.end();
            it++)
            {
                req.append(it->toString());
            }
        }
        return req;
    }
    return QList<QString>();
}

//Save answers to a answers' file
void ConverterJSON::putAnswers(QList<QList<RelativeIndex>> answers)
{
    QJsonDocument answers_template;
    QFile answer_file(answers_file_path);

    if (answer_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QJsonArray answer_array;
        int counter = 0;

        for (auto it = answers.begin(); it != answers.end(); it++, counter++)
        {
            if (it->size() == 0)
            {
                QJsonObject answer, result;
                result.insert("result", false);
                answer.insert(makeRequestNumber(counter),result);
                answer_array.append(answer);
            }
            else
            {
                QJsonObject answer, res;
                QJsonArray res_array;
                for(auto& pair : *it)
                {
                    res.insert("docid",pair.doc_id);
                    res.insert("rank",pair.rank);
                    res_array.append(res);
                }
                answer.insert(makeRequestNumber(counter),res_array);
                answer_array.append(answer);
            }
        }
        QJsonObject output;
        output.insert("answers",answer_array);
        answers_template.setObject(output);
        QByteArray out_stream;
        out_stream = answers_template.toJson(QJsonDocument::Indented);
        answer_file.write(out_stream);
        answer_file.close();
        emit answersSaved();
    }
    else
        emit sendError(EngineError(ExceptionType::WriteFileError,answers_file_path,
                        "Error while trying to write answers"));
}

//Save cofniguration to a configurations' file
void ConverterJSON::putConfig(const ConfigList settings, QString conf_file)
{
    QFileInfo target(conf_file);
    if (target.isWritable())
    {
        if ((target.exists()&&target.isDir())
            ||target.isDir())
        {
            conf_file = conf_file + "/config.json";
        }
        else if ((target.exists()&&!target.isFile())
                ||!target.isFile())
            {
                emit sendError(EngineError(ExceptionType::WriteFileError,conf_file,
                                "Error while trying to write to a file"));
                return;
            }
        emit configPathChanged(conf_file);
    }
    QFile config_file(conf_file);
    if (config_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        if (!configuration)
            configuration = new QJsonDocument();
        QJsonObject configs,output;
        QJsonArray json_files;

        configs.insert("name",settings.enegine_name);
        configs.insert("version",settings.engine_ver);
        configs.insert("max_responses", settings.max_responses);

        for(auto& file : settings.files)
        {
            json_files.append(file);
        }

        output.insert("config",configs);
        output.insert("files", json_files);

        configuration->setObject(output);
        QByteArray out_stream;
        out_stream = configuration->toJson(QJsonDocument::Indented);
        config_file.write(out_stream);
        config_file.close();

        config_loaded = true;
        config_file_path = conf_file;
//        emit configUpdated(ConverterStatus::NO_ERRORS);
    }
    else
        emit sendError(EngineError(ExceptionType::WriteDirectoryError,conf_file,
                        "Error while trying to write to a file"));
}

//Save requests to a requests' file
void ConverterJSON::putRequests(const QList<QString> in_requests)
{
    QFile requests_file(requests_file_path);
    if (requests_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (!requests)
            requests = new QJsonDocument();
        QJsonObject output;
        QJsonArray requests_list;

        for (auto& request:in_requests)
        {
            requests_list.append(request);
        }        
        output.insert("requests",requests_list);
        requests->setObject(output);
        QByteArray out_stream(requests->toJson(QJsonDocument::Indented));
        requests_file.write(out_stream);
        requests_file.close();
        requests_loaded = true;
    }
    else
        emit sendError(EngineError(ExceptionType::WriteDirectoryError,requests_file_path,
                        "Error while trying to write to a file"));
}

//Get search engine's name
QString ConverterJSON::getEngineName()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["name"].toString();
    }
    else
        return "Unknown";
}

//Get search engine's version
QString ConverterJSON::getEngineVersion()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["version"].toString();
    }
    else
        return "Unknown";
}

//Get pathes to files for search
QStringList ConverterJSON::getFilesPaths()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_fiels_field = configuration->object()["files"].toArray();
        QList<QString> file_paths;
        for (auto it = json_fiels_field.begin();it != json_fiels_field.end();it++)
            file_paths.append(it->toString());
        return file_paths;
    }
    else
        return QList<QString>();
}

//Make request number as a string
QString ConverterJSON::makeRequestNumber(std::size_t number) const
{
    QString num;
    num.setNum(number);

    if (number < 10)
        return QString("request00" + num);
    else if (number < 100)
        return QString("request0" + num);
    else
        return QString("request" + num);
}

//Check the configurations' file for errors
char ConverterJSON::configCorrectionCheck() const
{
    char result = ConverterStatus::NO_ERRORS;
    QFile config(config_file_path);
    if (config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
        config.close();

        if (!(configuration.isNull()||configuration.isEmpty()))
        {
            if (!configuration.object().isEmpty())
            {
                if (!configuration.object().contains("files")
                    || configuration.object()["files"].toArray().isEmpty())
                {
                    result |= ConverterStatus::SEARCH_FILES_MISSED;
                }

                if (configuration.object().contains("config"))
                {
                    auto json_config_field = configuration.object()["config"];
                    if (!json_config_field.toObject().contains("name"))
                        result |= ConverterStatus::ENGINE_NAME_MISSED;

                    if (!json_config_field.toObject().contains("version"))
                        result |= ConverterStatus::ENGINE_VERSION_MISSED;

                    if (!json_config_field.toObject().contains("max_responses"))
                        result |= ConverterStatus::MAX_RESPONSES_MISSED;
                }
                else result = ConverterStatus::CONFIG_FIELD_MISSED;
            }
            else
                result |= ConverterStatus::SEARCH_FILES_MISSED|ConverterStatus::CONFIG_FIELD_MISSED;
        }
        else
            result |= ConverterStatus::SEARCH_FILES_MISSED|ConverterStatus::CONFIG_FIELD_MISSED;
    }
    else
        result |= ConverterStatus::CONFIG_MISSED;

    return result;
}

//Check the requests' file for errors
char ConverterJSON::requestsCorrectionCheck() const
{
    char result = ConverterStatus::NO_ERRORS;
    QFile requests_file(requests_file_path);
    if (requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray in_stream;
        in_stream = requests_file.readAll();
        requests_file.close();

        QJsonDocument json_requests(QJsonDocument::fromJson(in_stream));

        if (json_requests.isNull())
            result |= ConverterStatus::REQUESTS_MISSED;
        else if (!json_requests.isObject()
                 || !json_requests.object().contains("requests"))
            result |= ConverterStatus::REQUESTS_EMPTY;
    }
    else
        result |= ConverterStatus::REQUESTS_MISSED;

    return result;
}

//Loading configurations' file
bool ConverterJSON::loadConfigs()
{
    if (!config_loaded
        && !(ConverterStatus::NO_CONFIG_ERRORS & configCorrectionCheck()))
    {
        QFile config_file(config_file_path);
        if (config_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            configuration = new QJsonDocument(QJsonDocument::fromJson(QByteArray(config_file.readAll())));
            config_file.close();
            config_loaded = true;
        }
        else
        {
            emit sendError(EngineError(ExceptionType::WriteDirectoryError,config_file_path,
                            "Error while trying to open a file"));
            config_loaded = false;
            return false;
        }
    }
    else if (!config_loaded)
        return false;
    return true;
}

//Loading requests' file
bool ConverterJSON::loadRequests()
{
    if (!requests_loaded
        && !(ConverterStatus::NO_REQUESTS_ERRORS & requestsCorrectionCheck()))
    {
        QFile requests_file(requests_file_path);
        if (requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            requests = new QJsonDocument(QJsonDocument::fromJson(QByteArray(requests_file.readAll().toLower())));
            requests_file.close();
            requests_loaded = true;
        }
        else
        {
            emit sendError(EngineError(ExceptionType::WriteDirectoryError,requests_file_path,
                            "Error while trying to open a file"));
            requests_loaded = false;
            return false;
        }
    }
    else if (!requests_loaded)
        return false;
    return true;
}

//Loads contents of files by path that is contained in source to list passed in result
void Loader::LoadFileContent(QStringList& result, const QStringList& source)
{
    QList<QFuture<QString>> open_file_threads;
    for (auto& file : source)
    {
        open_file_threads.append(QtConcurrent::run([file, result]()
        {
            QFile document_file(file);
            if (document_file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString content = document_file.readAll();
                document_file.close();
                content = content.toLower();
                return content;
            }
            else
            {
                return QString();
            }
        }));
    }
    for (auto& thread : open_file_threads)
    {
        thread.waitForFinished();
    }
    for (auto& thread : open_file_threads)
    {
        result.append(thread.result());
    }
}

//Checks file's path for correction
FileErrors Loader::checkFilePath(QString& file_path)
{
    QFileInfo file(file_path);
    if (!file.isFile())
        return FileErrors::NOT_A_FILE;
    if (!file.isReadable())
        return FileErrors::READ_ERR;
    if (!file.isWritable())
        return FileErrors::WRITE_ERR;
    return FileErrors::NO_ERR;
}

//Returns path to configurations' file
QString ConverterJSON::getConfigsPath() const
{
    return QFileInfo(config_file_path).filePath();
}

//Returns path to requests' file
QString ConverterJSON::getRequestsPath() const
{
    return QFileInfo(requests_file_path).filePath();
}

//Returns path to answers' file
QString ConverterJSON::getAnswersPath() const
{
    return QFileInfo(answers_file_path).filePath();
}
#endif
