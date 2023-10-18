//***************************************************************************//
//  This file contains RequestAddDialog class that creates dialog window for //
// adding new requests                                                       //
//***************************************************************************//
#ifndef REQUESTADDDIALOG_H
#define REQUESTADDDIALOG_H

#include <QObject>
#include <QDialog>
#include "../ui/ui_request_dialog.cpp"

class RequestAddDialog : public QDialog
{
    Q_OBJECT
public:
    RequestAddDialog(QWidget*);
    ~RequestAddDialog();

public slots:
    void accept();
signals:
    void newRequest(QString);
private:
    Ui::request_add_dialog* dialog;
};

#endif // REQUESTADDDIALOG_H
