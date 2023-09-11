#include "../include/requestadddialog.h"


RequestAddDialog::RequestAddDialog(QWidget* parent)
{
    dialog = new Ui::request_add_dialog;
    dialog->setupUi(this);
}


RequestAddDialog::~RequestAddDialog()
{
    delete dialog;
}

void RequestAddDialog::accept()
{
    emit newRequest(dialog->request_field_edit->text().toLower());
    this->close();
}
