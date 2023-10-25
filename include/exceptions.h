//***************************************************************************//
// This file contains declarations of the different exception classes, that  //
// are used to determine different error events                              //
//***************************************************************************//
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "QException"
#include <exception>

enum FileErrorType{
    NotAFile,
    OpenFileError,
    WriteFileError,
    ReadFileError,
    OpenDirectoryError,
    WriteDirectoryError,
    FileNotExistError,
    NoDataError
};

class FileError : public QException
{
public:
    void raise() const override
    {
        throw *this;
    }
    FileError *clone() const override
    {
        return new FileError(*this);
    }
    FileError(FileErrorType exc, QString source, QString info):
    ex(exc),source(source),additional_data(info){}
    FileErrorType getExceptionType() const
    {
        return ex;
    }
    QString getExceptionSource() const
    {
        return source;
    }
    QString getAdditionalData() const
    {
        return additional_data;
    }
private:
    FileErrorType ex;
    QString source;
    QString additional_data;
};

enum CommandLineErrorType
{
    INVALID_ARGUMENT,
    UNKNOWN_ARGUMENT,
    MISSING_ARGUMENT
};

class CommandLineError : public std::exception
{
    public:
    CommandLineError(CommandLineErrorType type, QString argument, QString additional_info)
    : type(type), argument(argument), additional_info(additional_info){}
    QString getArgument()
    {
        return argument;
    }
    QString getAdditionalInfo()
    {
        return additional_info;
    }
    CommandLineErrorType getType()
    {
        return type;
    }
    private:
    CommandLineErrorType type;
    QString argument;
    QString additional_info;
};

#endif // EXCEPTIONS_H
