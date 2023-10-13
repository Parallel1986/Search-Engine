#ifndef MAINWINDOWUGTHQY_H
#define MAINWINDOWUGTHQY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
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
    QPushButton *config_save_button;
    QLabel *answers_path_line;
    QPushButton *config_path_button;
    QFrame *line;
    QLabel *requests_path_line;
    QFrame *line_2;
    QFrame *line_4;
    QComboBox *mode_select;
    QFrame *line_3;
    QLabel *label_2;
    QListView *requests_line_list;
    QVBoxLayout *verticalLayout;
    QPushButton *requests_add_button;
    QPushButton *requests_delete_button;
    QSpacerItem *verticalSpacer;
    QPushButton *save_as_text_button;
    QTreeWidget *results_tree_widget;
    QLabel *answers_field_line;
    QHBoxLayout *horizontalLayout_2;
    QLabel *engine_name_line;
    QLineEdit *engine_name_edit;
    QLabel *engine_version_line;
    QLineEdit *engine_version_edit;
    QLabel *label_4;
    QSpinBox *max_response_spin;
    QLabel *label;
    QHBoxLayout *horizontalLayout_3;
    QLabel *files_line;
    QLabel *no_files_mark;
    QSpacerItem *horizontalSpacer;
    QPushButton *search_button;
    QListView *file_view;
    QLabel *config_path_line;
    QVBoxLayout *verticalLayout_3;
    QPushButton *add_file_button;
    QPushButton *delete_file_button;
    QSpacerItem *verticalSpacer_2;
    QPushButton *save_button;
    QHBoxLayout *horizontalLayout_4;
    QLabel *requests_fieeld_line;
    QLabel *no_requests_mark_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *generate_config_button;
    QPushButton *requests_path_button;
    QPushButton *answers_path_button;
    QLabel *no_requests_path_mark;
    QLabel *no_config_mark;
    QLineEdit *answers_path_edit;
    QLineEdit *requests_path_edit;
    QLineEdit *config_path_edit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setEnabled(true);
        MainWindow->resize(681, 614);
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
        config_save_button = new QPushButton(centralwidget);
        config_save_button->setObjectName(QString::fromUtf8("config_save_button"));

        gridLayout->addWidget(config_save_button, 8, 4, 1, 1);

        answers_path_line = new QLabel(centralwidget);
        answers_path_line->setObjectName(QString::fromUtf8("answers_path_line"));

        gridLayout->addWidget(answers_path_line, 5, 0, 1, 1);

        config_path_button = new QPushButton(centralwidget);
        config_path_button->setObjectName(QString::fromUtf8("config_path_button"));

        gridLayout->addWidget(config_path_button, 2, 4, 1, 1);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 17, 0, 1, 5);

        requests_path_line = new QLabel(centralwidget);
        requests_path_line->setObjectName(QString::fromUtf8("requests_path_line"));

        gridLayout->addWidget(requests_path_line, 3, 0, 1, 1);

        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_2, 7, 0, 1, 5);

        line_4 = new QFrame(centralwidget);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_4, 10, 0, 1, 5);

        mode_select = new QComboBox(centralwidget);
        mode_select->addItem(QString());
        mode_select->addItem(QString());
        mode_select->addItem(QString());
        mode_select->addItem(QString());
        mode_select->setObjectName(QString::fromUtf8("mode_select"));
        mode_select->setMaxVisibleItems(5);
        mode_select->setMaxCount(4);
        mode_select->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(mode_select, 9, 4, 1, 1);

        line_3 = new QFrame(centralwidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_3, 14, 0, 1, 5);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 8, 0, 1, 1);

        requests_line_list = new QListView(centralwidget);
        requests_line_list->setObjectName(QString::fromUtf8("requests_line_list"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(requests_line_list->sizePolicy().hasHeightForWidth());
        requests_line_list->setSizePolicy(sizePolicy);
        requests_line_list->setEditTriggers(QAbstractItemView::SelectedClicked);

        gridLayout->addWidget(requests_line_list, 16, 0, 1, 4);

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


        gridLayout->addLayout(verticalLayout, 16, 4, 1, 1);

        save_as_text_button = new QPushButton(centralwidget);
        save_as_text_button->setObjectName(QString::fromUtf8("save_as_text_button"));
        save_as_text_button->setEnabled(false);

        gridLayout->addWidget(save_as_text_button, 20, 1, 1, 3);

        results_tree_widget = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(2, QString::fromUtf8("3"));
        __qtreewidgetitem->setText(1, QString::fromUtf8("2"));
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        results_tree_widget->setHeaderItem(__qtreewidgetitem);
        results_tree_widget->setObjectName(QString::fromUtf8("results_tree_widget"));
        sizePolicy.setHeightForWidth(results_tree_widget->sizePolicy().hasHeightForWidth());
        results_tree_widget->setSizePolicy(sizePolicy);
        results_tree_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        results_tree_widget->setProperty("showDropIndicator", QVariant(false));
        results_tree_widget->setAlternatingRowColors(false);
        results_tree_widget->setSelectionMode(QAbstractItemView::NoSelection);
        results_tree_widget->setRootIsDecorated(true);
        results_tree_widget->setColumnCount(3);
        results_tree_widget->header()->setVisible(false);

        gridLayout->addWidget(results_tree_widget, 19, 0, 1, 5);

        answers_field_line = new QLabel(centralwidget);
        answers_field_line->setObjectName(QString::fromUtf8("answers_field_line"));

        gridLayout->addWidget(answers_field_line, 18, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        engine_name_line = new QLabel(centralwidget);
        engine_name_line->setObjectName(QString::fromUtf8("engine_name_line"));

        horizontalLayout_2->addWidget(engine_name_line);

        engine_name_edit = new QLineEdit(centralwidget);
        engine_name_edit->setObjectName(QString::fromUtf8("engine_name_edit"));
        engine_name_edit->setEnabled(true);
        engine_name_edit->setReadOnly(true);

        horizontalLayout_2->addWidget(engine_name_edit);

        engine_version_line = new QLabel(centralwidget);
        engine_version_line->setObjectName(QString::fromUtf8("engine_version_line"));

        horizontalLayout_2->addWidget(engine_version_line);

        engine_version_edit = new QLineEdit(centralwidget);
        engine_version_edit->setObjectName(QString::fromUtf8("engine_version_edit"));
        engine_version_edit->setEnabled(true);
        engine_version_edit->setReadOnly(true);

        horizontalLayout_2->addWidget(engine_version_edit);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(label_4);

        max_response_spin = new QSpinBox(centralwidget);
        max_response_spin->setObjectName(QString::fromUtf8("max_response_spin"));
        max_response_spin->setMinimum(0);
        max_response_spin->setMaximum(999);
        max_response_spin->setValue(5);

        horizontalLayout_2->addWidget(max_response_spin);


        gridLayout->addLayout(horizontalLayout_2, 9, 0, 1, 2);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 9, 3, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        files_line = new QLabel(centralwidget);
        files_line->setObjectName(QString::fromUtf8("files_line"));

        horizontalLayout_3->addWidget(files_line);

        no_files_mark = new QLabel(centralwidget);
        no_files_mark->setObjectName(QString::fromUtf8("no_files_mark"));
        no_files_mark->setEnabled(true);
        no_files_mark->setPixmap(QPixmap(QString::fromUtf8("icons/warning_small.png")));

        horizontalLayout_3->addWidget(no_files_mark);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_3, 12, 0, 1, 1);

        search_button = new QPushButton(centralwidget);
        search_button->setObjectName(QString::fromUtf8("search_button"));
        search_button->setEnabled(false);

        gridLayout->addWidget(search_button, 18, 4, 1, 1);

        file_view = new QListView(centralwidget);
        file_view->setObjectName(QString::fromUtf8("file_view"));
        sizePolicy.setHeightForWidth(file_view->sizePolicy().hasHeightForWidth());
        file_view->setSizePolicy(sizePolicy);

        gridLayout->addWidget(file_view, 13, 0, 1, 4);

        config_path_line = new QLabel(centralwidget);
        config_path_line->setObjectName(QString::fromUtf8("config_path_line"));

        gridLayout->addWidget(config_path_line, 1, 0, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        add_file_button = new QPushButton(centralwidget);
        add_file_button->setObjectName(QString::fromUtf8("add_file_button"));

        verticalLayout_3->addWidget(add_file_button);

        delete_file_button = new QPushButton(centralwidget);
        delete_file_button->setObjectName(QString::fromUtf8("delete_file_button"));

        verticalLayout_3->addWidget(delete_file_button);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);


        gridLayout->addLayout(verticalLayout_3, 13, 4, 1, 1);

        save_button = new QPushButton(centralwidget);
        save_button->setObjectName(QString::fromUtf8("save_button"));
        save_button->setEnabled(false);

        gridLayout->addWidget(save_button, 20, 4, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        requests_fieeld_line = new QLabel(centralwidget);
        requests_fieeld_line->setObjectName(QString::fromUtf8("requests_fieeld_line"));

        horizontalLayout_4->addWidget(requests_fieeld_line);

        no_requests_mark_2 = new QLabel(centralwidget);
        no_requests_mark_2->setObjectName(QString::fromUtf8("no_requests_mark_2"));
        no_requests_mark_2->setEnabled(true);
        no_requests_mark_2->setPixmap(QPixmap(QString::fromUtf8("icons/warning_small.png")));

        horizontalLayout_4->addWidget(no_requests_mark_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_4, 15, 0, 1, 1);

        generate_config_button = new QPushButton(centralwidget);
        generate_config_button->setObjectName(QString::fromUtf8("generate_config_button"));

        gridLayout->addWidget(generate_config_button, 1, 4, 1, 1);

        requests_path_button = new QPushButton(centralwidget);
        requests_path_button->setObjectName(QString::fromUtf8("requests_path_button"));

        gridLayout->addWidget(requests_path_button, 4, 4, 1, 1);

        answers_path_button = new QPushButton(centralwidget);
        answers_path_button->setObjectName(QString::fromUtf8("answers_path_button"));

        gridLayout->addWidget(answers_path_button, 6, 4, 1, 1);

        no_requests_path_mark = new QLabel(centralwidget);
        no_requests_path_mark->setObjectName(QString::fromUtf8("no_requests_path_mark"));
        no_requests_path_mark->setEnabled(true);
        no_requests_path_mark->setPixmap(QPixmap(QString::fromUtf8("icons/warning_small.png")));

        gridLayout->addWidget(no_requests_path_mark, 4, 3, 1, 1);

        no_config_mark = new QLabel(centralwidget);
        no_config_mark->setObjectName(QString::fromUtf8("no_config_mark"));
        no_config_mark->setEnabled(true);
        no_config_mark->setPixmap(QPixmap(QString::fromUtf8("icons/warning_small.png")));

        gridLayout->addWidget(no_config_mark, 2, 3, 1, 1);

        answers_path_edit = new QLineEdit(centralwidget);
        answers_path_edit->setObjectName(QString::fromUtf8("answers_path_edit"));

        gridLayout->addWidget(answers_path_edit, 6, 0, 1, 3);

        requests_path_edit = new QLineEdit(centralwidget);
        requests_path_edit->setObjectName(QString::fromUtf8("requests_path_edit"));

        gridLayout->addWidget(requests_path_edit, 4, 0, 1, 3);

        config_path_edit = new QLineEdit(centralwidget);
        config_path_edit->setObjectName(QString::fromUtf8("config_path_edit"));

        gridLayout->addWidget(config_path_edit, 2, 0, 1, 3);


        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);
        QObject::connect(requests_path_button, SIGNAL(clicked()), MainWindow, SLOT(openRequests()));
        QObject::connect(answers_path_button, SIGNAL(clicked()), MainWindow, SLOT(openAnswers()));
        QObject::connect(requests_add_button, SIGNAL(clicked()), MainWindow, SLOT(addRequests()));
        QObject::connect(requests_delete_button, SIGNAL(clicked()), MainWindow, SLOT(deleteRequests()));
        QObject::connect(config_path_edit, SIGNAL(textChanged(QString)), MainWindow, SLOT(setConfigPath(QString)));
        QObject::connect(requests_path_edit, SIGNAL(textChanged(QString)), MainWindow, SLOT(setRequestsPath(QString)));
        QObject::connect(answers_path_edit, SIGNAL(textChanged(QString)), MainWindow, SLOT(setAnswersPath(QString)));
        QObject::connect(search_button, SIGNAL(clicked()), MainWindow, SLOT(search()));
        QObject::connect(save_button, SIGNAL(clicked()), MainWindow, SLOT(saveResult()));
        QObject::connect(mode_select, SIGNAL(currentIndexChanged(int)), MainWindow, SLOT(changeMode(int)));
        QObject::connect(delete_file_button, SIGNAL(clicked()), MainWindow, SLOT(deleteFile()));
        QObject::connect(add_file_button, SIGNAL(clicked()), MainWindow, SLOT(addFile()));
        QObject::connect(config_save_button, SIGNAL(clicked()), MainWindow, SLOT(configSave()));
        QObject::connect(generate_config_button, SIGNAL(clicked()), MainWindow, SLOT(generateConfig()));
        QObject::connect(config_path_button, SIGNAL(clicked()), MainWindow, SLOT(openConfig()));
        QObject::connect(save_as_text_button, SIGNAL(clicked()), MainWindow, SLOT(saveResultAsText()));

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
        config_save_button->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        answers_path_line->setText(QCoreApplication::translate("MainWindow", "Path to Answers.json", nullptr));
        config_path_button->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        requests_path_line->setText(QCoreApplication::translate("MainWindow", "Path to Requests.json", nullptr));
        mode_select->setItemText(0, QCoreApplication::translate("MainWindow", "Standard", nullptr));
        mode_select->setItemText(1, QCoreApplication::translate("MainWindow", "Manual", nullptr));
        mode_select->setItemText(2, QCoreApplication::translate("MainWindow", "Do not use configurations' file", nullptr));
        mode_select->setItemText(3, QCoreApplication::translate("MainWindow", "Do not use requests' file", nullptr));

        mode_select->setCurrentText(QCoreApplication::translate("MainWindow", "Standard", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Configuration", nullptr));
        requests_add_button->setText(QCoreApplication::translate("MainWindow", "Add...", nullptr));
        requests_delete_button->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        save_as_text_button->setText(QCoreApplication::translate("MainWindow", "Save as text", nullptr));
        answers_field_line->setText(QCoreApplication::translate("MainWindow", "Results", nullptr));
        engine_name_line->setText(QCoreApplication::translate("MainWindow", "Engine's name", nullptr));
        engine_version_line->setText(QCoreApplication::translate("MainWindow", "Engine's version", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Response limit", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Engine's mode", nullptr));
        files_line->setText(QCoreApplication::translate("MainWindow", "Files", nullptr));
        no_files_mark->setText(QString());
        search_button->setText(QCoreApplication::translate("MainWindow", "Search", nullptr));
        config_path_line->setText(QCoreApplication::translate("MainWindow", "Path to Config.json", nullptr));
        add_file_button->setText(QCoreApplication::translate("MainWindow", "Add...", nullptr));
        delete_file_button->setText(QCoreApplication::translate("MainWindow", "Delete", nullptr));
        save_button->setText(QCoreApplication::translate("MainWindow", "Save as JSON", nullptr));
        requests_fieeld_line->setText(QCoreApplication::translate("MainWindow", "Requests", nullptr));
        no_requests_mark_2->setText(QString());
        generate_config_button->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        requests_path_button->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        answers_path_button->setText(QCoreApplication::translate("MainWindow", "Open...", nullptr));
        no_requests_path_mark->setText(QString());
        no_config_mark->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWUGTHQY_H