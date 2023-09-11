#ifndef REQUESTADDDIALOG_H
#define REQUESTADDDIALOG_H

#include <QObject>
#include <QDialog>
#include "../source/request_dialog.cpp"

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
