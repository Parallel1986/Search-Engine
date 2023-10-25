#include "comand_line_processor.h"
#include "exceptions.h"

//Proccessing command line
void CommandLineProcessor::processCommandLine()
{
    QStringList arguments;

    for (int arg_ptr = 1;arg_ptr < argc; ++arg_ptr)
        arguments.append(argv[arg_ptr]);

    auto arg_last = arguments.end();

    for (auto arg = arguments.begin(); arg!= arg_last;++arg)
    {
        if (*arg == "-c")         //Set configurations' file
        {
            ++arg;
            if (arg != arg_last
                && arg->at(0) != '-')
            {
                checkFilePath(*arg);
                prefrence.config_path = *arg;
                prefrence.comands |= ComandLineOrders::CHANGE_CONFIG_FILE;
            }
            else
                throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                                       "-c",
                                       "Missing path to configuration file");
        }
        else if (*arg == "-r")	//Set requests; file
        {
            ++arg;
            if (arg != arg_last
            && arg->at(0) != '-')
            {
                checkFilePath(*arg);
                prefrence.request_path = *arg;
                prefrence.comands |= ComandLineOrders::CHANGE_REQUESTS_FILE;
            }
            else
                throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                                       "-r",
                                       "Missing path to requests file");
        }
        else if (*arg == "-a")	//Set answers' file
        {
            ++arg;
            if (arg != arg_last
                && arg->at(0) != '-')
            {
                checkFilePath(*arg);
                prefrence.answers_path =*arg;
                prefrence.comands |= ComandLineOrders::CHANGE_ANSWERS_FILE;
            }
            else
                throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                                       "-a",
                                       "Missing path to answers file");
        }
        else if (*arg == "-ra")	//add requests to end of list
        {
            if ((arg+1) != arg_last && (arg+1)->at(0) != '-')
            {
                do
                {
                    ++arg;
                    prefrence.requests.append(*arg);
                }while ((arg+1) != arg_last && (arg+1)->at(0) != '-');
                prefrence.comands |= ComandLineOrders::ADD_REQUEST;
            }
            else
                throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                                       "-ra",
                                       "Missing requests. At least one is required");
        }
        else if (*arg == "-mr")	//Set response limit
        {
            ++arg;
            if (arg != arg_last
                && arg->at(0) != '-'
                && arg->toInt() != 0)
            {
                prefrence.max_response = arg->toInt();
                prefrence.comands |= ComandLineOrders::CHANGE_MAX_RESPONSE;
            }
            else
                throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                                       "-mr",
                                       "Incorrect response limit");
        }
        else if (*arg == "-cg")	//Generate config.json
        {
            prefrence.comands |= ComandLineOrders::GENERATE_CONFIG;
            ++arg;
        }
        else if (*arg == "-fa")	//Add files for search to the end of the list
        {
            ++arg;
            if (arg != arg_last && arg->at(0) != '-')
            {
                while (arg != arg_last && arg->at(0) != '-')
                {
                    checkFilePath(*arg);
                    prefrence.files.append(*arg);
                    ++arg;
                }
                prefrence.comands |= ComandLineOrders::ADD_FILES_FOR_SEARCH;
            }
            else
                throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                                       "-fa",
                                       "Missing path to files");
        }
        else if (*arg == "-mm")
            prefrence.comands |= ComandLineOrders::MANUAL_MODE;

        else if (*arg == "-nc")
            prefrence.comands |= ComandLineOrders::NO_CONFIG_MODE;

        else if (*arg == "-nr")
            prefrence.comands |= ComandLineOrders::NO_REQUESTS_MODE;

        else if (*arg == "-ui")
            prefrence.comands |= ComandLineOrders::USE_UI;

        else if (*arg == " ")
            ++arg;

        else
            CommandLineError(CommandLineErrorType::UNKNOWN_ARGUMENT,
                                       *arg,
                                       "Unknown argument");
    }
}

void CommandLineProcessor::checkFilePath(QString file)
{
    QFileInfo f_info(file);
    if (!f_info.isFile()||!f_info.exists())
        throw FileError(FileErrorType::NotAFile,file,"In command line");
    if (!f_info.isReadable())
        throw FileError(FileErrorType::ReadFileError, file, "In command line");
}

void CommandLineProcessor::checkResponseLimit()
{
    if (this->prefrence.max_response <= 0)
        throw CommandLineError(CommandLineErrorType::INVALID_ARGUMENT,
                               "-mr",
                               "Doesn't set maximal response limit");
}

void CommandLineProcessor::checkFilesForSearch()
{
    if(prefrence.files.isEmpty())
    {
    throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                           "-fa",
                           "Missing files! Add at least one.");
    }
}

void CommandLineProcessor::checkRequests()
{
    if (prefrence.requests.isEmpty())
        throw CommandLineError(CommandLineErrorType::MISSING_ARGUMENT,
                               "-ra",
                               "Missing requests! Add at least one.");
}


void CommandLineProcessor::applyPrefrenceTo(EngineCore* engine)
{
    if (prefrence.comands != ComandLineOrders::NO_ORDERS)
    {
        if (prefrence.comands & ComandLineOrders::CHANGE_ANSWERS_FILE)
            engine->setAnswersPath(prefrence.answers_path);

        if (prefrence.comands & ComandLineOrders::CHANGE_CONFIG_FILE)
            engine->setConfigPath(prefrence.config_path);

        if (prefrence.comands & ComandLineOrders::CHANGE_REQUESTS_FILE)
            engine->setRequestsPath(prefrence.request_path);

        if (prefrence.comands & ComandLineOrders::CHANGE_MAX_RESPONSE)
            engine->setMaxRequests(prefrence.max_response);

        if (prefrence.comands & ComandLineOrders::MANUAL_MODE
            && !(prefrence.comands & ComandLineOrders::NO_CONFIG_MODE)
            && !(prefrence.comands & ComandLineOrders::NO_REQUESTS_MODE))
        {
            checkResponseLimit();
            checkRequests();
            checkFilesForSearch();

            engine->setMode(EngineMode::MANUAL);
            engine->setMaxRequests(prefrence.max_response);

            for (auto& request : prefrence.requests)
                engine->addRequest(request);

            for (auto& file : prefrence.files)
                engine->addSearchFile(file);
        }
        else if ((prefrence.comands & ComandLineOrders::NO_CONFIG_MODE)
                 &&!(prefrence.comands & ComandLineOrders::MANUAL_MODE)
                 &&!(prefrence.comands & ComandLineOrders::NO_REQUESTS_MODE))
        {
            checkFilesForSearch();
            checkResponseLimit();

            for (auto& file : prefrence.files)
                engine->addSearchFile(file);

            engine->setMaxRequests(prefrence.max_response);
            engine->setMode(EngineMode::NO_CONFIG);
        }
        else if ((prefrence.comands & ComandLineOrders::NO_REQUESTS_MODE)
                 &&!(prefrence.comands & ComandLineOrders::MANUAL_MODE)
                 &&!(prefrence.comands & ComandLineOrders::NO_CONFIG_MODE))
        {
            checkRequests();

            for (auto& request : prefrence.requests)
                engine->addRequest(request);

            engine->setMode(EngineMode::NO_REQUESTS);
        }
        else if (prefrence.comands & ComandLineOrders::MANUAL_MODE
                 || prefrence.comands & ComandLineOrders::NO_CONFIG_MODE
                 || prefrence.comands & ComandLineOrders::NO_REQUESTS_MODE)
        {
            throw CommandLineError(CommandLineErrorType::INVALID_ARGUMENT,
                                   "-mm, -nc, -nr",
                                   "Only one mode can be applied!");
        }

        if (prefrence.comands & ComandLineOrders::GENERATE_CONFIG)
        {
            checkFilesForSearch();
            checkRequests();
            checkResponseLimit();

            engine->generateConfigFile(prefrence.files, prefrence.max_response);
        }

        if ((prefrence.comands & ComandLineOrders::ADD_FILES_FOR_SEARCH)
            && !(prefrence.comands & ComandLineOrders::GENERATE_CONFIG)
            && !(prefrence.comands & ComandLineOrders::MANUAL_MODE)
            && !(prefrence.comands & ComandLineOrders::NO_CONFIG_MODE))
        {
            checkFilesForSearch();

            for (auto& file : prefrence.files)
                engine->addSearchFile(file);
        }

        if ((prefrence.comands & ComandLineOrders::ADD_REQUEST)
            && !(prefrence.comands & ComandLineOrders::MANUAL_MODE)
            && !(prefrence.comands & ComandLineOrders::NO_REQUESTS_MODE))
        {
            checkRequests();
            for (auto& request : prefrence.requests)
                engine->addRequest(request);
        }

        if (prefrence.comands & ComandLineOrders::USE_UI)
            engine->setUI();
    }
}
