//***************************************************************************//
//  This file contains MainWindow class that designed to present graphical   //
// user interface and allows to change prefrenses and add files for search   //
// and add requests, even if configuration or request fileas are missing     //
//***************************************************************************//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMessageBox>
#include <QStringListModel>

#include "engine_core.h"
#include "exceptions.h"
#include "requestadddialog.h"

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
    void configIsSaved(ConfigList);

    void fileIsAdded(QString);
    void fileIsDeleted(QString);

    void requestIsAdded(QString);
    void requestIsDeleted(QString);

    void responseLimitIsChanged(int);

    void modeIsChanged(EngineMode);

public slots:
    void search();
    void showResult(QList<QList<RelativeIndex>>,const FileIDTable*, const RequestIDTable*);

    void openConfigPathDlg();
    void openRequestsPathDlg();
    void openAnswersPathDlg();

    void addRequest();
    void deleteRequest();
    void modifyRequests(QString);

    void setConfigPath(QString);
    void setRequestsPath(QString);
    void setAnswersPath(QString);

    void saveResult();
    void saveResultAsText();

    void changeMode(int);
    void saveConfig();

    void addFileDlg();
    void deleteFile();

    void fillConfigFields(ConfigList);
    void fillRequestsFields(QStringList);
    void fillSearchFilesFields(QStringList);

    void configErrorDlg(char);
    void requestsErrorDlg(char);

    void generateConfig();
    void checkUIMarks(char);
    void showError(FileError);
    void showErrorList(QList<FileError>);

private:
    void reloadFiles();
    void readinessCheck();

    bool config_ready = false;
    bool requests_ready = false;
    bool has_result = false;

    Ui::MainWindow *ui = nullptr;
    RequestAddDialog* r_dialog = nullptr;
    QStringListModel *request_list_model = nullptr;
    QStringListModel *files_list_model = nullptr;
    EngineCore* core = nullptr;
};
#endif // MAINWINDOW_H
