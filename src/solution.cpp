#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

vector<pair<int,int> > parsing(string filename){
    vector<pair<int,int> > data;
    ifstream file(filename);

    if(file.fail()){
        cout << "failed to load " + filename << '\n';
        return data;
    }
    
    int lnum = 0;
    while(!file.eof()){
        string line;
        getline(file, line);
        lnum++;
        if(line == "") break;

        // parsing 2 integers with delimiters
        char delimiters[2] = {'\t', ' '};
        int delimiter = -1;
        for(int i=0; i<2; i++){
            delimiter = line.find(delimiters[i]);
            if(delimiter != -1) break;
        }
        if(delimiter == -1){
            cout << "can't find any delimiter in line " << lnum << '\n';
            continue;
        }
        data.emplace_back(stoi(line.substr(0,delimiter)), stoi(line.substr(delimiter+1)));
    }
    return data;
}

void insertion(vector<pair<int,int> > data){}
void deletion(vector<pair<int,int> > data){}

int main(){
    while(1){
        int command;
        cout << "input command(1: insertion 2: deletion 3:quit): ";
        cin >> command;

        if(command == 1){
            string filename;
            cout << "file path for insertion: ";
            cin >> filename;
            insertion(parsing(filename));
        }
        else if(command == 2){
            string filename;
            cout << "file path for deletion: ";
            cin >> filename;
            deletion(parsing(filename));
        }
        else return 0;
    }
}