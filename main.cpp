#include "mainwindow.h"
#include "./include/engine_core.h"
#include "include/exceptions.h"
#include<iostream>

#include <QApplication>
#include <QStringList>

#define COMAND_LINE
//#define COMAND_LINE_TEST

#ifdef COMAND_LINE_TEST
#undef COMAND_LINE
#endif

#ifdef COMAND_LINE
enum ComandLineOrders
{
    NO_ORDERS               = 0,    //Do not do anything
    CHANGE_CONFIG_FILE      = 1,    //Change comfigurations' file
    CHANGE_REQUESTS_FILE    = 2,    //Change requests' file
    CHANGE_ANSWERS_FILE     = 4,    //Change answers' file
    CHANGE_MAX_RESPONSE     = 8,    //Change response limit
    ADD_FILES_FOR_SEARCH    = 16,   //Add files for search to the end of file list
    ADD_REQUEST             = 32,   //Add requests to the end of request list
    GENERATE_CONFIG         = 64,   //Create new configurations' file
    NO_CONFIG_MODE          = 128,  //Do not use configurations' file
    NO_REQUESTS_MODE        = 256,  //Do not use requests' file
    MANUAL_MODE             = 512,  //Do not use confirations' and requests' files
    USE_UI                  = 1024, //Use GUI
    ERROR                   = 2048  //Error in processing
};

//Structure for setting upo engine's core
struct Prefrence
{
    QString config_path, request_path, answers_path;    //Pathes to files
    int max_response = 0;                               //Response limit
    QStringList requests, files;                        //Lists of files and requests
    short comands = ComandLineOrders::NO_ORDERS;        //Contains required orders
};

//Proccessing command line
Prefrence processCommandLine(int argc,char** argv)
{
    Prefrence pref;         //Prefrence for accumulating comand line orders
    QList<QString> arguments;
    for (int arg_ptr = 1;arg_ptr < argc; ++arg_ptr)
    {
//        std::cout << argv[arg_ptr] << std::endl;
        arguments.append(argv[arg_ptr]);
//        std::cout << arguments.back().toStdString() << std::endl;
    }

    for (auto it = arguments.begin(); it != arguments.end(); ++it)
    {
        std::cout << it->toStdString() << std::endl;
    }

    auto arg_end = arguments.end();
    for (auto arg = arguments.begin(); arg!= arg_end;++arg)
    {
        std::cout << "Argument in: " << arg->toStdString() << std::endl;
        if (*arg == "-c")         //Set configurations' file
        {
            std::cout << "Entered to -c processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            if (arg != arg_end
                && arg->at(0) != '-')
            {
                pref.config_path = *arg;
                pref.comands |= ComandLineOrders::CHANGE_CONFIG_FILE;
            }
            else
            {
                std::cerr << "Missing path to config file!\n"
                             "Application will be terminated\n";
                pref.comands = ComandLineOrders::ERROR;
                return pref;
            }
        }
        else if (*arg == "-r")	//Set requests; file
        {
            std::cout << "Entered to -r processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            if (arg != arg_end
            && arg->at(0) != '-')
            {
                pref.request_path = *arg;
                pref.comands |= ComandLineOrders::CHANGE_REQUESTS_FILE;
            }
            else
            {
                std::cerr << "Missing path to requests.json!\n"
                             "Application will be terminated\n";
                pref.comands = ComandLineOrders::ERROR;
                return pref;
            }
        }
        else if (*arg == "-a")	//Set answers' file
        {
            std::cout << "Entered to -a processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            if (arg != arg_end
            && arg->at(0) != '-')
            {
                std::cout << "Added new answer path: "<< arg->toStdString() << std::endl;
                pref.answers_path =*arg;
                pref.comands |= ComandLineOrders::CHANGE_ANSWERS_FILE;
            }
            else
            {
                std::cerr << "Missing path to answers.json!\n"
                             "Application will be terminated\n";
                pref.comands = ComandLineOrders::ERROR;
                return pref;
            }
        }
        else if (*arg == "-ra")	//add requests to end of list
        {
            std::cout << "Entered to -ra processor with command "<< arg->toStdString() << std::endl;
//            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            if ((arg+1) != arg_end && (arg+1)->at(0) != '-')
            {
                do
                {
                    ++arg;
                    pref.requests.append(*arg);
                    std::cout << "Added: "<< arg->toStdString() << std::endl;
                    std::cout << "Next argument: "<< (arg+1)->toStdString() << std::endl;
                }while ((arg+1) != arg_end && (arg+1)->at(0) != '-');
                pref.comands |= ComandLineOrders::ADD_REQUEST;
            }
            else
            {
                std::cerr << "Missing request!\n"
                             "Application will be terminated\n";
                pref.comands = ComandLineOrders::ERROR;
                return pref;
            }
        }
        else if (*arg == "-mr")	//Set response limit
        {
            std::cout << "Entered to -mr processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            if (arg != arg_end
            && arg->at(0) != '-'
            && arg->toInt() != 0)
            {
                pref.max_response = arg->toInt();
                pref.comands |= ComandLineOrders::CHANGE_MAX_RESPONSE;
            }
            else
            {
                std::cerr << "Incorrect maximum requests count!\n"
                             "Application will be terminated\n";
                pref.comands = ComandLineOrders::ERROR;
                return pref;
            }
        }
        else if (*arg == "-cg")	//Generate config.json
        {
            pref.comands |= ComandLineOrders::GENERATE_CONFIG;
            std::cout << "Entered to -cg processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
        }
        else if (*arg == "-fa")	//Add files for search to the end of the list
        {
            std::cout << "Entered to -fa processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            if (arg != arg_end && arg->at(0) != '-')
            {
                while (arg != arg_end && arg->at(0) != '-')
                {
                    pref.files.append(*arg);
                    ++arg;
                }
                pref.comands |= ComandLineOrders::ADD_FILES_FOR_SEARCH;
            }
            else
            {
                std::cerr << "Missing request!\n"
                             "Application will be terminated\n";
                pref.comands = ComandLineOrders::ERROR;
                return pref;
            }
        }
        else if (*arg == "-mm")
        {
            pref.comands |= ComandLineOrders::MANUAL_MODE;
            std::cout << "Entered to -mm processor with command "<< arg->toStdString() << std::endl;
//            ++arg;
//            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
        }
		else if (*arg == "-nc")
		{
			pref.comands |= ComandLineOrders::NO_CONFIG_MODE;
		}
		else if (*arg == "-nr")
		{
			pref.comands |= ComandLineOrders::NO_REQUESTS_MODE;
		}
        else if (*arg == "-ui")
        {
            std::cout << "Entered to -ui processor with command "<< arg->toStdString() << std::endl;
//            ++arg;
//            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            pref.comands |= ComandLineOrders::USE_UI;
        }
        else if (*arg == " ")
        {
            std::cout << "Entered to ' ' processor with command "<< arg->toStdString() << std::endl;
            ++arg;
            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
        }
        else
        {
            std::cout << "Entered to unknown processor with command "<< arg->toStdString() << std::endl;
//            std::cout << "Next argument: "<< arg->toStdString() << std::endl;
            std::cerr << "Unknown command!\n"
                         "Aplication will be terminated\n";
            pref.comands = ComandLineOrders::ERROR;
            return pref;
        }
    }
    return pref;
}
#endif

int main(int argC, char *argV[])
{
    EngineCore* engine = new EngineCore();

#ifdef COMAND_LINE
    if (argC > 1)
    {
        auto res = processCommandLine(argC,argV);
        if (res.comands == ComandLineOrders::ERROR)
            return -1;
        else if (res.comands != ComandLineOrders::NO_ORDERS)
        {
            if (res.comands & ComandLineOrders::CHANGE_ANSWERS_FILE)
                engine->setAnswersPath(res.answers_path);

            if (res.comands & ComandLineOrders::CHANGE_CONFIG_FILE)
                engine->setConfigPath(res.config_path);

            if (res.comands & ComandLineOrders::CHANGE_REQUESTS_FILE)
                engine->setRequestsPath(res.request_path);

            if (res.comands & ComandLineOrders::CHANGE_MAX_RESPONSE)
                engine->setMaxRequests(res.max_response);

            if (res.comands & ComandLineOrders::MANUAL_MODE
                && !(res.comands & ComandLineOrders::NO_CONFIG_MODE)
                && !(res.comands & ComandLineOrders::NO_REQUESTS_MODE))
            {
                engine->setMode(EngineMode::MANUAL);
                if (res.max_response == 0)
                {
                    std::cerr << "Incorrect maximum response limit!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }
                else
                    engine->setMaxRequests(res.max_response);

                if (res.requests.isEmpty())
                {
                    std::cerr << "Requests are required for manual mode!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }
                else
                {
                    for (auto& request : res.requests)
                    {
                        engine->addRequest(request);
                    }
                }

                if (res.files.isEmpty())
                {
                    std::cerr << "Files for search are required!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }
                else
                {
                    for (auto& file : res.files)
                    {
                        engine->addSearchFile(file);
                    }
                }
            } else if ((res.comands & ComandLineOrders::NO_CONFIG_MODE)
                &&!(res.comands & ComandLineOrders::MANUAL_MODE)
                &&!(res.comands & ComandLineOrders::NO_REQUESTS_MODE))
            {
                if(res.files.isEmpty())
                {
                    std::cerr << "Files for search are required!\n"
                                 "Aplication will be terminated!\n";
                    return -1;
                }

                if (res.max_response == 0)
                {
                    std::cerr << "Incorrect response limit!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }

                for (auto& file : res.files)
                {
                    engine->addSearchFile(file);
                }
                engine->setMaxRequests(res.max_response);
                engine->setMode(EngineMode::NO_CONFIG);
            }
            else if ((res.comands & ComandLineOrders::NO_REQUESTS_MODE)
                &&!(res.comands & ComandLineOrders::MANUAL_MODE)
                &&!(res.comands & ComandLineOrders::NO_CONFIG_MODE))
            {
                if (res.requests.isEmpty())
                {
                    std::cerr << "Requests are required!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }

                for (auto& request : res.requests)
                {
                    engine->addRequest(request);
                }
                engine->setMode(EngineMode::NO_REQUESTS);
            }
            else if (res.comands & ComandLineOrders::MANUAL_MODE
                     || res.comands & ComandLineOrders::NO_CONFIG_MODE
                     || res.comands & ComandLineOrders::NO_REQUESTS_MODE)
            {
                std::cerr << "Only one mode is allowed!\n"
                             "Application will be terminated!\n";
                return -1;
            }


            if (res.comands & ComandLineOrders::GENERATE_CONFIG)
            {
                if(res.files.isEmpty())
                {
                    std::cerr << "Files for search are required!\n"
                                 "Aplication will be terminated!\n";
                    return -1;
                }

                if (res.requests.isEmpty())
                {
                    std::cerr << "Requests are required!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }

                if (res.max_response == 0)
                {
                    std::cerr << "Incorrect response limit!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }
                engine->generateConfigFile(res.files, res.max_response);
            }

            if ((res.comands & ComandLineOrders::ADD_FILES_FOR_SEARCH)
                && !(res.comands & ComandLineOrders::GENERATE_CONFIG)
                && !(res.comands & ComandLineOrders::MANUAL_MODE)
                && !(res.comands & ComandLineOrders::NO_CONFIG_MODE))
            {
                if(res.files.isEmpty())
                {
                    std::cerr << "Files for search are required!\n"
                                 "Aplication will be terminated!\n";
                    return -1;
                }

                for (auto& file : res.files)
                {
                    engine->addSearchFile(file);
                }
            }

            if ((res.comands & ComandLineOrders::ADD_REQUEST)
                && !(res.comands & ComandLineOrders::MANUAL_MODE)
                && !(res.comands & ComandLineOrders::NO_REQUESTS_MODE))
            {
                if (res.requests.isEmpty())
                {
                    std::cerr << "Requests are required for manual mode!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }
                else
                {
                    for (auto& request : res.requests)
                    {
                        engine->addRequest(request);
                    }
                }
            }
            if (res.comands & ComandLineOrders::USE_UI)
            {
                engine->setUI();
            }
        }
    }

#endif

#ifdef COMAND_LINE_TEST
    for (int i {}; i < argC; ++i)
    {
        std::cout << argV[i] << std::endl;
    }
#endif
    if (engine->isUseUI())
    {
        QApplication a(argC, argV);
        MainWindow w(nullptr,engine);
        QObject::connect(engine, &EngineCore::configPathChanged, &w, &MainWindow::setConfigPath);
        QObject::connect(engine, &EngineCore::requestsPathChanged, &w, &MainWindow::setRequestsPath);
        QObject::connect(engine, &EngineCore::answersPathChanged, &w, &MainWindow::setAnswersPath);
        QObject::connect(engine, &EngineCore::configsLoaded, &w, &MainWindow::loadConfig);
        QObject::connect(engine, &EngineCore::requestsLoaded, &w, &MainWindow::loadRequests);
        QObject::connect(engine, &EngineCore::searchResult, &w, &MainWindow::showResult);
        QObject::connect(engine, &EngineCore::reloadFilePaths, &w, &MainWindow::loadSearchFiles);
        QObject::connect(engine, &EngineCore::showError, &w, &MainWindow::errorShow);
        QObject::connect(&w, &MainWindow::deletedFile, engine, &EngineCore::removeSearchFile);
        QObject::connect(&w, &MainWindow::deletedRequest, engine, &EngineCore::removeRequest);
        QObject::connect(&w, &MainWindow::addedRequest, engine, &EngineCore::addRequest);
        QObject::connect(&w, &MainWindow::addedFile, engine, &EngineCore::addSearchFile);


        engine->initialize();
        w.show();
        return a.exec();
    }
    else
    {
        engine->initialize();
        engine->search();
        return 0;
    }
}
