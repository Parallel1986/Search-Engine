#include "../include/engine_core.h"

EngineCore::EngineCore()
{
//Creating engine's objects
    converter = new ConverterJSON;
    index = new InvertedIndex();
    server = new SearchServer(index);

//Connecting signals
    connect(converter,&ConverterJSON::configPathChanged,
            this,&EngineCore::configPathUpdated);

    connect(converter, &ConverterJSON::requestsPathChanged,
            this, &EngineCore::requestsPathUpdated);

    connect(converter, &ConverterJSON::answersPathChanged,
            this, &EngineCore::answersPathUpdated);

    connect(converter, &ConverterJSON::sendError,
            this, &EngineCore::processError);
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
    }
}

//Adds request
void EngineCore::addRequest(QString new_request)
{
    requests_add.append(new_request);
    emit requestsLoaded(requests+requests_add);
}

//Removes request
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

//Adds file for search
void EngineCore::addSearchFile(QString new_file)
{
    files_paths_add.append(new_file);
    emit reloadFilePaths(files_paths+files_paths_add);
}

//Removes file for search from files' list
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

//Create configurations' file config.json and adds fields to it
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

//Sets engine's mode
void EngineCore::setMode(EngineMode new_mode)
{
    mode = new_mode;
}

//Makes search
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
        makeFilesIDTable(files_paths_add);
        files_content.clear();
        Loader::LoadFileContent(files_content,adds);
        index->updateDocumentBase(files_content);
        server->setMaxResponse(max_responses);
        makeRequestsIDdTable(requests_add);
        search_result = (server->search(requests_add));
        break;

    case EngineMode::NO_REQUESTS:
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
        emit searchResult(search_result, files_id, requests_id);
    else
        saveResult();
}

//Returns a list of files for search
QStringList EngineCore::getFiles()
{
    return files_paths+files_paths_add;
}

//Returns a list of requests
QStringList EngineCore::getRequestsList()
{
    return requests+requests_add;
}

//Proccesses result of changing path to configurations' file
void EngineCore::configPathUpdated(QString new_path)
{
    initializeConfig();
    if (useUI)
        emit configPathChanged(new_path);
}

//Proccesses result of changing path to requests' file
void EngineCore::requestsPathUpdated(QString new_path)
{
    initializeRequests();
    if (useUI)
        emit requestsPathChanged(new_path);
}

//Proccesses result of changing path to answers' file
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
    QString path = QDir::current().absolutePath() + "/answers.txt";
    QString out_line;

    int counter = 0;
    for (auto it = search_result.begin();
             it != search_result.end();
             it++, counter++)
    {
        out_line += "Request: " + requests_id->requests[counter] + "\n";
        if (it->size() == 0)
        {
            out_line += "Result: No match\n";
        }
        else
        {
            for(auto& pair : *it)
            {
                out_line += "\t" + QString("Rank = %1 - ").arg(pair.rank, 0, '0', 2);
                out_line += files_id->file_path[pair.doc_id] + "\n";
            }
        }
    }

    QFile answer_file(path);
    if (answer_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        answer_file.write(out_line.toStdString().c_str());
        answer_file.close();
    }
}

//Creates table of accordance files' paths and its IDs
void EngineCore::makeFilesIDTable(QStringList& files)
{
    if (files_id != nullptr)
        {
            files_id->clear();
            files_id->reInitialise(files_paths.size()+files_paths_add.size());
        }
    else
        files_id = new FileIDTable(files_paths.size()+files_paths_add.size());

    int id = 0;
    QList<QFuture<void>> threads;
    for (auto file = files.begin();
         file != files.end();
         ++file)
    {
        threads.append(QtConcurrent::run([this, id, file](){
        files_id->id[id] = id;
        files_id->file_path[id] = *file;
        files_id->err[id] = Loader::checkFilePath(*file);
        }));
        ++id;
    }
    for (auto& thread : threads)
        thread.waitForFinished();
}

//Creates table of accordance requests and its IDs
void EngineCore::makeRequestsIDdTable(QStringList&)
{
    if (requests_id != nullptr)
    {
        requests_id ->clear();
        requests_id ->reInitialise(requests.size()+requests_add.size());
    }
    else
        requests_id = new RequestIDTable(requests.size()+requests_add.size());

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

//Returns true if GUI is used
bool EngineCore::isUseUI() const
{
    return useUI;
}

//Sets engine to use GUI
void EngineCore::setUI()
{
    useUI = true;
}

//Processes error
void EngineCore::processError(EngineError err)
{
    if (!useUI)
    {
        std::cerr << "Error!" << std::endl;
        if (err.getExceptionType() == ExceptionType::FileNotExistError
            || err.getExceptionType() == ExceptionType::OpenFileError
            || err.getExceptionType() == ExceptionType::OpenDirectoryError)
            std::cerr << "While trying to open: " <<
                    err.getExceptionSource().toStdString() << std::endl;
        else if (err.getExceptionType() == ExceptionType::WriteFileError
                 || err.getExceptionType() == ExceptionType::WriteDirectoryError)
            std::cerr << "While trying to write to: " <<
                    err.getExceptionSource().toStdString() << std::endl;
        else if (err.getExceptionType() == ExceptionType::NoDataError)
            std::cerr << "Does not has required data: " <<
                    err.getExceptionSource().toStdString() << std::endl;

        std::cerr << "Additional info: " <<
                    err.getAdditionalData().toStdString() << std::endl;
    }
    else
    {
        emit showError(err);
    }
}
