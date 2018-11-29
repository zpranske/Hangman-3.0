#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

    void loadDict(map<int,set<string>>& dict, ifstream& in);
template <typename T>
    void printDict(map<T,set<string>>& dict, ostream& out);
    void printDict(map<int,set<string>>& dict, int n, ostream& out);
    void getDictInfo(map<int,set<string>>& dict, int& min, int& max);
    void loadSubdict(map<int,set<string>>& src, set<string>& dest, int n);
    char getGuess(vector<char>& guesses, int& numGuesses);
    void print(set<string> s, ostream& out);
template <typename T>
    void print(vector<T> s, ostream& out);
    void printSpaces(string word, vector<int> guessLocations, ostream& out);
    void printspaces(string word);
    void updateDict(set<string>& subdict, vector<char>& guesses, char curGuess);
    bool isLastWord(set<string>& subdict);
    bool playRegularHangman(set<string>& subdict, vector<char>& guesses, vector<int>& guessLocations);
    bool done(set<string>& subdict, vector<int>& locations);

const int LIVES = 7;

int main() {
    ifstream in;
    in.open("dict.txt");
    if (!in) {
        cout << "Dictionary did not load. Quitting...\n";
        return 1;
    }

    map<int,set<string>> dict;
    loadDict(dict, in);

    int minSize = 0, maxSize = 0;
    getDictInfo(dict, minSize, maxSize);

    int wordLength = 0;
    bool valid = false;
    cout << "Welcome to Evil Hangman.\nPick a word length: \n";
    while (!valid) {
        cin >> wordLength;
        if (wordLength >= minSize && wordLength <= maxSize) valid = true;
        else {
            cout << "No words of that length exist. Please choose a length between ";
            cout << minSize << " and " << maxSize << ":\n";
        }
    }

    set<string> subdict1;
    loadSubdict(dict, subdict1, wordLength);
    //print(subdict1, cout);

    vector<char> guesses;
    int lives = LIVES;
    int numGuesses = 0;
    char currentGuess;
    vector<int> guessLocations;

    while (lives > 0) {
        while (!isLastWord(subdict1)) {
            currentGuess = getGuess(guesses, numGuesses);
            updateDict(subdict1, guesses,currentGuess);
            cout << lives << " lives remaining...\n";
            lives--;
            if (lives == 0) {
                cout << "You lose.";
                return 0;
            }
        }
        for (auto i = guesses.begin(); i != guesses.end(); i++) {
            cout << *i << " ";
        }

        while (!(done(subdict1, guessLocations))) {
            currentGuess = getGuess(guesses, numGuesses);
            guesses.push_back(currentGuess);
            if (playRegularHangman(subdict1, guesses, guessLocations)) lives--;
            if (lives == 0) {
                cout << "You lose.";
                return 0;
            }
            else cout << lives << " lives remaining...\n";
        }
        cout << "You win!";
        return 0;
    }
}

void loadDict(map<int,set<string>>& dict, ifstream& in){
    string temp;
    while(in >> temp){
        dict[temp.length()].insert(temp);
    }
}

template <typename T>
void printDict(map<T,set<string>>& dict, ostream& out) {
    for (auto i = dict.begin(); i != dict.end(); i++) {
        for ( auto j = i->second.begin(); j != i->second.end(); j++ ){
            out << *j << endl;
        }
    }
}

void printDict(map<int,set<string>>& dict, int n, ostream& out) {
    for (map<int,set<string>>::iterator i = dict.begin(); i != dict.end(); i++) {
        if (i->first == n) {
            for ( set<string>::iterator itr = i->second.begin(); itr != i->second.end(); ++itr ){
                out << *itr << endl;
            }
        }
    }
}

void getDictInfo(map<int,set<string>>& dict, int& min, int& max) {
    auto i = dict.begin();
    min = i->first;
    i = --dict.end();
    max = i->first;
}

void loadSubdict(map<int,set<string>>& src, set<string>& dest, int n) {
    for (auto i:src) {
        if (i.first == n) {
            for (auto j:i.second){
                dest.insert(j);
            }
        }
    }
}

char getGuess(vector<char>& guesses, int& numGuesses) {
    char c = '\0';
    cout << "Guess a letter: ";
    while (!(islower(c))) {
        cin >> c;
        if (isupper(c))
            c = tolower(c);
        if (!islower(c))
            cout << "Please enter a letter, not just some random character you moron\n";
    }
    guesses.push_back(c);
    numGuesses++;
    return c;
}

void print(set<string> s, ostream& out) {
    for (auto i = s.begin(); i != s.end(); i++) {
        out << *i << " ";
    }
}

template <typename T>
void print(vector<T> s, ostream& out) {
    for (auto i = s.begin(); i != s.end(); i++) {
        out << *i << " ";
    }
    out << endl;
}

void printSpaces(string word, vector<int> guessLocations, ostream& out) {
    out << endl;
    for (int i = 0; i < word.length(); i++) {
        bool isIn = false;
        for (auto j:guessLocations) {
            if (i == j)  isIn = true;
        }
        isIn? out << word[i] << " " : out << "_ ";
    }
    out << endl;
}

void printspaces(string word) {
    for (int i = 0; i<word.length(); i++) {
        cout << "_ ";
    }
}

void updateDict(set<string>& subdict, vector<char>& guesses, char curGuess) {
    string lastword;
    for (int n=0; n<2; n++) {
        for (auto i:subdict) {
            vector<int> guessLocations;
            string word = i;
            for (int k = 0; k < word.length(); k++) {
                if (word[k] == curGuess) guessLocations.push_back(k);
            }
            //If there are any words associated with empty vectors, we can make those
            //our new list. Otherwise we have to revert to normal hangman as all words
            //will have been removed from subdict. Use last word encountered for normal hangman

            //cout << word << " ";
            //print(guessLocations, cout);

            bool evil = guessLocations.empty();
            if (!evil) {
                subdict.erase(i);
                lastword = i;
            }
        }
    }

    vector<int> guessLocations;
    for (int k = 0; k < lastword.length(); k++) {
        if (lastword[k] == curGuess) guessLocations.push_back(k);
    }

    if(subdict.empty()) {
        subdict.insert(lastword);
        //cout << "No more evil words: word is " << lastword << endl;
        printSpaces(lastword, guessLocations, cout);
    }
    else {
        //cout << "Evil words not containing " << curGuess << ":\n";
        for (auto i:subdict) {
            string word = i;
            //cout << word << " ";
        }
        cout << endl;
        printspaces(lastword);
    }
    cout << endl;
}

bool isLastWord(set<string>& subdict) {
    return subdict.size() == 1;
}

bool playRegularHangman(set<string>& subdict, vector<char>& guesses, vector<int>& guessLocations) {
    auto i = subdict.begin();
    string word = *i;
    bool badguess = true;
    for (auto j = guesses.begin(); j != guesses.end(); ++j) {
        for (int k = 0; k < word.length(); k++) {
            if (word[k] == *j) {
                bool isIn = false;
                for (auto m:guessLocations) {
                    if (m == k) isIn = true;
                }
                if (!isIn) {
                    guessLocations.push_back(k);
                    badguess = false;
                }
            }
        }
    }
    print(guessLocations, cout);
    printSpaces(word, guessLocations, cout);
    return badguess;
}

bool done(set<string>& subdict, vector<int>& locations) {
    auto i = subdict.begin();
    string word = *i;
    if (locations.size() == word.length()) return true;
    return false;
}