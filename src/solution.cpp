#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;


struct BTree{
    int degree;
    Node *root;

    BTree(int degree): degree(degree){
        root = NULL;
    }

    void insertion(int key, int value){
        if(root == NULL){
            root = new Node(degree, true);
        }
        else if(root->numKeys == degree-1){
            Node *newRoot = new Node(degree, false);
            newRoot->childs[0] = root;
            newRoot->split(0);
        }
        root->insertion(key, value);
    }

    pair<Node*,int> search(int key){
        if(root == NULL) return {NULL,-1};
        return root->search(key);
    }

    void deletion(int key){}
};


struct Node{
    int *keys, *values, numKeys, maxChilds;
    Node **childs;
    bool leaf;

    Node(int maxChilds, bool leaf): maxChilds(maxChilds), numKeys(0), leaf(leaf){
        keys = new int[maxChilds-1];
        values = new int[maxChilds-1];
        childs = new Node *[maxChilds];
    }

    pair<Node*,int> search(int key){
        int idx = lower_bound(keys, keys+numKeys, key) - keys;
        if(keys[idx] == key) return {this,idx};
        else if(leaf == true) return {NULL,-1};
        else return childs[idx]->search(key);
    }

    void insertion(int key, int value){
        if(leaf){
            keys[numKeys] = key;
            values[numKeys++] = value;   
        }
        else{
            int idx = lower_bound(keys, keys+numKeys, key) - keys;
            if(keys[idx] == key) return;

            if(childs[idx]->numKeys == maxChilds-1){
                split(idx);
                if(keys[idx] < key) childs[idx+1]->insertion(key,value);
                else childs[idx]->insertion(key,value);
            }
            else childs[idx]->insertion(key,value);
        }
    }

    void split(int childIdx){
        // 노드 하나 더 만들어서 중앙값 오른쪽 값들 가져오기
        Node *rightNode = new Node(maxChilds, childs[childIdx]->leaf);
        int mid = (maxChilds-1)/2;

        childs[childIdx]->numKeys = mid;
        rightNode->numKeys = (maxChilds-2)-(mid+1)+1;

        for(int i=mid+1; i<maxChilds-1; i++){
            rightNode->keys[i-mid-1] = childs[childIdx]->keys[i];
            rightNode->values[i-mid-1] = childs[childIdx]->values[i];
            if(!rightNode->leaf){
                rightNode->childs[i-mid-1] = childs[childIdx]->childs[i];
            }
        }
        
        // key들 한 칸씩 미루고 왼쪽 자식의 중앙값 가져오기
        childs[numKeys+1] = childs[numKeys];
        for(int i=numKeys; i>childIdx; i--){
            keys[i] = keys[i-1];
            values[i] = values[i-1];
            childs[i] = childs[i-1];
        }
        childs[childIdx+1] = rightNode;

        keys[childIdx] = childs[childIdx]->keys[mid];
        values[childIdx] = childs[childIdx]->values[mid];
        numKeys++;
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
            cout << "can't find any delimiters in line number " << lineNum << '\n';
            continue;
        }
        data->emplace_back(stoi(line.substr(0,delimiter)), stoi(line.substr(delimiter+1)));
    }
    return;
}


int main(int argc, char *argv[]){
    BTree tree(*argv[1]);

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