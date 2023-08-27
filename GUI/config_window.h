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
    void maxResponsesChanged();
    void engineVerChanged();
    void engineNameChanged();
signals:
    void configPathChanged(QString new_path);
    void openConfigRequest();
private:    
    void fillFields();
    Ui::Config_window *ui;
    ConverterJSON* converter;
    QStringList file_list;
//  QList<QString> search_files;
    QStringListModel *search_files_list;
    QString engine_ver = "0.1";
    QString engine_name = "Search Engine";
    QString config_path = QDir::currentPath() + "/config.json";
    QString requests_path = QDir::currentPath() + "/requests.json";
    QString answers_path = QDir::currentPath() + "/answers.json";
};

#endif // CONFIG_WINDOW_H
