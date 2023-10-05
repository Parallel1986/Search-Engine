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
#include "include/exceptions.h"
//Widgets
#include "include/requestadddialog.h"

//Modes in main window
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

signals:
    void savedConfig(ConfigList);   //Is emited when configurations is saved
    void addedFile(QString);        //Is emited when file is added
    void addedRequest(QString);     //Is emited when request is added
    void deletedRequest(QString);   //Is emited when request in the list is deleted
    void deletedFile(QString);      //Is emited when file in the list is deleted
    void changedResponseLimit(int); //Is emited when response count is changed
    void changedMode(EngineMode);   //Is emited when changed mode in mode's selector

public slots:
    void search();                  //Search button is presed
    void openConfig();              //Open configurations' file button is pressed
    void openRequests();            //Open requests' file button is pressed
    void openAnswers();             //Open answers' file button is pressed
    void addRequests();             //Add request button is pressed
    void deleteRequests();          //Delete request button is pressed
    void modifyRequests(QString);   //New request is added
    void setConfigPath(QString);    //New path to configurations' file is setted
    void setRequestsPath(QString);  //New path to requests' file is setted
    void setAnswersPath(QString);   //New path to answers' file is setted
    void saveResult();              //Save result as JSON file
    void saveResultAsText();        //Save result as text file
    void changeMode(int);           //Change mode of engine
    void configSave();              //Save button in configurations' section
    void addFile();                 //Add button in requests' section is pressed
    void deleteFile();              //Delete button in files' section is pressed
    void showResult(QList<QList<RelativeIndex>>);   //Fill results of search
    void loadConfig(ConfigList);    //Fill filelds of configurations
    void loadRequests(QStringList); //Fill requests' field
    void loadSearchFiles(QStringList);  //Fill files for search
    void configError(char);         //Open configurations' error window
    void requestsError();           //Open requests' error window
    void generateConfig();          //Generates configurations' file with files' list
    void checkUI();                 //Checkes error marks
    void errorShow(EngineError);    //Shows an error message
private:
    void reloadFiles();             //is needed?//Reloads files' list
    void readinessCheck();          //Checkes readiness to search
    bool config_ready = false;      //Flag of configurations' readiness
    bool requests_ready = false;    //Flag of requests' readiness
    bool has_result = false;        //Flag of existing result
    Ui::MainWindow *ui = nullptr;   //User interface
    RequestAddDialog* r_dialog = nullptr;           //Request add dialog window
    QStringListModel *request_list_model = nullptr; //List model for requests' field
    QStringListModel *files_list_model = nullptr;   //List model for files' field
    EngineCore* core = nullptr;     //Pointer to core of search engine
};
#endif // MAINWINDOW_H
