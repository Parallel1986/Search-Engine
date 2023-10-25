#ifndef COMAND_LINE_PROCESSOR_H
#define COMAND_LINE_PROCESSOR_H
#include <QString>
#include <iostream>
#include <exception>
#include "engine_core.h"


class CommandLineProcessor{
public:
    CommandLineProcessor(int argc, char** argv):argc(argc), argv(argv){}

    void processCommandLine();
    void applyPrefrenceTo(EngineCore* engine);
private:
    enum ComandLineOrders
    {
        NO_ORDERS               = 0,    //Do not do anything
        CHANGE_CONFIG_FILE      = 1,    //Change path to comfigurations file
        CHANGE_REQUESTS_FILE    = 2,    //Change path to requests file
        CHANGE_ANSWERS_FILE     = 4,    //Change path to answers file
        CHANGE_MAX_RESPONSE     = 8,    //Change response limit
        ADD_FILES_FOR_SEARCH    = 16,   //Add files for search to the end of file list
        ADD_REQUEST             = 32,   //Add requests to the end of request list
        GENERATE_CONFIG         = 64,   //Create new configurations file
        NO_CONFIG_MODE          = 128,  //Do not use configurations file
        NO_REQUESTS_MODE        = 256,  //Do not use requests file
        MANUAL_MODE             = 512,  //Do not use confirations and requests files
        USE_UI                  = 1024, //Use GUI
        ERROR                   = 2048  //Error in processing
    };

    //Structure for setting up engine's core
    struct Prefrence
    {
        QString config_path, request_path, answers_path;
        int max_response = 0;
        QStringList requests, files;
        short comands = ComandLineOrders::NO_ORDERS;
    };

    void checkFilePath(QString file);
    void checkResponseLimit();
    void checkFilesForSearch();
    void checkRequests();

    int argc;
    char** argv;
    Prefrence prefrence;
};

#endif // COMAND_LINE_PROCESSOR_H
