#include "mainwindow.h"
#include "engine_core.h"
#include "exceptions.h"
#include "comand_line_processor.h"

#include <iostream>

#include <QApplication>
#include <QStringList>


void connectGUIInterface(EngineCore* engine, MainWindow& window)
{
        QObject::connect(engine, &EngineCore::configPathChanged, &window, &MainWindow::setConfigPath);
        QObject::connect(engine, &EngineCore::requestsPathChanged, &window, &MainWindow::setRequestsPath);
        QObject::connect(engine, &EngineCore::answersPathChanged, &window, &MainWindow::setAnswersPath);
        QObject::connect(engine, &EngineCore::configsLoaded, &window, &MainWindow::fillConfigFields);
        QObject::connect(engine, &EngineCore::requestsLoaded, &window, &MainWindow::fillRequestsFields);
        QObject::connect(engine, &EngineCore::searchResult, &window, &MainWindow::showResult);
        QObject::connect(engine, &EngineCore::reloadFilePaths, &window, &MainWindow::fillSearchFilesFields);
        QObject::connect(engine, &EngineCore::showError, &window, &MainWindow::showError);

        QObject::connect(&window, &MainWindow::fileIsDeleted, engine, &EngineCore::removeSearchFile);
        QObject::connect(&window, &MainWindow::requestIsDeleted, engine, &EngineCore::removeRequest);
        QObject::connect(&window, &MainWindow::requestIsAdded, engine, &EngineCore::addRequest);
        QObject::connect(&window, &MainWindow::fileIsAdded, engine, &EngineCore::addSearchFile);
}

int main(int argC, char *argV[])
{
    EngineCore* engine = new EngineCore();

    if (argC > 1)
    {
        CommandLineProcessor line_processor(argC,argV);
        try {
            line_processor.processCommandLine();
            line_processor.applyPrefrenceTo(engine);
        }
        catch(FileError& error)
        {
            if (error.getExceptionType() == FileErrorType::NotAFile)
                std::cerr << "Wrong file!" <<std::endl;

            else if (error.getExceptionType() == FileErrorType::ReadFileError)
                std::cerr << "Can't read a file" << std::endl;

            else
                std::cerr << "Unknown error with file" << std::endl;

            std::cerr << error.getExceptionSource().toStdString() << std::endl
                      << error.getAdditionalData().toStdString();

            return -1;
        }
        catch(CommandLineError& error)
        {
            if (error.getType() == CommandLineErrorType::INVALID_ARGUMENT)
                std::cerr << "Invalid argument" << std::endl;

            else if (error.getType() == CommandLineErrorType::MISSING_ARGUMENT)
                std::cerr << "Missing argument" << std::endl;

            else
                std::cerr << "Unknown argument" << std::endl;

            std::cerr << error.getArgument().toStdString() << std::endl
                      << error.getAdditionalInfo().toStdString() << std::endl;

            return -1;
        }

        catch (...) {
            std::cerr << "Unknown error!\n";
            return -1;
        }
    }

    if (engine->isUseUI())
    {
        QApplication a(argC, argV);
        MainWindow w(nullptr,engine);

        connectGUIInterface(engine, w);

        try
        {
            engine->initialize();
        } catch (std::exception& error)
        {
            std::cerr << error.what();
            return -1;
        }


        w.show();
        return a.exec();
    }
    else
    {
        try
        {
            engine->initialize();
        } catch (std::exception& error)
        {
            std::cerr << error.what();
            return -1;
        }
        engine->search();
        return 0;
    }
}
