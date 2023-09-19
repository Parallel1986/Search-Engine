#include "mainwindow.h"
#include "./include/engine_core.h"

#include<iostream>

#include <QApplication>


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
#define COMAND_LINE_TEST

#ifdef COMAND_LINE_TEST
#undef COMAND_LINE
#endif

#ifdef COMAND_LINE

//Обработка аргументов командной строки
int processCommandLine(Preference& pref,int argc, char** argv)
{
    int com_ptr = 1;
    unsigned char set = 0;
    std::string arg_v;
    while (com_ptr < argc)
    {
        arg_v = argv[com_ptr];
        if (arg_v == "-cp")         //Указать путь к config.json
        {
            ++com_ptr;
            if (com_ptr < argc
                && *argv[com_ptr] != '-'
                && !(set&Comands::CONFIG_PATH))
            {
                arg_v = argv[com_ptr];
                pref.config_path = arg_v;
                pref.changes |= CHANGES::PATH_TO_CONFIG;
                ++com_ptr;
            }
            else
            {
                std::cerr << "Missing path to config file! Application will be terminated\n";
                return 1;
            }
        }
        else if (arg_v == "-rp")	//Указать путь к requests.json
        {
            ++com_ptr;
            if (com_ptr < argc
            && *argv[com_ptr] != '-'
            && !(set&Comands::REQUESTS_PATH))
            {
                arg_v = argv[com_ptr];
                pref.requests_path = arg_v;
                pref.changes |= CHANGES::PATH_TO_REQUESTS;
                ++com_ptr;
            }
            else
            {
                std::cerr << "Missing path to requests.json! Application will be terminated\n";
                return 2;
            }
        }
        else if (arg_v == "-ap")	//Указать путь к answers.json
        {
            ++com_ptr;
            if (com_ptr < argc
            && *argv[com_ptr] != '-'
            && !(set&Comands::ANSWERS_PATH))
            {
                arg_v = argv[com_ptr];
                pref.answers_path = arg_v;
                pref.changes |= CHANGES::PATH_TO_ANSWERS;
                ++com_ptr;
            }
            else
            {
                std::cerr << "Missing path to answers.json! Application will be terminated\n";
                return 3;
            }
        }
        else if (arg_v == "-ra")	//Добавить поисковый запрос в начало списка запросов
        {
            ++com_ptr;
            if (com_ptr < argc && *argv[com_ptr] != '-')
            {
                while (com_ptr < argc && *argv[com_ptr] != '-')
                {
                    arg_v = argv[com_ptr];
                    pref.requests_adds.push_back(arg_v);
                    pref.changes |= CHANGES::REQUESTS_ADDED;
                    ++com_ptr;
                }
            }
            else
            {
                std::cerr << "Missing request! Application will be terminated\n";
                return 4;
            }
        }
        else if (arg_v == "-mr")	//Задать макввсимальное количество ответов
        {
            ++com_ptr;
            if (com_ptr < argc
            && *argv[com_ptr] != '-'
            && !(set&Comands::MAX_RESPONSE))
            {
                arg_v = argv[com_ptr];
                pref.max_response = stoi(arg_v);
                pref.changes |= CHANGES::RESPONSES;
                ++com_ptr;
            }
            else
            {
                std::cerr << "Missing maximum requests count! Application will be terminated\n";
                return 5;
            }
        }
        else if (arg_v == "-cg")	//Сгенерировать config.json в формате 'path_to_config.json' 'max_response_count' 'path_to_file_for_search\\filename'
        {
            ++com_ptr;
            if (com_ptr < argc && *argv[com_ptr] != '-')
            {
                arg_v = argv[com_ptr];
                pref.config_path = arg_v;
                pref.changes |= CHANGES::PATH_TO_CONFIG;
                ++com_ptr;
                if (com_ptr < argc && *argv[com_ptr] != '-')
                {
                    arg_v = argv[com_ptr];
                    pref.max_response = stoi(arg_v);
                    pref.changes |= CHANGES::RESPONSES;
                    ++com_ptr;
                }
                else
                {
                    std::cerr << "Missing maximum response count! Application will be terminated\n";
                    return 6;
                }
                if (com_ptr < argc && *argv[com_ptr] != '-')
                {
                    while (com_ptr < argc && *argv[com_ptr] != '-')
                    {
                        arg_v = argv[com_ptr];
                        pref.file_list_adds.push_back(arg_v);
                        pref.changes |= CHANGES::FILES_TO_SEARCH_ADDED;
                        ++com_ptr;
                    }
                }
                else
                {
                    std::cerr << "Missing path to file for search! Application will be terminated\n";
                    return 6;
                }
                pref.changes |= CHANGES::NEW_CONFIG;
            }
            else
            {
                std::cerr << "Missing path to config.json! Application will be terminated\n";
                return 6;
            }
        }
        else if (arg_v == "-fa")	//Добавить файлы для индексирования в наало списка
        {
            ++com_ptr;
            if (com_ptr < argc && *argv[com_ptr] != '-')
            {
                while (com_ptr < argc && *argv[com_ptr] != '-')
                {
                    arg_v = argv[com_ptr];
                    pref.file_list_adds.push_back(arg_v);
                    pref.changes |= CHANGES::FILES_TO_SEARCH_ADDED;
                    ++com_ptr;
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
#ifdef GUI
    QApplication a(argC, argV);
    MainWindow w;
#endif // GUI

#ifdef COMAND_LINE
    Preference pref;

    if (argC > 1)
    {
        int res = processCommandLine(pref, argC,argV);
        if (res != 0)
            return 1;
        converter.SetPreferences(pref);
    }

#endif

#ifdef COMAND_LINE_TEST
    for (int i {}; i < argC; ++i)
    {
        std::cout << argV[i] << std::endl;
    }
#endif
    QApplication a(argC, argV);
    MainWindow w;

    w.show();
    w.initializeSearchEngine();
    return a.exec();

#ifdef GUI
    w.show();
    return a.exec();
#endif // GUI
}
