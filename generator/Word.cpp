/*
 * Word.cpp
 *
 *      Author: adamc
 */
#include "Word.hpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

Word::Word(string st){
	content = st;
	start_freq = 0;
	end_freq = 0;
	noNext = true;
}

string Word::getContent(){
	return content;
}

void Word::addNextWord(Word * nw){
		//first instance of this word
	if(next_words.size() == 0){
		next_words.push_back(nw); //add new word to vector
		freq_next_words.push_back(1); //only 1 instance so far
	}
	//another instance of this word already written
	else{
		bool nw_firstinstance = true;
		//SERIAL VERSION
		/*
		for(int i = 0; i<(int)next_words.size(); i++){
			if(next_words[i]->getContent() == nw->getContent()){ //found word in the vector
				freq_next_words[i]++; //increment frequency
				nw_firstinstance = false; //its not the first instance of this next word
				break; //exit for loop
			}
		}
		*/

		//PARALLEL VERSION
		int next_words_size = (int)next_words.size();
		#pragma omp parallel for private(nw,next_words,freq_next_words) shared(nw_firstinstance) schedule(static,5) num_threads(4)
		for(int i = 0; i<next_words_size; i++){
			if(next_words[i]->getContent() == nw->getContent()){ //found word in the vector
				freq_next_words[i]++; //increment frequency
				nw_firstinstance = false; //its not the first instance of this next word
			}
		}
		if(nw_firstinstance){//if new word not located in vector (first instance)
			next_words.push_back(nw); //add new word to vector
			freq_next_words.push_back(1); //only 1 instance so far
		}
	}
	noNext = false;
}

Word * Word::generateNextWord(int r){

	r = rand()%100;
	int size = (int) next_words.size();
	vector<string> nw;

	int size2 = 0; //nw size
	//add copies to nw based on next word frequency
	for(int i=0; i<size; i++){
		int count = freq_next_words[i];
		while(count>0){

			nw.push_back(next_words[i]->content);
			size2++;
			count--;
		}
	}

	//select random index of nw (random word)
	int sel = r % size2;

	//search next_words for the selected random word
	Word * w;
	for(int i = 0; i<(int)next_words.size(); i++){
		if(next_words[i]->getContent() == nw[sel]){
			w = next_words[i];
		}
	}

	return w;
}
