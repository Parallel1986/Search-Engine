#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <QList>
#include <QMap>
#include <QObject>

//Structure of word's entry
struct Entry
{
    int doc_id, count;
    Entry(){}
    Entry(int in_doc_id, int in_count) : doc_id(in_doc_id), count(in_count){}

    // Required for unit tests
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};

namespace IndexParser {
    QStringList splitTextToWords(QString);
    QMap<QString,Entry> fillEntryDictionary(const QString& text_of_document, int doc_id);
}

class InvertedIndex
{

public:
    InvertedIndex() = default;

    void updateDocumentBase(QList<QString> input_docs);

    QList<Entry> getWordCount(const QString& word);

private:
    void createFrequencyDictionary();
    QStringList docs;
    QMap<QString, QList<Entry>> freq_dictionary;
};
#endif
