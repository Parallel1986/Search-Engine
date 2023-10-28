#include "exceptions.h"

QString Error::writeErrorType(FileErrorType error, const QString& source)
{
    QString message;
    switch (error) {
    case FileErrorType::NotAFile:
    case FileErrorType::FileNotExistError:
    case FileErrorType::Direcotry:
        message = "File " + source + " doesn't exists.";
        break;
    case FileErrorType::OpenFileError:
        message = "Can't open the file " + source;
        break;
    case FileErrorType::ReadFileError:
        message = "Can't read the file " + source;
        break;
    case FileErrorType::WriteFileError:
        message = "Can't write to the file " + source;
        break;
    case FileErrorType::WriteDirectoryError:
        message = "Can't write to the directory " + source;
        break;
    case FileErrorType::OpenDirectoryError:
        message = "Can't open the directory " + source;
        break;
    case FileErrorType::NoDataError:
        message = "Missing data at " + source;
        break;
    //should not get here
    default:
        message = "Unknown error with " + source;
        break;
    }
    return message;
}
