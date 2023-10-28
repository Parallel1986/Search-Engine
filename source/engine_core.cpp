#include "engine_core.h"

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
        delete converter;

    if (index)
        delete index;

    if (server)
        delete server;

    if (files_id)
        delete files_id;

    if (requests_id)
        delete requests_id;
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
    configs.max_responses = MIN_RESPONSE;
    files_paths.clear();
    if (!(engine_status & ConverterStatus::CONFIG_MISSED)
        && !(engine_status & ConverterStatus::CONFIG_FIELD_MISSED))
    {
        if (!(engine_status&ConverterStatus::MAX_RESPONSES_MISSED))
        {
            max_responses = converter->getResponsesLimit();
            configs.max_responses = max_responses;
        }
        else
        {
            FileError error (FileErrorType::NoDataError,
                             converter->getConfigsPath(),
                             "Maximal response is missing");
            processError(error);
            emit updateStatus(engine_status);
        }

        if (!(engine_status&ConverterStatus::SEARCH_FILES_MISSED))
            files_paths = converter->getFilesPaths();
        else
        {
            FileError error (FileErrorType::NoDataError,
                             converter->getConfigsPath(),
                             "Files for search are missing");
            processError(error);
            emit updateStatus(engine_status);
        }
    }
    else if (engine_status & ConverterStatus::CONFIG_FIELD_MISSED)
    {
        FileError error (FileErrorType::NoDataError,
                         converter->getConfigsPath(),
                         "Field \"config\" is missing");
        processError(error);
        emit updateStatus(engine_status);
    }
    else
        {
        FileError error (FileErrorType::FileNotExistError,
                         converter->getConfigsPath(),
                         "Configuration file is missing");
        processError(error);
        emit updateStatus(engine_status);
    }
    configs.enegine_name = converter->getEngineName();
    configs.engine_version = converter->getEngineVersion();

    configs.files = files_paths;

    configs.max_responses = max_responses;

    if (isUseUI())
    {
        emit updateStatus(engine_status);
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
    else if (engine_status & ConverterStatus::REQUESTS_EMPTY)
    {
        FileError error (FileErrorType::NoDataError,
                         converter->getRequestsPath(),
                         "Field \"requests\" is missing");
        processError(error);
        emit updateStatus(engine_status);
    }
    else
    {
        FileError error (FileErrorType::FileNotExistError,
                         converter->getRequestsPath(),
                         "Requests file is missing");

        requests.clear();
        processError(error);
        emit updateStatus(engine_status);
    }

    if (isUseUI())
    {
        emit requestsLoaded(requests);
        emit updateStatus(engine_status);
    }
}

char EngineCore::getEngineStatus() const
{
    return (converter->configCorrectionCheck()
           |converter->requestsCorrectionCheck());
}

void EngineCore::initialize()
{
    switch (mode)
    {
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
        emit updateStatus(getEngineStatus());
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
    QStringList reqs;   //requests
    QStringList files;  //list of files
    switch (mode)
    {
    case EngineMode::AUTOCONFIG:
    case EngineMode::STANDARD:

        files = files_paths;
        files += files_paths_add;
        reqs = requests;
        reqs += requests_add;
        break;
    case EngineMode::NO_CONFIG:
        files = files_paths_add;
        reqs = requests;
        reqs += requests_add;
        break;
    case EngineMode::NO_REQUESTS:
        files = files_paths;
        files += files_paths_add;
        reqs = requests_add;
        break;

    case EngineMode::MANUAL:
        files = files_paths_add;
        reqs = requests_add;
        break;

    //Should not get here
    default:
        break;
    }

    makeSearch(files,reqs);

    if (isUseUI())
        emit searchResult(search_result, files_id, requests_id);
    else
    {
        saveResult();
        std::cout << "Search done!" << std::endl;
    }
}

void EngineCore::makeSearch(QStringList& files, QStringList& requests)
{
    makeFilesIDTable(files);
    sendErrorFilesMessage();
    removeErrorFiles(files);
    stopSearchIfNoFiles(files);

    files_content.clear();
    files_content = Loader::LoadFileContent(files);
    index->updateDocumentBase(files_content);
    server->setMaxResponse(max_responses);

    makeRequestsIDdTable(requests);
    search_result = (server->search(requests));
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
        out_line += "Request: " + requests_id->frame[counter].request + "\n";
        if (it->size() == 0)
            out_line += "Result: No match\n";
        else
        {
            for(auto& pair : *it)
            {
                out_line += "\t" + QString("Rank = %1 - ").arg(pair.rank, 0, '0', 2)
                          + files_id->frame[pair.doc_id].file_path + "\n";
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

    files_id = new FileIDTable(files.size());

    int id = 0;
//    QList<QFuture<void>> threads;
    for (auto& file : files)
    {    
//        threads.append(QtConcurrent::run([this, id, file](){
        files_id->frame[id].id = id;
        files_id->frame[id].file_path = (file);
        files_id->frame[id].err = Loader::checkFilePath(file);
//        }));
        ++id;
    }
//    for (auto& thread : threads)
//        thread.waitForFinished();
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
        if (err.getExceptionType() == FileErrorType::FileNotExistError
            || err.getExceptionType() == FileErrorType::NotAFile
            || err.getExceptionType() == FileErrorType::OpenFileError
            || err.getExceptionType() == FileErrorType::OpenDirectoryError)
            std::cerr << "While trying to open: " <<
                    err.getExceptionSource().toStdString() << std::endl;

        else if (err.getExceptionType() == FileErrorType::WriteFileError
                 || err.getExceptionType() == FileErrorType::WriteDirectoryError)
            std::cerr << "While trying to write to: " <<
                    err.getExceptionSource().toStdString() << std::endl;

        else if (err.getExceptionType() == FileErrorType::NoDataError)
            std::cerr << "Does not has required data: " <<
                    err.getExceptionSource().toStdString() << std::endl;

        std::cerr << "Additional info: " <<
                    err.getAdditionalData().toStdString() << std::endl;

        throw std::runtime_error("Critical error!\n");
    }
    else
    {
        emit showError(err);
    }
}

void EngineCore::removeErrorFiles(QStringList& files)
{
    for (int frame_no = 0;frame_no < files_id->size;++frame_no)
    {
        if (!(files_id->frame[frame_no].err == FileErrorType::NotError))
        {
            files[frame_no].clear();
        }
    }
}

void EngineCore::sendErrorFilesMessage()
{
    if (useUI)
    {
        QList<FileError> error_list;
        for (int file_no = 0; file_no < files_id->size;++file_no)
        {
            if (files_id->frame[file_no].err != FileErrorType::NotError)
            {
                FileError error (files_id->frame[file_no].err,
                                  files_id->frame[file_no].file_path,
                                  "Error while trying to read file");
                error_list.append(error);
            }
        }

        if (error_list.empty())
            return;
        else if (error_list.size() == 1)
            emit showError(error_list.at(0));
        else
            emit showErrorList(error_list);
    }
    else
    {
        for (int file_no = 0; file_no < files_id->size;++file_no)
        {
            if (files_id->frame[file_no].err != FileErrorType::NotError)
            {
                QString message = Error::writeErrorType(files_id->frame[file_no].err,
                                                        files_id->frame[file_no].file_path);
                std::cerr << message.toStdString() << std::endl;
            }
        }
    }
}

void EngineCore::stopSearchIfNoFiles(QStringList& file_list)
{
    bool not_empty = false;
    for (auto& file: file_list)
    {
        if (!file.isEmpty())
        {
            not_empty = true;
            break;
        }
    }
    if (!not_empty)
    {
        processError(FileError(FileErrorType::NoDataError,
                               "Files for search.",
                               "Doesn't have at least one valid file. Search is stoped."));
    }
}
