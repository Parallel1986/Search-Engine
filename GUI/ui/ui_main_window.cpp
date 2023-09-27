#ifndef MAINWINDOWGMPYSP_H
#define MAINWINDOWGMPYSP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen_Config_json;
    QAction *actionOpen_Requests_json;
    QAction *actionSave_Answers_json;
    QAction *actionExit;
    QAction *actionabout;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *requests_fieeld_line;
    QListView *requests_line_list;
    QLabel *config_path_line;
    QFrame *line;
    QLabel *answers_field_line;
    QPushButton *search_button;
    QLabel *answers_path_line;
    QVBoxLayout *verticalLayout;
    QPushButton *requests_add_button;
    QPushButton *requests_delete_button;
    QSpacerItem *verticalSpacer;
    QTreeWidget *results_tree_widget;
    QLabel *requests_path_line;
    QComboBox *mode_select;
    QPushButton *config_path_button;
    QFrame *line_2;
    QPushButton *save_button;
    QLabel *label;
    QLineEdit *config_path_edit;
    QLineEdit *requests_path_edit;
    QLineEdit *answers_path_edit;
    QLabel *no_requests_mark;
    QLabel *no_config_mark;
    QPushButton *requests_path_button;
    QPushButton *answers_path_button;
    QPushButton *config_change;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(920, 551);
        actionOpen_Config_json = new QAction(MainWindow);
        actionOpen_Config_json->setObjectName(QString::fromUtf8("actionOpen_Config_json"));
        QIcon icon;
        QString iconThemeName = QString::fromUtf8("document-open");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionOpen_Config_json->setIcon(icon);
        actionOpen_Requests_json = new QAction(MainWindow);
        actionOpen_Requests_json->setObjectName(QString::fromUtf8("actionOpen_Requests_json"));
        actionOpen_Requests_json->setIcon(icon);
        actionSave_Answers_json = new QAction(MainWindow);
        actionSave_Answers_json->setObjectName(QString::fromUtf8("actionSave_Answers_json"));
        QIcon icon1;
        iconThemeName = QString::fromUtf8("document-save");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionSave_Answers_json->setIcon(icon1);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon2;
        iconThemeName = QString::fromUtf8("window-close");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon2 = QIcon::fromTheme(iconThemeName);
        } else {
            icon2.addFile(QString::fromUtf8("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionExit->setIcon(icon2);
        actionabout = new QAction(MainWindow);
        actionabout->setObjectName(QString::fromUtf8("actionabout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        requests_fieeld_line = new QLabel(centralwidget);
        requests_fieeld_line->setObjectName(QString::fromUtf8("requests_fieeld_line"));

        gridLayout->addWidget(requests_fieeld_line, 7, 0, 1, 1);

        requests_line_list = new QListView(centralwidget);
        requests_line_list->setObjectName(QString::fromUtf8("requests_line_list"));
        requests_line_list->setEditTriggers(QAbstractItemView::SelectedClicked);

        gridLayout->addWidget(requests_line_list, 8, 0, 1, 4);

        config_path_line = new QLabel(centralwidget);
        config_path_line->setObjectName(QString::fromUtf8("config_path_line"));

        gridLayout->addWidget(config_path_line, 0, 0, 1, 1);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 9, 0, 1, 5);

        answers_field_line = new QLabel(centralwidget);
        answers_field_line->setObjectName(QString::fromUtf8("answers_field_line"));

        gridLayout->addWidget(answers_field_line, 10, 0, 1, 1);

        search_button = new QPushButton(centralwidget);
        search_button->setObjectName(QString::fromUtf8("search_button"));
        search_button->setEnabled(false);

        gridLayout->addWidget(search_button, 10, 4, 1, 1);

        answers_path_line = new QLabel(centralwidget);
        answers_path_line->setObjectName(QString::fromUtf8("answers_path_line"));

        gridLayout->addWidget(answers_path_line, 4, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        requests_add_button = new QPushButton(centralwidget);
        requests_add_button->setObjectName(QString::fromUtf8("requests_add_button"));

        verticalLayout->addWidget(requests_add_button);

        requests_delete_button = new QPushButton(centralwidget);
        requests_delete_button->setObjectName(QString::fromUtf8("requests_delete_button"));

        verticalLayout->addWidget(requests_delete_button);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout->addLayout(verticalLayout, 8, 4, 1, 1);

        results_tree_widget = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(2, QString::fromUtf8("3"));
        __qtreewidgetitem->setText(1, QString::fromUtf8("2"));
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        results_tree_widget->setHeaderItem(__qtreewidgetitem);
        results_tree_widget->setObjectName(QString::fromUtf8("results_tree_widget"));
        results_tree_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        results_tree_widget->setProperty("showDropIndicator", QVariant(false));
        results_tree_widget->setAlternatingRowColors(false);
        results_tree_widget->setSelectionMode(QAbstractItemView::NoSelection);
        results_tree_widget->setRootIsDecorated(true);
        results_tree_widget->setColumnCount(3);
        results_tree_widget->header()->setVisible(false);

        gridLayout->addWidget(results_tree_widget, 11, 0, 1, 5);

        requests_path_line = new QLabel(centralwidget);
        requests_path_line->setObjectName(QString::fromUtf8("requests_path_line"));

        gridLayout->addWidget(requests_path_line, 2, 0, 1, 1);

        mode_select = new QComboBox(centralwidget);
        mode_select->addItem(QString());
        mode_select->addItem(QString());
        mode_select->addItem(QString());
        mode_select->addItem(QString());
        mode_select->setObjectName(QString::fromUtf8("mode_select"));
        mode_select->setMaxVisibleItems(5);
        mode_select->setMaxCount(4);
        mode_select->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(mode_select, 7, 3, 1, 2);

        config_path_button = new QPushButton(centralwidget);
        config_path_button->setObjectName(QString::fromUtf8("config_path_button"));

        gridLayout->addWidget(config_path_button, 1, 4, 1, 1);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_2, 6, 0, 1, 5);

        save_button = new QPushButton(centralwidget);
        save_button->setObjectName(QString::fromUtf8("save_button"));

        gridLayout->addWidget(save_button, 12, 4, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 7, 1, 1, 2);

        config_path_edit = new QLineEdit(centralwidget);
        config_path_edit->setObjectName(QString::fromUtf8("config_path_edit"));

        gridLayout->addWidget(config_path_edit, 1, 0, 1, 1);

        requests_path_edit = new QLineEdit(centralwidget);
        requests_path_edit->setObjectName(QString::fromUtf8("requests_path_edit"));

        gridLayout->addWidget(requests_path_edit, 3, 0, 1, 1);

        answers_path_edit = new QLineEdit(centralwidget);
        answers_path_edit->setObjectName(QString::fromUtf8("answers_path_edit"));

        gridLayout->addWidget(answers_path_edit, 5, 0, 1, 1);

        no_requests_mark = new QLabel(centralwidget);
        no_requests_mark->setObjectName(QString::fromUtf8("no_requests_mark"));
        no_requests_mark->setEnabled(true);
        no_requests_mark->setPixmap(QPixmap(QString::fromUtf8("icons/warning_small.png")));

        gridLayout->addWidget(no_requests_mark, 3, 1, 1, 1);

        no_config_mark = new QLabel(centralwidget);
        no_config_mark->setObjectName(QString::fromUtf8("no_config_mark"));
        no_config_mark->setEnabled(true);
        no_config_mark->setPixmap(QPixmap(QString::fromUtf8("icons/warning_small.png")));

        gridLayout->addWidget(no_config_mark, 1, 1, 1, 1);

        requests_path_button = new QPushButton(centralwidget);
        requests_path_button->setObjectName(QString::fromUtf8("requests_path_button"));

        gridLayout->addWidget(requests_path_button, 3, 2, 1, 3);

        answers_path_button = new QPushButton(centralwidget);
        answers_path_button->setObjectName(QString::fromUtf8("answers_path_button"));

        gridLayout->addWidget(answers_path_button, 5, 2, 1, 3);

        config_change = new QPushButton(centralwidget);
        config_change->setObjectName(QString::fromUtf8("config_change"));

        gridLayout->addWidget(config_change, 1, 2, 1, 2);


        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);
        QObject::connect(config_path_button, SIGNAL(clicked()), MainWindow, SLOT(openConfig()));
        QObject::connect(requests_path_button, SIGNAL(clicked()), MainWindow, SLOT(openRequests()));
        QObject::connect(answers_path_button, SIGNAL(clicked()), MainWindow, SLOT(openAnswers()));
        QObject::connect(requests_add_button, SIGNAL(clicked()), MainWindow, SLOT(addRequests()));
        QObject::connect(requests_delete_button, SIGNAL(clicked()), MainWindow, SLOT(deleteRequests()));
        QObject::connect(config_path_edit, SIGNAL(textChanged(QString)), MainWindow, SLOT(modifyConfig()));
        QObject::connect(requests_path_edit, SIGNAL(textChanged(QString)), MainWindow, SLOT(modifyRequests()));
        QObject::connect(answers_path_edit, SIGNAL(textChanged(QString)), MainWindow, SLOT(modifyAnswers()));
        QObject::connect(config_change, SIGNAL(clicked()), MainWindow, SLOT(modifyConfig()));
        QObject::connect(search_button, SIGNAL(clicked()), MainWindow, SLOT(search()));
        QObject::connect(save_button, SIGNAL(clicked()), MainWindow, SLOT(saveResult()));
        QObject::connect(mode_select, SIGNAL(currentIndexChanged(int)), MainWindow, SLOT(changeMode()));

        mode_select->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Search engine", nullptr));
        actionOpen_Config_json->setText(QCoreApplication::translate("MainWindow", "Open Config.json...", nullptr));
#if QT_CONFIG(tooltip)
        actionOpen_Config_json->setToolTip(QCoreApplication::translate("MainWindow", "Open Config.json file", nullptr));
#endif // QT_CONFIG(tooltip)
        actionOpen_Requests_json->setText(QCoreApplication::translate("MainWindow", "Open Requests.json...", nullptr));
#if QT_CONFIG(tooltip)
        actionOpen_Requests_json->setToolTip(QCoreApplication::translate("MainWindow", "Open Requests.json file", nullptr));
#endif // QT_CONFIG(tooltip)
        actionSave_Answers_json->setText(QCoreApplication::translate("MainWindow", "Save Answers.json...", nullptr));
#if QT_CONFIG(tooltip)
        actionSave_Answers_json->setToolTip(QCoreApplication::translate("MainWindow", "Choose path to Answers.json file", nullptr));
#endif // QT_CONFIG(tooltip)
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionabout->setText(QCoreApplication::translate("MainWindow", "About...", nullptr));
        requests_fieeld_line->setText(QCoreApplication::translate("MainWindow", "Requests", nullptr));
        config_path_line->setText(QCoreApplication::translate("MainWindow", "Path to Config.json", nullptr));
        answers_field_line->setText(QCoreApplication::translate("MainWindow", "Results", nullptr));
        search_button->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
        answers_path_line->setText(QCoreApplication::translate("MainWindow", "Path to Answers.json", nullptr));
        requests_add_button->setText(QCoreApplication::translate("MainWindow", "Add...", nullptr));
        requests_delete_button->setText(QCoreApplication::translate("MainWindow", "Delete...", nullptr));
        requests_path_line->setText(QCoreApplication::translate("MainWindow", "Path to Requests.json", nullptr));
        mode_select->setItemText(0, QCoreApplication::translate("MainWindow", "Standard", nullptr));
        mode_select->setItemText(1, QCoreApplication::translate("MainWindow", "Manual", nullptr));
        mode_select->setItemText(2, QCoreApplication::translate("MainWindow", "Do not use configurations' file", nullptr));
        mode_select->setItemText(3, QCoreApplication::translate("MainWindow", "Do not use requests' file", nullptr));

        mode_select->setCurrentText(QCoreApplication::translate("MainWindow", "Standard", nullptr));
        config_path_button->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        save_button->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Engine's mode", nullptr));
        no_requests_mark->setText(QString());
        no_config_mark->setText(QString());
        requests_path_button->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        answers_path_button->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        config_change->setText(QCoreApplication::translate("MainWindow", "Configure...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWGMPYSP_H
