#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMessageBox>

//Включения формы настроек config.json //Includes of settings of config.json
#include "config_window.h"

//Включения поискового движка //Search engine`s includes
#include "include/file_index.h"
#include "include/converter_json.h"
#include "include/inverted_index.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class ConfigWindow;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initializeSearchEngine();

public slots:
    void openConfig();
    void openRequests();
    void openAnswers();
    void addRequests();
    void deleteRequests();
    void modifyConfig();
    void modifyRequeests();
    void modifyAnswers();
    void setConfigPath();
    void setRequestsPath();
    void setAnswersPath();
    void checkConfigPath();
private:
    void configError();
    void requestsError();
//    void setConfigWarning();
//    void unsetConfigWarning();
//    void setRequestsWarning();
//    void unsetRequestsWarning();
    bool config_ready = false;
    bool requests_ready = false;
    Ui::MainWindow *ui;
    ConfigWindow *conf_ui;
    ConverterJSON* converter;
    InvertedIndex* index;
    SearchServer* server;
};
#endif // MAINWINDOW_H
