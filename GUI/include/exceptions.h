//***************************************************************************//
//  This file contains declarations of the different exception classes, that //
// are used to determine different error events                              //
//***************************************************************************//
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "QException"

enum ExceptionType{
    OpenFileError,
    WriteFileError,
    FileNotExistError,
    NoDataError
};

class EngineError : public QException
{
public:
    void raise() const override { throw *this; }
    EngineError *clone() const override { return new EngineError(*this); }
    ExceptionType getExceptionType() const {return ex;}
    QString getExceptionSource() const {return source;}
    QString getAdditionalData() const {return additional_data;}
private:
    ExceptionType ex;
    QString source;
    QString additional_data;
};


#endif // EXCEPTIONS_H
