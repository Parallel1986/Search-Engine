#include "../include/engine_core.h"

EngineCore::EngineCore()
{
    converter = new ConverterJSON;
    index = new InvertedIndex();
    server = new SearchServer(index);

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

void EngineCore::setConfigPath(QString new_path)
{
    converter->changeConfigPath(new_path);
}

void EngineCore::setRequestsPath(QString new_path)
{
    converter->changeRequestsPath(new_path);
}

void EngineCore::setAnswersPath(QString new_path)
{
    converter->changeAnswersPath(new_path);
}

bool EngineCore::isInitialized() const
{
    return isConfigInitialized() && isRequestsInitialized();
}

bool EngineCore::isConfigInitialized() const
{
    return (!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
        &&!(engine_status&ConverterStatus::CONFIG_MISSED)
        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED));
}

bool EngineCore::isRequestsInitialized() const
{
    return (!(engine_status&ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status&ConverterStatus::REQUESTS_EMPTY));
}

void EngineCore::setMaxRequests(int new_max)
{
    if (max_responses != new_max)
        max_responses = new_max;
}

void EngineCore::initializeConfig()
{
    ConfigList configs;
    engine_status &= ConverterStatus::RESET_CONFIG_ERRORS;
    engine_status |= converter->configCorrectionCheck();
    configs.enegine_name = converter->getEngineName();
    configs.engine_version = converter->getEngineVersion();
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
    if (isUseUI())
    {
        emit configsLoaded(configs);
    }
}

void EngineCore::initializeRequests()
{
    engine_status &= ConverterStatus::RESET_REQUEST_ERRORS;
    engine_status |= converter->requestsCorrectionCheck();
    if (!(engine_status & ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status & ConverterStatus::REQUESTS_EMPTY))
        requests = converter->getRequests();
    else
        requests.clear();

    if (isUseUI())
    {
        emit requestsLoaded(requests);
    }
}

char EngineCore::getEngineStatus() const
{
    return (converter->configCorrectionCheck()
           |converter->requestsCorrectionCheck());
}

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
    if (isUseUI())
    {
        emit answersPathChanged(converter->getAnswersPath());
        emit requestsPathChanged(converter->getRequestsPath());
        emit configPathChanged(converter->getConfigsPath());
    }
}

void EngineCore::addRequest(QString new_request)
{
    requests_add.append(new_request);
    emit requestsLoaded(requests+requests_add);
}

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

void EngineCore::addSearchFile(QString new_file)
{
    files_paths_add.append(new_file);
    emit reloadFilePaths(files_paths+files_paths_add);
}

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

void EngineCore::generateConfigFile(QStringList files, int response_limit)
{
    ConfigList configurations;
    configurations.enegine_name = "Autogen Name";
    configurations.engine_version = "Autogen V0.1.0";
    configurations.max_responses = response_limit;
    for (auto& file : files)
    {
        configurations.files.append(file);
    }
    converter->putSettingsToConfig(configurations, "config.json");
    initializeConfig();
}

void EngineCore::setMode(EngineMode new_mode)
{
    mode = new_mode;
}

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

    if (isUseUI())
        emit searchResult(search_result, files_id, requests_id);
    else
        saveResult();
}

QStringList EngineCore::getFiles() const
{
    return files_paths+files_paths_add;
}

QStringList EngineCore::getRequestsList() const
{
    return requests+requests_add;
}

void EngineCore::configPathUpdated(QString new_path)
{
    initializeConfig();
    if (isUseUI())
        emit configPathChanged(new_path);
}

void EngineCore::requestsPathUpdated(QString new_path)
{
    initializeRequests();
    if (isUseUI())
        emit requestsPathChanged(new_path);
}

void EngineCore::answersPathUpdated(QString new_path)
{
    if (isUseUI())
        emit answersPathChanged(new_path);
}

void EngineCore::saveResult()
{
    if (!search_result.isEmpty())
        converter->putAnswersToJSON(search_result);
}

void EngineCore::saveResultAsText()
{
    QString path = QDir::current().absolutePath() + "/answers.txt";
    QString out_line;

    int counter = 0;
    for (auto it = search_result.begin();
             it != search_result.end();
             it++, counter++)
    {
        out_line += "Request: " + requests_id->frame->request[counter] + "\n";
        if (it->size() == 0)
            out_line += "Result: No match\n";
        else
        {
            for(auto& pair : *it)
            {
                out_line += "\t" + QString("Rank = %1 - ").arg(pair.rank, 0, '0', 2)
                          + files_id->frame->file_path[pair.doc_id] + "\n";
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
            delete files_id;

    files_id = new FileIDTable(files_paths.size()+files_paths_add.size());

    int id = 0;
    QList<QFuture<void>> threads;
    for (auto file = files.begin();
         file != files.end();
         ++file)
    {    
        threads.append(QtConcurrent::run([this, id, file](){
        files_id->frame[id].id = id;
        files_id->frame[id].file_path = (*file);
        files_id->frame[id].err = Loader::checkFilePath(*file);
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
        delete requests_id;

    requests_id = new RequestIDTable(requests.size()+requests_add.size());

    int id = 0;
    for (auto request = requests.begin();
         request != requests.end();
         ++request)
    {
        requests_id->frame[id].id = id;
        requests_id->frame[id].request = *request;
        ++id;
    }
    for (auto request = requests_add.begin();
         request != requests_add.end();
         ++request)
    {
        requests_id->frame[id].id = id;
        requests_id->frame[id].request = *request;
        ++id;
    }
}

const FileIDTable* EngineCore::getFilesIDTable() const
{
    return files_id;
}

const RequestIDTable* EngineCore::getRequestsIDTable() const
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

void EngineCore::processError(FileError err)
{
    if (!isUseUI())
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
