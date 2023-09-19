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

//Включение виджетов
#include "include/requestadddialog.h"

class EngineCore;

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

signals:


public slots:
    void search();
    void openConfig();
    void openRequests();
    void openAnswers();
    void addRequests();
    void deleteRequests();
    void modifyConfig();
    void modifyRequests(QString);
    void setConfigPath();
    void setRequestsPath();
    void setAnswersPath();
    void checkConfigPath();
    void saveResult();
private:
    void configError();
    void requestsError();
    void loadRequests();
    void readynessCheck();
    bool config_ready = false;
    bool requests_ready = false;
    bool has_result = false;
    Ui::MainWindow *ui = nullptr;
    ConfigWindow *conf_ui = nullptr;
    RequestAddDialog* r_dialog = nullptr;
    QStringListModel *request_list_model = nullptr;
    EngineCore* core = nullptr;

//Removed to engine_core.h
//    QList<QList<RelativeIndex>> search_result;
//    QList<QString> requests_list;
//    ConverterJSON* converter = nullptr;
//    InvertedIndex* index = nullptr;
//    SearchServer* server = nullptr;
};
#endif // MAINWINDOW_H
