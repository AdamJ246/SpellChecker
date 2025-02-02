#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>
#include <vector>
using namespace std;
#include "HashTable.h"
#include "BinarySearchTree.h"

struct BNode{
	string word;
	int lineNum;
	bool operator < (BNode otherStruct) const{
		return (word < otherStruct.word);
	}
};
typedef BinarySearchTree<BNode> BST;
ostream & operator << (ostream & out, BNode & temp){
	out << temp.word << " " << temp.lineNum;
	return out;
}

void runTheProgram(ifstream& file);
void detectionDecision(string inputWd, int lineNum, bool& userQuit, bool& userIgnore, HashTable& dictWordTable, BST& myBST);
void dictInsert(string inputWd);
string suggestedSpell(string inputWd, HashTable& dictWordTable);
void insertOrUpdate(BST& myBST, string inputWd, int lineNum);
void insertMisspelled(BST&myBST);
string swap(string word, char c1, char c2, int mov);


/* Beginning with the main funciton for this spellchecking program,
 * its primary funciton is to open the file that the user input on
 * the command line and wishes to spellcheck.
*/
int main(int argc, char *argv[]){
	if(argc != 2)
		cerr << "Usage: " << argv[0] << " <filename>" << endl;
	ifstream file(argv[1]);
	//if the file is open, the primary "runTheProgram" function will run and take with
	//it the spell checkable file the user gave.
	if(file.is_open()){
		runTheProgram(file);
		file.close();
	}else{
		cerr << "Error: Unable to open file." << endl;
		return 1;
	}
return 0;
}

/* Within this major function, the bulk of the program is run.
 * Here, taking in the ifstream file, the program opens a stream to the dictionary text file
 * and stores it into a hash table.
 * Second, the actual text file is analyzed and if the program detects a word that is not found
 * within the dictionary, it will call the second primary function "decisionDetection" which allows
 * the user to decide what to do with the word.
 * This repeats until there is nothing left to scan and the program ends.
*/
void runTheProgram(ifstream& file){
	cout << "!@#$%^&*(){} THE SPELL CHECKER PROGRAM !@#$%^&*(){}\n\n";
	//definition of variables, tables, and BST creation here:
	string line;
	string word;
	bool userQuit = false;
	bool userIgnore = false;
	HashTable dictWordTable(2000);
	HashTable ignoredRefTable(200);
	BNode defaultNode;
	defaultNode.word = "";
	defaultNode.lineNum = 0;
	BST myBST(defaultNode); 
	int lineCounter = 1;
	//open the dictionary file and read its contents into the dictionary hash.
	ifstream dictFile("dict.txt");
	while(dictFile >> word){
		dictWordTable.insert(word);
	}
	dictFile.close();
	//here, analyze the spellcheckable text file line by line until there are no lines.
	while(getline(file, line)){
		istringstream iss(line);
		//analyze the text file second word by word until there are no words in the line.
		while(iss >> word && userQuit != true){
			//make the current word lowercase in all spaces.
			for(int i=0; i < word.length(); i++){
				if(isalpha(word[i]))
					word[i] = tolower(word[i]);
			}
			//clear the current word of any unnecessary punctuation.
			for(int i=0; i < word.length(); i++){
				if(isspace(word[i]) || ispunct(word[i]) && (!isalpha(word[i+1]) && !isdigit(word[i+1]))){
					word = word.substr(0, word.length() - 1);
				}
			}
			//if the word is unable to be found in the dictionary and is also not found in the ignored Hash table,
			//proceed to prompt the user with a decision to address the currently problematic word.
			if(dictWordTable.find(word) == false && word != "\n" && ignoredRefTable.find(word) == false){
				detectionDecision(word, lineCounter, userQuit, userIgnore, dictWordTable, myBST);
				//store any ignored words into the ignored Hash table.
				if(userIgnore == true){
					ignoredRefTable.insert(word);
					userIgnore = false;
				}
			}
		}
		lineCounter++;
	}
	//here, we insert the collected misspelled words into the "notfound.txt" file in ascending order.
	if(!userQuit)
		cout << "\n   *** You have reached the END of the text file! *** \n";
	insertMisspelled(myBST);
	cout << "\nNow Exiting Program . . .\n";
}

/* Here is the second primary function responsible for taking the most user input.
 * When a current word is not detected within the dictionary, the program will prompt the user
 * to make one of FIVE decisions listed here:
 * A) the user adds the word to both the dictionary Hash table AND the dictionary text file permanently.
 * I) the user ignores the word and flags it to be ignored by the detection function for the rest of the current run.
 * G) the user proceeds onto the next word without doing anything to it.
 * S) the user calls for a word with similar spelling to be suggested. After displaying it, the program waits for a return input to move on.
 * Q) the user quits the entire program and it ends abrubtly.
 * after a decision is made, cases are run and the code returns back to the next word in the text scan.
*/
void detectionDecision(string inputWd, int lineNum, bool& userQuit, bool& userIgnore, HashTable& dictWordTable, BST& myBST){
	cout << "\n" << inputWd << " On Line " << lineNum << " Was Not Found In Dictionary\n\n";
	cout << "A) Add the Word To Dictionary" << endl;
	cout <<	"I) Ignore Word, and Skip Future References" << endl;
	cout <<	"G) Go On To Next Word" << endl;
	cout <<	"S) Search For A Suggested Spelling" << endl;
	cout <<	"Q) Quit Spell Checking File" << endl << endl;
	//variable declaration:
	char userChoice;
	bool userGo = true;
	string SugWord;
	string waitString;
	//while the user has not input a correct value, continue running the switch statement
	while(userGo){
	//here, the user input determines one of the five decisions:
	cout << "Selection : "; 
	cin >> userChoice;
	switch(userChoice){
		//the add case:
		case 'A':
			cout << "\n choosing to Add the word -> " << inputWd << " to the Dictionary\n\n\n";
			//insert the word into the dictionary hash table before
			//inserting the word into the DICTIONARY TEXT.
			dictWordTable.insert(inputWd);
			dictInsert(inputWd);
			userGo = false; break;
		//the ignore case:
		case 'I':
			userIgnore = true;
			//update the BST with a new node.
			insertOrUpdate(myBST, inputWd, lineNum);
			cout << "\n choosing to Ignore -> " << inputWd << " for the rest of this check\n\n\n";
			userGo = false; break;
		//the Go case:
		case 'G':
			cout << "\n choosing to Move on from -> " << inputWd << "\n\n\n";
			//update the BST with a new node.
			insertOrUpdate(myBST, inputWd, lineNum);
			userGo = false; break;
		//the suggestion case:
		case 'S':
			cout << "\n\n Suggested Spelling choices: ";
			//update the BST with a new node.
			insertOrUpdate(myBST, inputWd, lineNum);
			//if the suggested word is not a word but rather a space, this signals that a dictionary word
			//was not found and that there is no dictionary suggestion.
			SugWord = suggestedSpell(inputWd, dictWordTable);
			if(SugWord == " "){
				cout << " There Are no suggestable words in the dictionary. " << endl;
			}else{
				cout << SugWord << endl;
			}
			cout << "\n      press Return to continue on to the next word. . .\n\n";
			//the user is prompted for a simple return input to continue after observing the current suggestion.
			getline(cin, waitString);
			cin.ignore();
			userGo = false; break;
		//the quit case:
		case 'Q':
			userQuit = true;
			userGo = false; break;
		default:
			cout << "Invalid user input\n\n\n"; break;			
	}
	}
}

/* The function responsible for inserting a new node into the BST
 * OR
 * updating the line number of a duplicate word in the BST.
*/
void insertOrUpdate(BST& myBST, string inputWd, int lineNum){
	//create a search node with values equal to the new input word and line number.
	BNode searchNode;
	searchNode.word = inputWd;
	searchNode.lineNum = lineNum;
	//set the result equal to a special result node.
	BNode result = myBST.find(searchNode);
	//if the find is blank ("ZZZ") then the word will simply be inserted,
	//otherwise the word will be removed and replaced with the new node entirely.
	if(result.word == "ZZZ"){
		myBST.insert(searchNode);	
	}else{
		myBST.remove(result);
		myBST.insert(searchNode);
	}
}
/* The function responsible for inserting the misspelled words into the notfound text file
 * in ascending order.
*/
void insertMisspelled(BST& myBST){
	//clear the file so that any previous uses are not interferring.
	ofstream nFclear("notfound.txt");
	cout << "";
	nFclear.close();
	//now input the BST values into the clean not found text.
	ofstream nFFile("notfound.txt", ios::app);
	BNode currNode;
	while(!myBST.isEmpty()){
		currNode = myBST.findMin();
		nFFile << currNode.word << " " << currNode.lineNum << endl;
		myBST.remove(currNode);
	}
	nFFile.close();
}

/* The function responsible for traversing a word and determining whether or not it matches a dictionary
 * HASH table words for each time it swaps two letters for ALL letters in the word.
*/
string suggestedSpell(string word, HashTable& dictWordTable){
	string swappedWord;
	//for the size of the word until two places before the end of the string array, loop.
	for(int i=0; i <= word.size()-2; i++){
		//calling the swappedWord function to swap the characters given.
		swappedWord = swap(word, word[i], word[i+1], i);
		//if it is a found word, stop the function and return a suggested word
		if(dictWordTable.find(swappedWord))
			return swappedWord;
	}
	//if nothing is found, return a particular space answer.
	return " ";
}
/* This minor function takes two characters addressed in the previous suggestedSpell function and swaps them 
 * before returning the newly swapped word.
*/
string swap(string word, char c1, char c2, int mov){
	string swappedWord = word;
	swappedWord[mov] = c2;
	swappedWord[mov+1] = c1;
	return swappedWord;
}
/* This insert function takes a given string word and appends it to the end of the dictionary.
 * THIS IS PERMANENT TO THAT FILE.
*/
void dictInsert(string inputWd){
	ofstream dictAdd("dict.txt", ios::app);
	if(dictAdd.is_open()){
		dictAdd << inputWd << endl;
	}else{
		cout << "\nThere was an Error opening the dictionary txt. \n";
	}
	dictAdd.close();
}
