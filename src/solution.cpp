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
            root = newRoot;
        }
        root->insertion(key, value);
    }

    pair<Node*,int> search(int key){
        if(root == NULL) return {NULL,-1};
        return root->search(key);
    }

    void traversal(vector<pair<int,int> >* data){
        if(root == NULL) return;
        root->traversal(data);
    }

    void deletion(int key){
        if(root == NULL) return;
            // key 검색
            // key 있음
                // leaf 노드일 때
                    // 그냥 삭제
                    // key 개수가 0이면 노드 삭제
                // leaf 노드가 아닐 때 
                    // 왼쪽 자식의 키가 남을 때
                        // 왼쪽 서브트리의 max 값 검색 후 돌아옴
                        // max 값을 key가 있던 자리에 덮어쓰기
                        // 왼쪽 서브트리에 대해 max 값 삭제 함수 호출
                    // 오른쪽 자식의 키가 남을 때
                        // 위와 동일하지만 min 값 검색
                    // 둘 다 안남을 때
                        // 왼쪽자식 + key + 오른쪽 자식 merge 후 키 한 칸씩 당김. 
                        // 키 개수가 0이면 루트를 왼쪽 자식으로 변경하고 현재 루트 삭제
                        // 왼쪽 서브트리 삭제 함수 호출
            // key 없음
                // 왼쪽 자식이 미니멈 노드인 경우
                    // 왼쪽 자식의 인접 노드들이 키가 남는 경우
                        // 현재 노드의 key를 왼쪽 자식의 제일 오른쪽에 삽입
                        // 인접 노드의 키(max or min)를 현재 노드의 key에 덮어쓰기
                    // 안 남는 경우
                        // 왼쪽 자식+현재 키+오른쪽 자식 merge, 오른쪽 자식 없애고 현재 노드 key 한 칸씩 당기기
                        // key 개수가 0이면 루트를 왼쪽 자식으로 변경하고 현재 루트 삭제
                        // 이미 현재 노드의 부모에서 현재 노드가 미니멈 노드가 아니도록 해놨음

    }            
};


struct Node{
    int *keys, *values, numKeys, maxChilds;
    Node **childs;
    bool leaf;

    Node(int maxChilds, bool leaf): maxChilds(maxChilds), leaf(leaf){
        keys = new int[maxChilds-1];
        values = new int[maxChilds-1];
        childs = new Node *[maxChilds];
        numKeys = 0;
    }

    pair<Node*,int> search(int key);
    void insertion(int key, int value);
    void split(int childIdx);
    void traversal(vector<pair<int,int> >* data);
    void deletion(int key);
};


/*
    return the node and index of the key
*/
pair<Node*,int> Node::search(int key){
    int idx = lower_bound(keys, keys+numKeys, key) - keys;
    if(keys[idx] == key) return {this,idx};
    else if(leaf == true) return {NULL,-1};
    else return childs[idx]->search(key);
}

void Node::insertion(int key, int value){
    int idx = lower_bound(keys, keys+numKeys, key) - keys;
    if(idx < numKeys && keys[idx] == key) return;

    if(leaf){ // insert if the node is leaf
        for(int i=numKeys; i>idx; i--){ // shifting
            keys[i] = keys[i-1];
            values[i] = values[i-1];
        }
        keys[idx] = key;
        values[idx] = value;
        numKeys++; 
    }
    else{ // non-leaf node
        if(childs[idx]->numKeys == maxChilds-1){
            split(idx); // when the child node is full, split.

            if(keys[idx] < key) childs[idx+1]->insertion(key,value);
            else childs[idx]->insertion(key,value);
        }
        else childs[idx]->insertion(key,value);
    }
}

/*
    when childs[childIdx] is full, split into two.  
*/
void Node::split(int childIdx){
    // 노드 하나 더 만들어서 full인 자식의 중앙값 기준 오른쪽 key-value 옮기기
    Node *rightNode = new Node(maxChilds, childs[childIdx]->leaf);
    int mid = (maxChilds-1)/2;

    for(int i=mid+1; i<maxChilds-1; i++){
        rightNode->keys[i-mid-1] = childs[childIdx]->keys[i];
        rightNode->values[i-mid-1] = childs[childIdx]->values[i];
    }

    rightNode->numKeys = (maxChilds-2)-(mid+1)+1;
    childs[childIdx]->numKeys = mid;

    // leaf 노드가 아니라면 자식 노드까지 옮기기
    if(rightNode->leaf == false){
        for(int i=mid+1; i<maxChilds; i++){
            rightNode->childs[i-mid-1] = childs[childIdx]->childs[i];
        }
    }
    
    // 오른쪽에 있는 key들 한 칸씩 미루고 중앙값 가져오기
    childs[numKeys+1] = childs[numKeys];
    for(int i=numKeys; i>childIdx; i--){
        keys[i] = keys[i-1];
        values[i] = values[i-1];
        childs[i] = childs[i-1];
    }
    keys[childIdx] = childs[childIdx]->keys[mid];
    values[childIdx] = childs[childIdx]->values[mid];
    numKeys++;

    // 중앙값의 오른쪽 자식으로 새로 만든 rightNode 연결
    childs[childIdx+1] = rightNode;
}

/*
    inorder traversal
    key가 오름차순이 되도록 순회하여 key-value 쌍을 vector에 저장
*/
void Node::traversal(vector<pair<int,int> >* data){
    for(int i=0; i<numKeys; i++){
        if(!leaf) childs[i]->traversal(data);
        data->emplace_back(keys[i],values[i]);
    }
    if(numKeys > 0 && !leaf) childs[numKeys]->traversal(data);
}

void Node::deletion(int key){
    // key 검색
    // key 있음
        // leaf 노드일 때
            // 그냥 삭제
        // leaf 노드가 아닐 때 
            // 왼쪽 자식의 키가 남을 때
                // 왼쪽 서브트리의 max 값 검색 후 돌아옴
                // max 값을 key가 있던 자리에 덮어쓰기
                // 왼쪽 서브트리에 대해 max 값 삭제 함수 호출
            // 오른쪽 자식의 키가 남을 때
                // 위와 동일하지만 min 값 검색
            // 둘 다 안남을 때
                // 왼쪽자식 + key + 오른쪽 자식 merge 후 키 한 칸씩 당김. 왼쪽 서브트리 삭제 함수 호출
    // key 없음
        // 왼쪽 자식이 미니멈 노드인 경우
            // 왼쪽 자식의 인접 노드들이 키가 남는 경우
                // 현재 노드의 key를 왼쪽 자식의 제일 오른쪽에 삽입
                // 인접 노드의 키(max or min)를 현재 노드의 key에 덮어쓰기
            // 안 남는 경우
                // 왼쪽 자식+현재 키+오른쪽 자식 merge, 오른쪽 자식 없애고 현재 노드 key 한 칸씩 당기기
                // 이미 현재 노드의 부모에서 현재 노드가 미니멈 노드가 아니도록 해놨음
}


/*
    csv 파일을 읽어 key-value 정보를 data vector에 저장
*/
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
    file.close();
    return;
}

/*
    2개의 key-value vector를 비교
    같으면 true, 다르면 false를 반환
*/
bool compare(vector<pair<int,int> > *data, vector<pair<int,int> > *res){
    if(data->size() != res->size()) return false;

    vector<pair<int,int> >::iterator dataItr, resItr;
    dataItr = data->begin(), resItr = res->begin();

    for(; dataItr!=data->end(); dataItr++, resItr++){
        if(dataItr->first != resItr->first) return false;
        if(dataItr->second != resItr->second) return false;
    }
    return true;
}

void insertion(BTree* tree){
    string filename;

    cout << "file path for insertion: ";
    cin >> filename;

    vector<pair<int,int> > data;
    parsing(filename, &data);

    vector<pair<int,int> >::iterator itr;
    for(itr=data.begin(); itr!=data.end(); itr++){
        tree->insertion(itr->first, itr->second);
    }
    cout << "insertion completed\n";

    // search
    ofstream file("searched.csv");

    for(itr=data.begin(); itr!=data.end(); itr++){
        auto ans = tree->search(itr->first);
        if(ans.first == NULL) 
            file << itr->first << " " << "N/A\n";
        else{
            int val = ans.first->values[ans.second];
            file << itr->first << " " << val << '\n';
        }
    }

    file.close();
    cout << "saved the result file named 'searched.csv'\n";

    // compare
    vector<pair<int,int> > results;
    parsing("searched.csv", &results);

    if(compare(&data, &results)){
        cout << filename + " equals searched.csv\n";
    }
    else{
        cout << filename + " doesn't equal searched.csv\n";
    }
}

void deletion(BTree* tree){
    string filename;

    cout << "file path for deletion: ";
    cin >> filename;

    vector<pair<int,int> > data;
    parsing(filename, &data);

    vector<pair<int,int> >::iterator itr;
    for(itr=data.begin(); itr!=data.end(); itr++){
        tree->deletion(itr->first);
    }
}

int main(int argc, char *argv[]){
    int degree = argc > 1 ? *argv[1] : -1;

    while(degree < 2){
        cout << "input the degree of a tree: ";
        cin >> degree;
    }

    BTree tree(degree);

    while(1){
        int command;
        cout << "input command(1: insertion 2: deletion 3: quit): ";
        cin >> command;

        if(command == 1){ // insertion
            insertion(&tree);
        }
        else if(command == 2){ // deletion
            deletion(&tree);
        }
        else return 0;
    }
}