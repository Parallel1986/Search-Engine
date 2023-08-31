#ifndef CONFIG_WINDOW_H
#define CONFIG_WINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QStringListModel>

//Включения поискового движка //Search engine`s includes
//#include "include/file_index.h"
#include "include/converter_json.h"
//#include "include/inverted_index.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Config_window; }
QT_END_NAMESPACE


class ConfigWindow : public QMainWindow
{
    Q_OBJECT

    friend class MainWindow;
public:
    ConfigWindow(QWidget *parent = nullptr, ConverterJSON *converter = nullptr);
    ~ConfigWindow();    

public slots:
    void clickedOk();
    void clickedCancel();
    void clickedRemove();
    void clickedAdd();
//    void maxResponsesChanged(int new_max);
//    void engineVerChanged(QString new_ver);
//    void engineNameChanged(QString new_name);
signals:
    void configPathChanged(QString new_path);
    void openConfigRequest();
    void ready();
    void noConfigChanges();
private:    
    void fillFields();
    void fillSettings();
    bool config_ready = false;
    Ui::Config_window *ui;
    ConverterJSON* converter;
    QStringListModel *search_files_list;
    ConfigList settings;
    QList<QString> bad_files {"Error files! Could not open!"};
    QString config_path = QDir::currentPath() + "/config.json";
    QString requests_path = QDir::currentPath() + "/requests.json";
    QString answers_path = QDir::currentPath() + "/answers.json";
};

#endif // CONFIG_WINDOW_H
