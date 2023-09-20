
#include "./include/engine_core.h"

#include<iostream>

#include <QApplication>
#include <QStringList>

#ifdef GUI
#include "mainwindow.h"
#endif

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
    ERROR                   = 1024  //Error in processing
};

struct Prefrence
{
    QString config_path, request_path, answers_path;    //Pathes tp files
    int max_response = 0;                               //Response limit
    QStringList requests, files;                        //Lists of files and requests
    short comands = ComandLineOrders::NO_ORDERS;        //Contains required orders
};

//Proccessing command line
Prefrence processCommandLine(const EngineCore& engine,int argc,char** argv)
{
    Prefrence pref;         //Prefrence for accumulating comand line orders
    QStringList arguments;    
    for (int arg_ptr = 1;arg_ptr <= argc; ++arg_ptr)
    {
        arguments.append(argv[arg_ptr]);
    }
    auto arg_end = arguments.end();
    for (auto arg = arguments.begin(); arg!= arguments.end();++arg)
    {
        if (*arg == "-c")         //Set configurations' file
        {
            ++arg;
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
            ++arg;
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
            ++arg;
            if (arg != arg_end
            && arg->at(0) != '-')
            {
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
            ++arg;
            if (arg != arg_end && arg->at(0) != '-')
            {
                while (arg != arg_end && arg->at(0) != '-')
                {
                    pref.requests.append(*arg);
                    ++arg;
                }
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
            ++arg;
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
            ++arg;            
        }
        else if (*arg == "-fa")	//Add files for search to the end of the list
        {
            ++arg;
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
        }
        else
        {
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
    EngineCore engine;

#ifdef GUI
    QApplication a(argC, argV);
    MainWindow w;
#endif // GUI

#ifdef COMAND_LINE
    if (argC > 1)
    {
        auto res = processCommandLine(engine, argC,argV);
        if (res.comands == ComandLineOrders::ERROR)
            return -1;
        else if (res.comands != ComandLineOrders::NO_ORDERS)
        {
            if (res.comands & ComandLineOrders::CHANGE_ANSWERS_FILE)
                engine.SetAnswersPath(res.answers_path);

            if (res.comands & ComandLineOrders::CHANGE_CONFIG_FILE)
                engine.SetConfigPath(res.config_path);

            if (res.comands & ComandLineOrders::CHANGE_REQUESTS_FILE)
                engine.SetRequestsPath(res.request_path);

            if (res.comands & ComandLineOrders::CHANGE_MAX_RESPONSE)
                engine.SetMaxRequests(res.max_response);

            if (res.comands & ComandLineOrders::MANUAL_MODE)
            {
                engine.SetMode(EngineMode::MANUAL);
                if (res.max_response == 0)
                {
                    std::cerr << "Incorrect maximum response limit!\n"
                                 "Application will be terminated!\n";
                    return -1;
                }
                else
                    engine.SetMaxRequests(res.max_response);

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
                        engine.AddRequest(request);
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
                        engine.AddSearchFile(file);
                    }
                }
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
                engine.GenerateConfigFile(res.files, res.max_response, res.requests);
            }

            if ((res.comands & ComandLineOrders::NO_CONFIG_MODE)
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
                    engine.AddSearchFile(file);
                }
                engine.SetMaxRequests(res.max_response);
                engine.SetMode(EngineMode::NO_CONFIG);
            }
            else
            {
                std::cerr << "Only one mode is allowed!\n"
                             "Application will be terminated!\n";
                return -1;
            }

            if ((res.comands & ComandLineOrders::NO_REQUESTS_MODE)
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
                    engine.AddRequest(request);
                }
                engine.SetMode(EngineMode::NO_REQUESTS);
            }
            else
            {
                std::cerr << "Only one mode is allowed!\n"
                             "Application will be terminated!\n";
                return -1;
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
                    engine.AddSearchFile(file);
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
                        engine.AddRequest(request);
                    }
                }
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

#ifndef GUI
    engine.Initialize();
    engine.Search();
#endif

#ifdef GUI

    QApplication a(argC, argV);
    MainWindow w;

    w.show();
    w.initializeSearchEngine();
    return a.exec();
#endif
}
