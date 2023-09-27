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

class InvertedIndex
{

public:
    InvertedIndex() = default;
/**
* Updates documents' base for search
* @param texts_input documents' content
*/
    void updateDocumentBase(QList<QString> input_docs);

/**
 * Method gets count of word's entries in the loaded base of documents
 * @param word word that count of entries needs to get
 * @return returns list of word.s entries
 */
    QList<Entry> getWordCount(const QString& word);

private:
    void createFrequencyDictionary();               //Creqtes frequency dictionary
    QList<QString> docs;                            //Contains list of documens' content
    QMap<QString, QList<Entry>> freq_dictionary;    //Frequency dictionary
};
#endif
