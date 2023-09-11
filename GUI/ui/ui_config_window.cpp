#ifndef CONFIG_WINDOWCYOMIS_H
#define CONFIG_WINDOWCYOMIS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Config_window
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLineEdit *engine_ver_edit;
    QPushButton *ok_button;
    QLabel *label;
    QLabel *max_response_label;
    QLineEdit *engine_name_edit;
    QLabel *engine_ver_label;
    QLabel *engine_name_label;
    QPushButton *cancel_button;
    QSpinBox *max_response_spin;
    QPushButton *add_file_button;
    QPushButton *remove_file_button;
    QListView *file_listView;

    void setupUi(QMainWindow *Config_window)
    {
        if (Config_window->objectName().isEmpty())
            Config_window->setObjectName(QString::fromUtf8("Config_window"));
        Config_window->setEnabled(true);
        Config_window->resize(400, 320);
        centralwidget = new QWidget(Config_window);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        engine_ver_edit = new QLineEdit(centralwidget);
        engine_ver_edit->setObjectName(QString::fromUtf8("engine_ver_edit"));

        gridLayout->addWidget(engine_ver_edit, 1, 1, 1, 3);

        ok_button = new QPushButton(centralwidget);
        ok_button->setObjectName(QString::fromUtf8("ok_button"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ok_button->sizePolicy().hasHeightForWidth());
        ok_button->setSizePolicy(sizePolicy);

        gridLayout->addWidget(ok_button, 6, 2, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 3, 0, 1, 2);

        max_response_label = new QLabel(centralwidget);
        max_response_label->setObjectName(QString::fromUtf8("max_response_label"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(max_response_label->sizePolicy().hasHeightForWidth());
        max_response_label->setSizePolicy(sizePolicy1);
        max_response_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(max_response_label, 2, 0, 1, 1);

        engine_name_edit = new QLineEdit(centralwidget);
        engine_name_edit->setObjectName(QString::fromUtf8("engine_name_edit"));

        gridLayout->addWidget(engine_name_edit, 0, 1, 1, 3);

        engine_ver_label = new QLabel(centralwidget);
        engine_ver_label->setObjectName(QString::fromUtf8("engine_ver_label"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(engine_ver_label->sizePolicy().hasHeightForWidth());
        engine_ver_label->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(engine_ver_label, 1, 0, 1, 1);

        engine_name_label = new QLabel(centralwidget);
        engine_name_label->setObjectName(QString::fromUtf8("engine_name_label"));
        sizePolicy2.setHeightForWidth(engine_name_label->sizePolicy().hasHeightForWidth());
        engine_name_label->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(engine_name_label, 0, 0, 1, 1);

        cancel_button = new QPushButton(centralwidget);
        cancel_button->setObjectName(QString::fromUtf8("cancel_button"));
        sizePolicy.setHeightForWidth(cancel_button->sizePolicy().hasHeightForWidth());
        cancel_button->setSizePolicy(sizePolicy);

        gridLayout->addWidget(cancel_button, 6, 3, 1, 1);

        max_response_spin = new QSpinBox(centralwidget);
        max_response_spin->setObjectName(QString::fromUtf8("max_response_spin"));
        max_response_spin->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(max_response_spin, 2, 1, 1, 1);

        add_file_button = new QPushButton(centralwidget);
        add_file_button->setObjectName(QString::fromUtf8("add_file_button"));

        gridLayout->addWidget(add_file_button, 3, 2, 1, 1);

        remove_file_button = new QPushButton(centralwidget);
        remove_file_button->setObjectName(QString::fromUtf8("remove_file_button"));

        gridLayout->addWidget(remove_file_button, 3, 3, 1, 1);

        file_listView = new QListView(centralwidget);
        file_listView->setObjectName(QString::fromUtf8("file_listView"));
        file_listView->setEnabled(true);
        file_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        gridLayout->addWidget(file_listView, 4, 0, 1, 4);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        Config_window->setCentralWidget(centralwidget);

        retranslateUi(Config_window);
        QObject::connect(ok_button, SIGNAL(clicked()), Config_window, SLOT(clickedOk()));
        QObject::connect(cancel_button, SIGNAL(clicked()), Config_window, SLOT(clickedCancel()));
        QObject::connect(remove_file_button, SIGNAL(clicked()), Config_window, SLOT(clickedRemove()));
        QObject::connect(add_file_button, SIGNAL(clicked()), Config_window, SLOT(clickedAdd()));

        QMetaObject::connectSlotsByName(Config_window);
    } // setupUi

    void retranslateUi(QMainWindow *Config_window)
    {
        Config_window->setWindowTitle(QCoreApplication::translate("Config_window", "Config.json settings", nullptr));
        ok_button->setText(QCoreApplication::translate("Config_window", "Ok", nullptr));
        label->setText(QCoreApplication::translate("Config_window", "Search file list", nullptr));
        max_response_label->setText(QCoreApplication::translate("Config_window", "Max responses", nullptr));
        engine_ver_label->setText(QCoreApplication::translate("Config_window", "Engine version", nullptr));
        engine_name_label->setText(QCoreApplication::translate("Config_window", "Engine name", nullptr));
        cancel_button->setText(QCoreApplication::translate("Config_window", "Cancel", nullptr));
        add_file_button->setText(QCoreApplication::translate("Config_window", "Add", nullptr));
        remove_file_button->setText(QCoreApplication::translate("Config_window", "Remove", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Config_window: public Ui_Config_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // CONFIG_WINDOWCYOMIS_H
