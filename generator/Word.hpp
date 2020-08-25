/*
 * Word.hpp
 *
 *  Created on: Sep 17, 2018
 *      Author: adamc
 */

#ifndef WORD_HPP_
#define WORD_HPP_

#include <vector>
#include <string>
using namespace std;

class Word {
	public:
		string content;
		vector<Word*> next_words;
		vector<int> freq_next_words;
		int start_freq;
		int end_freq;
		bool noNext;
		Word(string st);
		string getContent();
		void addNextWord(Word * nw);
		Word * generateNextWord(int r);

};

#endif /* WORD_HPP_ */
