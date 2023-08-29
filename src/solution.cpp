#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

struct BTree{
    int degree;
    Node *root;

    BTree(int degree): degree(degree){
        root = NULL;
    }

    void insertion(int key, int value){}
    void deletion(int key){}
};

struct Node{
    int *keys;
    Node **childs;
    int numChilds, numKeys;

    Node(int maxChilds): numChilds(0), numKeys(0){
        keys = new int[maxChilds-1];
        childs = new Node *[maxChilds];
    }
};


void parsing(string filename, vector<pair<int,int> >* data){
    ifstream file(filename);

    if(file.fail()){
        cout << "failed to load " + filename << '\n';
        return;
    }
    
    int lineNum = 0;

    while(!file.eof()){
        string line;
        getline(file, line);
        lineNum++;
        if(line == "") continue;

        // parsing 2 integers with delimiters
        char delimiters[2] = {'\t', ' '};
        int delimiter = -1;

        for(int i=0; i<2; i++){
            delimiter = line.find(delimiters[i]);
            if(delimiter != -1) break;
        }
        if(delimiter == -1){
            cout << "can't find any delimiters in line number " << lnum << '\n';
            continue;
        }
        data->emplace_back(stoi(line.substr(0,delimiter)), stoi(line.substr(delimiter+1)));
    }
    return;
}

int main(){
    int degree;

    cout << "set a degree of the tree: ";
    cin >> degree;

    BTree tree(degree);

    while(1){
        int command;

        cout << "input command(1: insertion 2: deletion 3: quit): ";
        cin >> command;

        if(command == 1){ // insertion
            string filename;

            cout << "file path for insertion: ";
            cin >> filename;

            vector<pair<int,int> > data;
            parsing(filename, &data);

            vector<pair<int,int> >::iterator itr;
            for(itr=data.begin(); itr!=data.end(); itr++){
                tree.insertion(itr->first, itr->second);
            }
        }
        else if(command == 2){ // deletion
            string filename;

            cout << "file path for deletion: ";
            cin >> filename;

            vector<pair<int,int> > data;
            parsing(filename, &data);

            vector<pair<int,int> >::iterator itr;
            for(itr=data.begin(); itr!=data.end(); itr++){
                tree.deletion(itr->first);
            }
        }
        else return 0;
    }
}