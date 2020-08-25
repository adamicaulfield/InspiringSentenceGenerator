/*
 * Main.cpp
 *
 *      Author: adamic
 */
#include "Word.hpp"
#include <iostream>
#include <stdlib.h>
#include "omp.h"
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

//Method Declarations
vector<string> stringtoVector(string sent);
void strvectToWordVect(vector<string> str_vect);
int searchDictionary(string w);
void readSentence(string sentence);
void inspectDictionary();
Word * generateFirstWord(int r);
vector<Word*> generateSentence(int r, int totalLines);

//Global Variables
vector<Word*> dictionary;

//Global timing variables
long int str_to_vect_duration= 0;
long int strvect_to_Wordvect_duration= 0;
long int searchDict_duration= 0;
long int addWord_duration= 0;

int main(){
	srand(time(NULL));

	auto start = high_resolution_clock::now();
	ifstream file2 ("inspire.txt");
	string line2;
	vector<string> reformed;

	if(file2.is_open())
	{
		while(getline(file2,line2))
		{
			for(int i = 0; i<line2.size()-1; i++){
				if(line2[i]=='.' && line2[i+1]!='\n'){
					line2.replace(i+1,0,"");
					line2.replace(i,1,"\n");
				}
			}
			reformed.push_back(line2);
		}
	}
	auto stop = high_resolution_clock::now();
	auto reformed_dur = duration_cast<microseconds>(stop-start);
	
	start = high_resolution_clock::now();
	ofstream outputFile;
	outputFile.open("reformed.txt");
	for(int i = 0; i<reformed.size(); i++){
		outputFile << reformed[i];
	}
	stop = high_resolution_clock::now();
	auto output_reformed_dur = duration_cast<microseconds>(stop-start);

	start = high_resolution_clock::now();
 	string line;
	ifstream file ("reformed.txt");
	int totalLines = 0;
	if (file.is_open())
	{

		while (getline(file,line))
		{
			if(line!=""){
				readSentence(line);
				totalLines++;
			}
		}
		file.close();
	}
	stop = high_resolution_clock::now();
	auto read_and_process = duration_cast<microseconds>(stop-start);

	start = high_resolution_clock::now();
	vector<Word*> sentence;
	int count = 0;
//	int num_sentences = 1;//3;
//	while(count<num_sentences){
	
	sentence = generateSentence(rand()%1000, totalLines);

	//output sentence
	cout << "totalLines read: " << totalLines << endl;
//	cout << "Sentence " << count << ": ";
	for(int i = 0; i < (int)sentence.size(); i++){
		cout << sentence[i]->getContent() << " ";
	}
	cout << "." << endl;
	cout << endl;
	count ++;
//	}
	stop = high_resolution_clock::now();
	auto create_sentence_dur = duration_cast<microseconds>(stop-start);

	ofstream analysisFile;
	analysisFile.open("analysis.csv", fstream::app);
	analysisFile << "Program Block,Execution Time (micro-s)" << endl;
	analysisFile << "Reform Input,"<< reformed_dur.count() << endl;
	analysisFile << "Output Reformed," << output_reformed_dur.count() << endl;
	analysisFile << "Read and Process Input," << read_and_process.count() << endl;
	analysisFile << "String to Vector," << str_to_vect_duration << endl;
	analysisFile << "SearchDictionary," << searchDict_duration << endl;
	analysisFile << "Add Next Word," << addWord_duration << endl;
	analysisFile << "Create Sentence," << create_sentence_dur.count() << endl;
	analysisFile.close();

/*
	printf("Program Block\t\t Execution Time (micro-s)\n");
	printf("Reform Input\t\t %ld\n", reformed_dur.count());
	printf("Output Reformed\t\t %ld\n", output_reformed_dur.count());
	printf("Read and Process Input\t %ld\n", read_and_process.count());
	printf("-> String to Vector\t\t %ld\n", str_to_vect_duration);
	printf("-> StrVect to WordVect\t\t %ld\n", strvect_to_Wordvect_duration);
	printf("    -> SearchDictionary\t\t %ld\n", searchDict_duration);
	printf("    -> Add Next Word\t\t %ld\n", addWord_duration);
	printf("Create Sentence\t %ld\n", create_sentence_dur.count());
*/
	return 0;

}

Word * generateFirstWord(int r){
	int size = (int) dictionary.size();
	vector<string> nw;

	//use start freq of each word to add that many copies to nw
	for(int i=0; i<size; i++){
		int count = dictionary[i]->start_freq;
		while(count>0){
			nw.push_back(dictionary[i]->content);
			count--;
		}
	}

	//select random number from nw
	int sel = (r% (int)nw.size());

	//search dictionary for word selected
	Word * w;
	for(int i = 0; i<(int)dictionary.size(); i++){
		if(dictionary[i]->getContent() == nw[sel]){
			w = dictionary[i];
		}
	}
	return w;
}

vector<Word*> generateSentence(int r, int totalLines){
	int num_words = r%10+10;
	vector<Word*> sentence;
	int i = 1;

	Word * currentWord = generateFirstWord(rand());

	int cont = rand()%totalLines+1;
	sentence.push_back(currentWord);
	while(currentWord->end_freq != cont){
		if(currentWord->noNext){
			i=num_words;
		}
		else{
			Word * w = currentWord->generateNextWord(r); 
			sentence.push_back(w);
			currentWord = w;
			i++;
		}
		cont = rand()%totalLines+1;
	}

	return sentence;
}

void readSentence(string sentence){
	vector<string> vs = stringtoVector(sentence);
	strvectToWordVect(vs);
}

void inspectDictionary(){
	cout << "Inspecting next words: " << endl;
	for(int i=0; i<(int)dictionary.size(); i++){
		if(dictionary[i]->end_freq > 0){
			cout << "word: " << dictionary[i]->getContent() << "\tend_freq: " << dictionary[i]->end_freq << endl;
		}
//		cout << dictionary[i]->getContent() << ": ";
//		for(int j = 0; j<(int)dictionary[i]->next_words.size(); j++){
//			cout << dictionary[i]->next_words[j]->getContent() << "(" << dictionary[i]->freq_next_words[j] << "),";
//		}
		//cout << endl;
	}
//	cout << endl;
}

vector<string> stringtoVector(string sent){
		auto startTimer = high_resolution_clock::now();
		vector<string> fresh; //vector to return
		int i = 0;
		int start = 0;	//index of string to start on for substring
		while(i<(int)sent.size()){

			if(sent[i]==' '){ //once space is reached, get all chars before as a string, add to vector. 
				fresh.push_back(sent.substr(start,i-start));
				start = i+1;
			}
			if(i==(int)sent.size()-1){ //if end of string is reached, add all remaining chars to vector
				fresh.push_back(sent.substr(start,(int)sent.size()-start));
				start = i+1;
			}
			i++;
		}
		auto stopTimer = high_resolution_clock::now();
		auto duration= duration_cast<microseconds>(stopTimer 	-startTimer);
		str_to_vect_duration = str_to_vect_duration + duration.count();
		return fresh;
}

void strvectToWordVect(vector<string> str_vect){
	auto start = high_resolution_clock::now();
	int s0_index = searchDictionary(str_vect[0]);
	Word * w1;
	if(s0_index != -1){//found in dictionary
		w1 = dictionary[s0_index];	//point to the word in dict. that is the first word int the sentence
	}
	else{ //not found in dictionary. Declare new word and add to dictionary
		w1 = new Word(str_vect[0]);
		dictionary.push_back(w1);
	}
	w1->start_freq++; //incr start freq
	int i = 1;
	//repeat for all words in string vector
	while(i < (int)str_vect.size()){
		int index = searchDictionary(str_vect[i]); //search dictionary for next word index
		Word * w2;
		if(index != -1){ //found in dictionary
			w2 = dictionary[index];
		}
		else{ // not in dictionary
			w2 = new Word(str_vect[i]);
			dictionary.push_back(w2);
		}
		auto startWord = high_resolution_clock::now();
		w1->addNextWord(w2); //add word2 to word1 nextWord
		auto stopWord = high_resolution_clock::now();
		auto durationWord = duration_cast<microseconds>(stopWord-startWord);
		addWord_duration = addWord_duration + durationWord.count();
		w1 = w2;
		i++;
	}
	w1->end_freq++;
	auto stop = high_resolution_clock::now();
	auto duration= duration_cast<microseconds>(stop-start);
	strvect_to_Wordvect_duration = strvect_to_Wordvect_duration + duration.count();
}

int searchDictionary(string w){
	//SERIAL VERSION
	/*
		auto start = high_resolution_clock::now();
	//find the index of search word 'w' in the dictionary
	int newWord = -1;
	int i;
	for(i=0; i<(int)dictionary.size(); i++){
		if(dictionary[i]->content==w){
			newWord = i;
			//break;
		}
	}
	auto stop = high_resolution_clock::now();
	auto duration= duration_cast<microseconds>(stop-start);
	searchDict_duration = searchDict_duration + duration.count();

	return newWord;
	*/


	//PARALLEL VERSION
//	/*
	auto start = high_resolution_clock::now();
	//find the index of search word 'w' in the dictionary
	int newWord_t[4] = {-1,-1,-1,-1};
	int newWord = -1;
	int i;
	int j;
	//cout << "start" << endl;
	int dict_size = (int)dictionary.size();
	#pragma omp parallel for private(w,dictionary) schedule(static,50) num_threads(4)
	for(i=0; i<(int)dictionary.size(); i++){
		if(dictionary[i]->content==w){
			newWord_t[omp_get_thread_num()] = i;
			//break;
		}
	}
	//cout << "finish" << endl; ;

	if(newWord_t[0] != -1)
		newWord = newWord_t[0];
	else if(newWord_t[1] != -1)
		newWord = newWord_t[1];
	else if(newWord_t[2] != -1)
		newWord = newWord_t[2];
	else if(newWord_t[3] != -1)
		newWord = newWord_t[3];

	auto stop = high_resolution_clock::now();
	auto duration= duration_cast<microseconds>(stop-start);
	searchDict_duration = searchDict_duration + duration.count();

	return newWord;
//	 */
}