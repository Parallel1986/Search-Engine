//***************************************************************************//
//  This file contains search server class that creates search results by    //
// using frequency dictionaries and comparing requsts with it                //
//***************************************************************************//

#ifndef FILE_INDEX_H
#define FILE_INDEX_H
#include "inverted_index.h"

//Structure of relative index
struct RelativeIndex {
    int doc_id;             //Doncument's ID
    float rank;             //Rank of search result

    //required for unit tests
	bool operator ==(const RelativeIndex& other) const {
		return (doc_id == other.doc_id && rank == other.rank);
	}
};

class SearchServer {
public:
	/**
    * @param idx To constructor of class is received link
    * to InvertedIndex so SearchServer can get number of
    * words' instances listed in requests
    */
	SearchServer(InvertedIndex& idx) : _index(idx) { };

    /**
    * Method of processing search requests
    * @param queries_input search requests from requests' file
    * @return returns sorted list of relevant answers
    * for given requests
	*/
    QList<QList<RelativeIndex>> Search(const
        QList<QString>& queries_input);

    /**
     * @brief sets maximal requests' count
     * @param max_response
     */
    void SetMaxResponse(int max_response);
private:
    InvertedIndex _index;
	int max_response = 5;
};

#endif
