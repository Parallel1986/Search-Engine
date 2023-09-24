#include "../include/engine_core.h"

EngineCore::EngineCore()
{
//Creating engine's objects
    converter = &ConverterJSON::GetInstance();
    index = new InvertedIndex();
    server = new SearchServer(*index);

//Connecting signals

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
void EngineCore::SetConfigPath(QString new_path)
{
    converter->ChangeConfigPath(new_path);
}

//Set new requests' file
void EngineCore::SetRequestsPath(QString new_path)
{
    converter->ChangeRequestsPath(new_path);
}

//Sewt new answers' file
void EngineCore::SetAnswersPath(QString new_path)
{
    converter->ChangeAnswersPath(new_path);
}

//Check of initialization
bool EngineCore::IsInitialized()
{
    return IsConfigInitialized() && IsRequestsInitialized();
}

//Check if configurations are initialized
bool EngineCore::IsConfigInitialized()
{
    engine_status &= ConverterStatus::NO_CONFIG_ERRORS;
    return (!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
        &&!(engine_status&ConverterStatus::CONFIG_MISSED)
        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED));
}

//Check if requests are initialized
bool EngineCore::IsRequestsInitialized()
{
    engine_status &= ConverterStatus::NO_REQUESTS_ERRORS;
    return (!(engine_status&ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status&ConverterStatus::REQUESTS_EMPTY));
}

//Set maximal limit of responses
void EngineCore::SetMaxRequests(int new_max)
{
    if (max_responses != new_max)
        max_responses = new_max;
}

//Initialize configurations from the configurations' file
void EngineCore::InitializeConfig()
{
    engine_status = converter->ConfigCorrectionCheck();
    engine_name = converter->GetEngineName();
    engine_version = converter->GetEngineVersion();
    if (!(engine_status & ConverterStatus::CONFIG_MISSED)
        && !(engine_status & ConverterStatus::CONFIG_FIELD_MISSED))
        {
        if (!(engine_status&ConverterStatus::MAX_RESPONSES_MISSED))
            max_responses = converter->GetResponsesLimit();
        if (!(engine_status&ConverterStatus::SEARCH_FILES_MISSED))
        {
            file_list = converter->GetTextDocuments();
            files_paths = converter->GetFilesPaths();
        }
    }
}

//Initializing requests from the requests' file
void EngineCore::InitializeRequests()
{
    engine_status = converter->RequestsCorrectionCheck();
    if (!(engine_status & ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status & ConverterStatus::REQUESTS_EMPTY))
        requests = converter->GetRequests();
}

//Get engine status
char EngineCore::GetEngineStatus()
{
    return converter->ConfigCorrectionCheck()
          &converter->RequestsCorrectionCheck();
}

//Initialize search engine
void EngineCore::Initialize()
{
    InitializeConfig();
    InitializeRequests();
}

//Add requests
void EngineCore::AddRequest(QString new_request)
{
    requests.append(new_request);
}

//Add file for search
void EngineCore::AddSearchFile(QString new_file)
{
    file_list.append(new_file);
}

//Create configurations' file config.json and add fields to it
void EngineCore::GenerateConfigFile(QStringList files, int response_limit)
{
    ConfigList configurations;
    configurations.enegine_name = "Autogen Name";
    configurations.engine_ver = "Autogen V0.1.0";
    configurations.max_responses = response_limit;
    for (auto& file : files)
    {
        configurations.files.append(file);
    }
    converter->PutConfig(configurations, "./config.json");
}

//Set engine's mode
void EngineCore::SetMode(EngineMode new_mode)
{
    mode = new_mode;
}

//Make search
void EngineCore::Search()
{
    switch (mode) {
    case EngineMode::AUTOCONFIG:
    case EngineMode::STANDARD:
        Initialize();
        index->UpdateDocumentBase(converter->GetTextDocuments());
        server->setMaxResponse(converter->GetResponsesLimit());
        search_result = (server->search(converter->GetRequests()));
        converter->PutAnswers(search_result);
        break;

    case EngineMode::NO_CONFIG:
        InitializeRequests();
        Loader::LoadFileContent(file_list, file_list);
        index->UpdateDocumentBase(file_list);
        server->setMaxResponse(max_responses);
        search_result = (server->search(converter->GetRequests()));
        converter->PutAnswers(search_result);
        break;

    case EngineMode::NO_REQUESTS:
        InitializeConfig();
        index->UpdateDocumentBase(converter->GetTextDocuments());
        server->setMaxResponse(converter->GetResponsesLimit());
        search_result = (server->search(requests));
        converter->PutAnswers(search_result);
        break;

    case EngineMode::MANUAL:
        Loader::LoadFileContent(file_list, file_list);
        index->UpdateDocumentBase(file_list);
        server->setMaxResponse(max_responses);
        search_result = (server->search(requests));
        converter->PutAnswers(search_result);
        break;

    //Should not get here
    default:
        break;
    }

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
