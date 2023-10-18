//***************************************************************************//
// This file contains declarations of the different exception classes, that  //
// are used to determine different error events                              //
//***************************************************************************//
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "QException"

enum ExceptionType{
    OpenFileError,
    WriteFileError,
    ReadFileError,
    OpenDirectoryError,
    WriteDirectoryError,
    FileNotExistError,
    NoDataError
};

struct FileError : public QException
{
public:
    void raise() const override { throw *this; }
    FileError *clone() const override { return new FileError(*this); }
    FileError(ExceptionType exc, QString source, QString info):
    ex(exc),source(source),additional_data(info){}
    ExceptionType getExceptionType() const {return ex;}
    QString getExceptionSource() const {return source;}
    QString getAdditionalData() const {return additional_data;}
private:
    ExceptionType ex;
    QString source;
    QString additional_data;
};

#endif // EXCEPTIONS_H
