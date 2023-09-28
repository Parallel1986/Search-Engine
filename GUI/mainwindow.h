#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMessageBox>
#include <QStringListModel>

//Includes of settings of config.json
//#include "config_window.h"

//Search engine`s includes
#include "include/engine_core.h"

//Widgets
#include "include/requestadddialog.h"

enum EngineMode_MW
{
    STAND   = 0,
    MAN     = 1,
    NO_CONF = 2,
    NO_REQ  = 3,
};

class EngineCore;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class ConfigWindow;
public:
    MainWindow(QWidget *parent = nullptr, EngineCore* search_engine = nullptr);
    ~MainWindow();
//    void initializeSearchEngine();

signals:
/**/void savedConfig(ConfigList);
/**/void addedFile(QString);
/**/void addedRequest(QString);
/**/void deletedRequest(QString);
/**/void deletedFile(QString);
/**/void changedResponseLimit(int);
/**/void changedMode(EngineMode);

public slots:
    void search();
    void openConfig();
    void openRequests();
    void openAnswers();
    void addRequests();
    void deleteRequests();
//    void modifyConfig();
    void modifyRequests(QString);
    void setConfigPath(QString);
    void setRequestsPath(QString);
    void setAnswersPath(QString);
//    void checkConfigPath();
    void saveResult();
/**/void changeMode(EngineMode);
    void configSave();
    void addFile();
    void deleteFile();
    void showResult(QList<QList<RelativeIndex>>);
    void loadConfig(ConfigList);
    void loadRequests(QStringList);
    void configError(char);
private:    
    void checkUI();
    void reloadFiles();
    void requestsError();    
    void readinessCheck();
    bool config_ready = false;
    bool requests_ready = false;
    bool has_result = false;
    Ui::MainWindow *ui = nullptr;
//    ConfigWindow *conf_ui = nullptr;
    RequestAddDialog* r_dialog = nullptr;
    QStringListModel *request_list_model = nullptr;
    QStringListModel *files_list_model = nullptr;
    EngineCore* core = nullptr;

//Removed to engine_core.h
//    QList<QList<RelativeIndex>> search_result;
//    QList<QString> requests_list;
//    ConverterJSON* converter = nullptr;
//    InvertedIndex* index = nullptr;
//    SearchServer* server = nullptr;
};
#endif // MAINWINDOW_H
