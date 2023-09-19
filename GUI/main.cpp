#include "mainwindow.h"
#include "./include/engine_core.h"

#include<iostream>

#include <QApplication>
#include <QStringList>


enum Comands
{
    CONFIG_PATH     = 1,    //set path to config.json
    REQUESTS_PATH   = 2,    //set path to requests.json
    ANSWERS_PATH    = 4,    //set path to answers.json
    MAX_RESPONSE    = 8,    //set maximum response's count
    GUI_MODE        = 16,   //set mode with GUI
    MANUAL_MODE     = 32    //set mode without *json files - all settings are required in comand line
};

#define COMAND_LINE
//#define COMAND_LINE_TEST

#ifdef COMAND_LINE_TEST
#undef COMAND_LINE
#endif

#ifdef COMAND_LINE
//Proccessing command line
int processCommandLine(EngineCore& engine,int argc, char** argv)
{
    QStringList arguments;
    for (int arg_ptr = 1;arg_ptr <= argc; ++arg_ptr)
    {
        arguments.append(argv[arg_ptr]);
    }
    auto arg_end = arguments.end();
    for (auto arg = arguments.begin(); arg!= arguments.end();++arg)
    {
        if (*arg == "-cp")         //Set path to a configurations' file
        {
            ++arg;
            if (arg != arg_end
                && arg->at(0) != '-')
            {
                engine.SetConfigPath(*arg);
            }
            else
            {
                std::cerr << "Missing path to config file! Application will be terminated\n";
                return 1;
            }
        }
        else if (*arg == "-rp")	//Указать путь к requests.json
        {
            ++arg;
            if (arg != arg_end
            && arg->at(0) != '-')
            {
                engine.SetRequestsPath(*arg);
            }
            else
            {
                std::cerr << "Missing path to requests.json! Application will be terminated\n";
                return 2;
            }
        }
        else if (*arg == "-ap")	//Указать путь к answers.json
        {
            ++arg;
            if (arg != arg_end
            && arg->at(0) != '-')
            {
               engine.SetAnswersPath(*arg);
            }
            else
            {
                std::cerr << "Missing path to answers.json! Application will be terminated\n";
                return 3;
            }
        }
        else if (*arg == "-ra")	//Добавить поисковый запрос в начало списка запросов
        {
            ++arg;
            if (arg != arg_end && arg->at(0) != '-')
            {
                while (arg != arg_end && arg->at(0) != '-')
                {
                    engine.AddRequest(*arg);
                    ++arg;
                }
            }
            else
            {
                std::cerr << "Missing request! Application will be terminated\n";
                return 4;
            }
        }
        else if (*arg == "-mr")	//Задать максимальное количество ответов
        {
            ++arg;
            if (arg != arg_end
            && arg->at(0) != '-')
            {
                engine.SetMaxRequests(arg->toInt());
            }
            else
            {
                std::cerr << "Missing maximum requests count! Application will be terminated\n";
                return 5;
            }
        }
        else if (*arg == "-cg")	//Сгенерировать config.json в формате 'path_to_config.json' 'max_response_count' 'path_to_file_for_search\\filename'
        {
            ++arg;
            if (arg != arg_end && arg->at(0) != '-')
            {
                QStringList pref;
                int max_response;
                pref.append(*arg);
                pref.append("AutoGen");
                pref.append("AutoGen V.0");
                ++arg;
                if (arg != arg_end && arg->at(0) != '-')
                {
                    max_response = arg->toInt();
                    ++arg;
                }
                else
                {
                    std::cerr << "Missing maximum response count! Application will be terminated\n";
                    return 6;
                }
                if (arg != arg_end && arg->at(0) != '-')
                {
                    while (arg != arg_end && arg->at(0) != '-')
                    {
                        pref.append(*arg);
                        ++arg;
                    }
                }
                else
                {
                    std::cerr << "Missing path to file for search! Application will be terminated\n";
                    return 6;
                }
                engine.GenerateConfigFile(pref,max_response);
            }
            else
            {
                std::cerr << "Missing path to config.json! Application will be terminated\n";
                return 6;
            }
        }
        else if (*arg == "-fa")	//Добавить файлы для индексирования в наало списка
        {
            ++arg;
            if (arg != arg_end && arg->at(0) != '-')
            {
                while (arg != arg_end && arg->at(0) != '-')
                {
                    engine.AddSearchFile(*arg);
                    ++arg;
                }
            }
            else
            {
                std::cerr << "Missing request! Application will be terminated\n";
                return 7;
            }
        }
        else
        {
            std::cerr << "Unknown command! Aplication will be terminated\n";
        }
    }
    return 0;
}
#endif

int main(int argC, char *argV[])
{
    EngineCore engine;

    engine.Initialize();
    engine.Search();

#ifdef GUI
    QApplication a(argC, argV);
    MainWindow w;
#endif // GUI

#ifdef COMAND_LINE

    if (argC > 1)
    {
        int res = processCommandLine(engine, argC,argV);
        if (res != 0)
            return 1;
    }

#endif

#ifdef COMAND_LINE_TEST
    for (int i {}; i < argC; ++i)
    {
        std::cout << argV[i] << std::endl;
    }
#endif

#ifndef GUI

#endif

#ifdef GUI

    QApplication a(argC, argV);
    MainWindow w;

    w.show();
    w.initializeSearchEngine();
    return a.exec();
#endif
}
