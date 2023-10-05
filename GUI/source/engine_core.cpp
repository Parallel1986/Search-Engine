#include "../include/engine_core.h"

EngineCore::EngineCore()
{
//Creating engine's objects
    converter = new ConverterJSON;
    index = new InvertedIndex();
    server = new SearchServer(index);

//Connecting signals
    connect(converter,&ConverterJSON::configPathChanged,this,&EngineCore::configPathUpdated);
    connect(converter, &ConverterJSON::requestsPathChanged, this, &EngineCore::requestsPathUpdated);
    connect(converter, &ConverterJSON::answersPathChanged, this, &EngineCore::answersPathUpdated);

}

EngineCore::~EngineCore()
{
    if (converter)
    {
        delete converter;
        converter = nullptr;
    }
    if (index)
    {
        delete index;
        index = nullptr;
    }
    if (server)
    {
        delete server;
        server = nullptr;
    }
}

//Set new configurations' file
void EngineCore::setConfigPath(QString new_path)
{
    converter->changeConfigPath(new_path);
}

//Set new requests' file
void EngineCore::setRequestsPath(QString new_path)
{
    converter->changeRequestsPath(new_path);
}

//Sewt new answers' file
void EngineCore::setAnswersPath(QString new_path)
{
    converter->changeAnswersPath(new_path);
}

//Check of initialization
bool EngineCore::isInitialized()
{
    return isConfigInitialized() && isRequestsInitialized();
}

//Check if configurations are initialized
bool EngineCore::isConfigInitialized()
{
    engine_status &= ConverterStatus::NO_CONFIG_ERRORS;
    return (!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
        &&!(engine_status&ConverterStatus::CONFIG_MISSED)
        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED));
}

//Check if requests are initialized
bool EngineCore::isRequestsInitialized()
{
    engine_status &= ConverterStatus::NO_REQUESTS_ERRORS;
    return (!(engine_status&ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status&ConverterStatus::REQUESTS_EMPTY));
}

//Set maximal limit of responses
void EngineCore::setMaxRequests(int new_max)
{
    if (max_responses != new_max)
        max_responses = new_max;
}

//Initialize configurations from the configurations' file
void EngineCore::initializeConfig()
{
    ConfigList configs;
    engine_status = converter->configCorrectionCheck();
    engine_name = converter->getEngineName();
    configs.enegine_name = engine_name;
    engine_version = converter->getEngineVersion();
    configs.engine_ver = engine_version;
    files_paths.clear();
    max_responses = MIN_RESPONSE;
    if (!(engine_status & ConverterStatus::CONFIG_MISSED)
        && !(engine_status & ConverterStatus::CONFIG_FIELD_MISSED))
        {
        if (!(engine_status&ConverterStatus::MAX_RESPONSES_MISSED))
            {
                max_responses = converter->getResponsesLimit();
                configs.max_responses = max_responses;
            }
        if (!(engine_status&ConverterStatus::SEARCH_FILES_MISSED))
        {
            files_paths = converter->getFilesPaths();
        }
    }
    configs.files = files_paths;
    configs.max_responses = max_responses;
    if (useUI)
    {
        emit configsLoaded(configs);
//        emit updateStatus(engine_status);
    }

}

//Initializing requests from the requests' file
void EngineCore::initializeRequests()
{
    engine_status = converter->requestsCorrectionCheck();
    if (!(engine_status & ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status & ConverterStatus::REQUESTS_EMPTY))
        requests = converter->getRequests();
    else
        requests.clear();

    if (useUI)
    {
        emit requestsLoaded(requests);
//        emit updateStatus(engine_status);
    }
}

//Get engine status
char EngineCore::getEngineStatus()
{
    return (converter->configCorrectionCheck()
          |converter->requestsCorrectionCheck());
}

//Initialize search engine
void EngineCore::initialize()
{
    switch (mode) {
    case EngineMode::STANDARD:
    case EngineMode::AUTOCONFIG:
        initializeConfig();
        initializeRequests();
        break;
    case EngineMode::NO_CONFIG:
        initializeRequests();
        break;
    case EngineMode::NO_REQUESTS:
        initializeConfig();
        break;
    case::EngineMode::MANUAL:    
    default:
        break;
    }
    if (useUI)
    {
        emit answersPathChanged(converter->getAnswersPath());
        emit requestsPathChanged(converter->getRequestsPath());
        emit configPathChanged(converter->getConfigsPath());
//        emit updateStatus(engine_status);
    }
}

//Add requests
void EngineCore::addRequest(QString new_request)
{
    requests_add.append(new_request);
    emit requestsLoaded(requests+requests_add);
}

//
void EngineCore::removeRequest(QString rm_request)
{
    for (auto request = requests.begin();
         request != requests.end();
         ++request)
    {
        if (*request == rm_request)
        {
            requests.erase(request);
            break;
        }
    }
    for (auto request = requests_add.begin();
         request != requests_add.end();
         ++request)
    {
        if (*request == rm_request)
        {
            requests_add.erase(request);
            break;
        }
    }
    return emit requestsLoaded(requests + requests_add);
}

//Add file for search
void EngineCore::addSearchFile(QString new_file)
{
    files_paths_add.append(new_file);
    emit reloadFilePaths(files_paths+files_paths_add);
}

//Remove file for search from files' list
void EngineCore::removeSearchFile(QString rm_file)
{
    for (auto file = files_paths.begin();
         file != files_paths.end();
         ++file)
    {
        if (*file == rm_file)
        {
            files_paths.erase(file);            
            return emit reloadFilePaths(files_paths+files_paths_add);
        }
    }
    for (auto file = files_paths_add.begin();
         file != files_paths_add.end();
         ++file)
    {
        if (*file == rm_file)
        {
            files_paths_add.erase(file);            
            return emit reloadFilePaths(files_paths+files_paths_add);
        }
    }
}

//Create configurations' file config.json and add fields to it
void EngineCore::generateConfigFile(QStringList files, int response_limit)
{
    ConfigList configurations;
    configurations.enegine_name = "Autogen Name";
    configurations.engine_ver = "Autogen V0.1.0";
    configurations.max_responses = response_limit;
    for (auto& file : files)
    {
        configurations.files.append(file);
    }
    converter->putConfig(configurations, "config.json");
    initializeConfig();
}

//Set engine's mode
void EngineCore::setMode(EngineMode new_mode)
{
    mode = new_mode;
}

//Make search
void EngineCore::search()
{    
    QStringList adds;
    switch (mode)
    {
    case EngineMode::AUTOCONFIG:
    case EngineMode::STANDARD:
        adds.clear();
        adds = files_paths;
        adds += files_paths_add;
        files_content.clear();
        Loader::LoadFileContent(files_content,adds);
        makeFilesIDTable(adds);
        index->updateDocumentBase(files_content);
        server->setMaxResponse(max_responses);
        adds.clear();
        adds = requests;
        adds += requests_add;
        makeRequestsIDdTable(adds);
        search_result = (server->search(adds));
        break;

    case EngineMode::NO_CONFIG:
//        files_content.clear();
//        Loader::LoadFileContent(files_content, files_paths_add);
        makeFilesIDTable(files_paths_add);
        files_content.clear();
        Loader::LoadFileContent(files_content,adds);
        index->updateDocumentBase(files_content);
        server->setMaxResponse(max_responses);
        makeRequestsIDdTable(requests_add);
        search_result = (server->search(requests_add));
        break;

    case EngineMode::NO_REQUESTS:
//        files_content.clear();
//        Loader::LoadFileContent(files_content,files_paths);
//        Loader::LoadFileContent(files_content,files_paths_add);
        adds.clear();
        adds = files_paths;
        adds += files_paths_add;
        files_content.clear();
        Loader::LoadFileContent(files_content,adds);
        makeFilesIDTable(adds);
        index->updateDocumentBase(files_content);
        server->setMaxResponse(max_responses);
        makeRequestsIDdTable(requests_add);
        search_result = (server->search(requests_add));
        break;

    case EngineMode::MANUAL:
//        files_content.clear();
//        Loader::LoadFileContent(files_content, files_paths_add);
        files_content.clear();
        Loader::LoadFileContent(files_content,adds);
        index->updateDocumentBase(files_content);
        makeFilesIDTable(files_paths_add);
        server->setMaxResponse(max_responses);
        makeRequestsIDdTable(requests_add);
        search_result = (server->search(requests_add));
        break;

    //Should not get here
    default:
        break;
    }

    if (useUI)
        emit searchResult(search_result);
    else
        saveResult();
}

QStringList EngineCore::getFiles()
{
    return files_paths+files_paths_add;
}

QStringList EngineCore::getRequestsList()
{
    return requests+requests_add;
}

void EngineCore::checkConfigPath(char status)
{

}

void EngineCore::checkRequestsPath(char status)
{

}

//Proccess result of changing path to configurations' file
void EngineCore::configPathUpdated(QString new_path)
{
    initializeConfig();
    if (useUI)
        emit configPathChanged(new_path);
}

//Proccess result of changing path to requests' file
void EngineCore::requestsPathUpdated(QString new_path)
{
    initializeRequests();
    if (useUI)
        emit requestsPathChanged(new_path);
}

//Proccess result of changing path to answers' file
void EngineCore::answersPathUpdated(QString new_path)
{
    if (useUI)
        emit answersPathChanged(new_path);
}

//Saves results of the search
void EngineCore::saveResult()
{
    if (!search_result.isEmpty())
    {
        converter->putAnswers(search_result);
    }
}

//Saves result of the search as text file
void EngineCore::saveResultAsText()
{
    QJsonDocument answers_template;
    QString path = QDir::current().absolutePath() + "/answers.txt";
    QFile answer_file(path);

    if (answer_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QJsonArray answer_array;
        int counter = 0;

        for (auto it = search_result.begin();
             it != search_result.end();
             it++, counter++)
        {
            if (it->size() == 0)
            {
                QJsonObject answer, result;
                result.insert("result", false);
                answer.insert(requests_id->requests[counter], result);
                answer_array.append(answer);
            }
            else
            {
                QJsonObject answer, res;
                QJsonArray res_array;
                for(auto& pair : *it)
                {
                    res.insert("document",files_id->file_path[pair.doc_id]);
                    res.insert("rank",pair.rank);
                    res_array.append(res);
                }
                answer.insert(requests_id->requests[counter],res_array);
                answer_array.append(answer);
            }
        }
        QJsonObject output;
        output.insert("answers",answer_array);
        answers_template.setObject(output);
        QByteArray out_stream;
        out_stream = answers_template.toJson(QJsonDocument::Indented);
        QString unformat_doc = out_stream;
        unformat_doc.remove('{');
        unformat_doc.remove('}');
        unformat_doc.remove('[');
        unformat_doc.remove(']');
        unformat_doc.remove(',');
        for (auto it = 0;
             it < unformat_doc.size() - 1;
             ++it)
        {
            if (unformat_doc[it] == '\n' && unformat_doc[it+1] == '\n')
                unformat_doc[it] = ' ';
        }
        out_stream.clear();
        out_stream = unformat_doc.toStdString().c_str();
        answer_file.write(out_stream);
        answer_file.close();
    }
}

//Creates table of accordance files' paths and its IDs
void EngineCore::makeFilesIDTable(QStringList& files)
{
    if (files_id != nullptr)
    {
        files_id->clear();
        files_id = nullptr;
    }
    files_id = new FileIDTable(files_paths.size()+files_paths_add.size());

    int id = 0;
    QList<QFuture<void>> threads;
    for (auto file = files.begin();
         file != files.end();
         ++file)
    {
        threads.append(QtConcurrent::run([this, id, file](){
//        FileIDFrame frame;
//        frame.id = id;
//        frame.file_path = *file;
//        frame.err = Loader::checkFilePath(*file);
//        return frame;
        files_id->id[id] = id;
        files_id->file_path[id] = *file;
        files_id->err[id] = Loader::checkFilePath(*file);
        }));
        ++id;
    }
    for (auto& thread : threads)
        thread.waitForFinished();

//    for (int it = 0; it < files_paths.size()+files_paths_add.size(); ++it)
//    {
//        files_id->id = threads.at(it).results()
//    }
}

//Creates table of accordance requests and its IDs
void EngineCore::makeRequestsIDdTable(QStringList&)
{
    if (requests_id != nullptr)
    {
        requests_id->clear();
        requests_id = nullptr;
    }
    int total_size = requests.size()+requests_add.size();
    requests_id = new RequestIDTable(total_size);

    int id = 0;
    for (auto request = requests.begin();
         request != requests.end();
         ++request)
    {
        requests_id->id[id] = id;
        requests_id->requests[id] = *request;
        ++id;
    }
    for (auto request = requests_add.begin();
         request != requests_add.end();
         ++request)
    {
        requests_id->id[id] = id;
        requests_id->requests[id] = *request;
        ++id;
    }
}

//Returns table of accordance files' paths and its IDs
FileIDTable* EngineCore::getFilesIDTable()
{
    return files_id;
}

//Returns table of accordance requests and its IDs
RequestIDTable* EngineCore::getRequestsIDTable()
{
    return requests_id;
}

bool EngineCore::isUseUI() const
{
    return useUI;
}


void EngineCore::setUI()
{
    useUI = true;
}

//Excluded
////Load fields config from the configurations' file
//void EngineCore::LoadConfigs()
//{
//    if (!(engine_status&ConverterStatus::CONFIG_MISSED)             //Проверка статуса
//        &&!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED))
//    {
//        QFile config(config_file_path);
//        if (config.open(QIODevice::ReadOnly | QIODevice::Text))     //Проверка открытия
//        {
//            //Читаем файл конфигурации
//            QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
//            config.close();                 //Закрываем файл
//            LoadConfigs(configuration);     //Загружаем настройки
//        }
//    }
//}

////Load fields config from the JSON document
//void EngineCore::LoadConfigs(QJsonDocument& configuration)
//{
//    auto json_config_field = configuration.object()["config"];      //Получаем объект из JSON документа
//    if (json_config_field.toObject().contains("name"))
//    {
//        engine_name = json_config_field.toObject()["name"].toString();  //Записываем имя поискового двигателя
//        engine_status &= ~ConverterStatus::ENGINE_NAME_MISSED;      //Выставляем статус
//    }
//    else
//        engine_status |= ConverterStatus::ENGINE_NAME_MISSED;       //Выставляем статус

//    if (json_config_field.toObject().contains("version"))           //Проверяем наличие поля "version"
//    {
//        engine_version = json_config_field.toObject()["version"].toString();    //Записываем версию поискового двигателя
//        engine_status &= ~ConverterStatus::ENGINE_VERSION_MISSED;   //Выставляем статус
//    }
//    else
//        engine_status |= ConverterStatus::ENGINE_VERSION_MISSED;    //Выставляем статус

//    if (json_config_field.toObject().contains("max_responses"))
//    {
//        max_responses = json_config_field.toObject()["max_responses"].toInt();  //Записываем максимавльное число запросов //Filling number of maximum responses
//        engine_status &= ~ConverterStatus::MAX_RESPONSES_MISSED;    //Выставляем статус
//    }
//    else
//        engine_status |= ConverterStatus::MAX_RESPONSES_MISSED;     //Выставляем статус
//}

////Load field files from the
///configurations' file
//void EngineCore::LoadSearchFiles()
//{
//    if (!(engine_status&ConverterStatus::CONFIG_MISSED)             //Проверяем статус
//        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED))
//    {
//        QFile config(config_file_path);
//        if (config.open(QIODevice::ReadOnly | QIODevice::Text))     //Проверка на открытие
//        {
//            //Загружаем JSON документ из файла
//            QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
//            config.close();                     //Закрываем файл
//            LoadSearchFiles(configuration);     //Загружаем список фвайлов
//        }
//    }
//}

////Загрузить поля files из JSON документа
//void EngineCore::LoadSearchFiles(QJsonDocument& configuration)
//{
//    auto json_fiels_field = configuration.object()["files"].toArray();
//    file_paths.clear();                 //Очищаем список путей
//    if (!json_fiels_field.isEmpty())    //Проверка на наличие записей
//    {
//        //Заполняем список
//        for (auto it = json_fiels_field.begin();it != json_fiels_field.end();it++)
//            file_paths.append(it->toString());
//    }
//    else
//        engine_status |= ConverterStatus::SEARCH_FILES_MISSED;  //Выставляем статус
//}
