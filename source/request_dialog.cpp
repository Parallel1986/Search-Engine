#ifndef REQUEST_EDITFVXVFA_H
#define REQUEST_EDITFVXVFA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_request_add_dialog
{
public:
    QGridLayout *gridLayout;
    QLineEdit *request_field_edit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *request_add_dialog)
    {
        if (request_add_dialog->objectName().isEmpty())
            request_add_dialog->setObjectName(QString::fromUtf8("request_add_dialog"));
        request_add_dialog->setWindowModality(Qt::NonModal);
        request_add_dialog->resize(480, 70);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(request_add_dialog->sizePolicy().hasHeightForWidth());
        request_add_dialog->setSizePolicy(sizePolicy);
        request_add_dialog->setMaximumSize(QSize(1080, 70));
        request_add_dialog->setModal(false);
        gridLayout = new QGridLayout(request_add_dialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        request_field_edit = new QLineEdit(request_add_dialog);
        request_field_edit->setObjectName(QString::fromUtf8("request_field_edit"));

        gridLayout->addWidget(request_field_edit, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(request_add_dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(false);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(request_add_dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), request_add_dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), request_add_dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(request_add_dialog);
    } // setupUi

    void retranslateUi(QDialog *request_add_dialog)
    {
        request_add_dialog->setWindowTitle(QCoreApplication::translate("request_add_dialog", "Enter request", nullptr));
    } // retranslateUi

};

namespace Ui {
    class request_add_dialog: public Ui_request_add_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // REQUEST_EDITFVXVFA_H
