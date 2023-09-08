#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

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
    void merge(int idx);
    void makeNonMinimum(int idx);

    pair<int,int> findmin(){
        if(leaf) return {keys[0], values[0]};
        return childs[0]->findmin();
    }
    pair<int,int> findmax(){
        if(leaf) return {keys[numKeys-1], values[numKeys-1]};
        return childs[numKeys]->findmax();
    }
};


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
        root->deletion(key);
        if(root->numKeys == 0){
            Node *tmp = root;
            if(root->leaf) root = NULL;
            else root = root->childs[0];
            delete tmp;
        }
    }            
};

/*
    return the node and index of the key
*/
pair<Node*,int> Node::search(int key){
    int idx = lower_bound(keys, keys+numKeys, key) - keys;
    if(idx < numKeys && keys[idx] == key) return {this,idx};
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

/*
    해당 노드를 루트로 하는 서브트리에서 key 삭제
    자식 노드로 이동할 때 makeNonMinimum 함수를 호출하여
    key가 존재할 수도 있는 자식 노드가 minimum node가 아님을 보장
*/
void Node::deletion(int key){
    int minKeys = maxChilds/2-1; // 최소 키 개수
    int idx = lower_bound(keys, keys+numKeys, key) - keys; // key 검색
    
    if(idx < numKeys && keys[idx] == key){ // key가 현재 노드에 있음
        if(leaf){ // leaf 노드라면 그냥 key 삭제
            for(int i=idx; i<numKeys-1; i++){
                keys[i] = keys[i+1];
                values[i] = values[i+1];
            }
            numKeys--;
        }
        else{ // leaf 노드가 아닐 때 
            if(childs[idx]->numKeys > minKeys){ // 왼쪽 자식이 minimum node가 아니면
                pair<int,int> leftMax = childs[idx]->findmax(); // 왼쪽 서브트리의 max 값 검색 후 돌아옴
                keys[idx] = leftMax.first; // max 값을 key가 있던 자리에 덮어쓰기
                values[idx] = leftMax.second;
                childs[idx]->deletion(leftMax.first); // 왼쪽 서브트리에 대해 max값 삭제 함수 호출
            }
            else if(childs[idx+1]->numKeys > minKeys){ // 오른쪽 자식이 minimum node가 아니면
                pair<int,int> rightMin = childs[idx+1]->findmin(); // 위와 동일하지만 min 값 검색
                keys[idx] = rightMin.first;
                values[idx] = rightMin.second;
                childs[idx+1]->deletion(rightMin.first);
            }
            else{ // 왼쪽 오른쪽 자식이 다 minimum node일 때
                merge(idx); // key와 오른쪽 자식을 왼쪽 자식에 merge (즉, key를 아래로 내림)
                childs[idx]->deletion(key);
            }
        }
    }
    else{ // key가 현재 노드에 없음
        if(leaf) return;
        if(childs[idx]->numKeys == minKeys){ // 왼쪽 자식(childs[idx])이 minimum node인 경우
            makeNonMinimum(idx);
            idx = min(idx, numKeys);
        }
        childs[idx]->deletion(key);
    }
}


/*
    childs[idx]가 minimum node가 아니도록 만드는 함수
    인접한 형제 노드에서 key를 가져오거나 merge를 실행
*/
void Node::makeNonMinimum(int idx){
    int minKeys = maxChilds/2-1; // 최소 키 개수

    if(idx > 0 && childs[idx-1]->numKeys > minKeys){ // childs[idx-1]이 키가 남는 경우
        // childs[idx]의 key를 오른쪽으로 한 칸씩 shifting
        for(int i=minKeys; i>0; i--){
            childs[idx]->keys[i] = childs[idx]->keys[i-1];
            childs[idx]->values[i] = childs[idx]->values[i-1];
        }
        if(childs[idx]->leaf == false){
            for(int i=minKeys+1; i>0; i--){
                childs[idx]->childs[i] = childs[idx]->childs[i-1];
            }
        }
        // keys[idx-1]을 childs[idx]의 가장 왼쪽으로 내리기
        childs[idx]->keys[0] = keys[idx-1];
        childs[idx]->values[0] = values[idx-1];
        childs[idx]->numKeys++;

        // childs[idx-1]의 가장 오른쪽 키를 keys[idx-1]로 올리기
        int sibling_key_cnt = childs[idx-1]->numKeys;
        keys[idx-1] = childs[idx-1]->keys[sibling_key_cnt-1];
        values[idx-1] = childs[idx-1]->values[sibling_key_cnt-1];
        childs[idx-1]->numKeys--;

        // childs[idx-1]의 가장 오른쪽 자식을 childs[idx]의 가장 왼쪽 자식으로 옮기기
        if(childs[idx]->leaf == false)
            childs[idx]->childs[0] = childs[idx-1]->childs[sibling_key_cnt];
    }
    else if(idx < numKeys && childs[idx+1]->numKeys > minKeys){ // childs[idx+1]이 키가 남는 경우
        // keys[idx]를 childs[idx]의 가장 오른쪽으로 내리기
        childs[idx]->keys[minKeys] = keys[idx]; 
        childs[idx]->values[minKeys] = values[idx];
        childs[idx]->numKeys++;

        // childs[idx+1]의 가장 왼쪽 키를 keys[idx]로 올리기
        keys[idx] = childs[idx+1]->keys[0];
        values[idx] = childs[idx+1]->values[0];
        childs[idx+1]->numKeys--;

        // childs[idx+1]의 key를 왼쪽으로 한 칸씩 shifting
        int sibling_key_cnt = childs[idx+1]->numKeys;
        for(int i=0; i<sibling_key_cnt; i++){
            childs[idx+1]->keys[i] = childs[idx+1]->keys[i+1];
            childs[idx+1]->values[i] = childs[idx+1]->values[i+1];
        }
        if(childs[idx]->leaf == false){
            // childs[idx+1]의 가장 왼쪽 자식을 childs[idx]의 가장 오른쪽 자식으로 옮기기
            childs[idx]->childs[minKeys+1] = childs[idx+1]->childs[0];
            for(int i=0; i<=sibling_key_cnt; i++){
                childs[idx+1]->childs[i] = childs[idx+1]->childs[i+1];
            }
        }
    }
    else{ // 안 남는 경우
        if(idx == numKeys) idx--; // 가장 오른쪽 자식인 경우 
        merge(idx);
    }
}

/*
    keys[idx]와 오른쪽 자식을 왼쪽 자식에 순서대로 merge하여 full node로 만드는 함수
*/
void Node::merge(int idx){
    int minkey = maxChilds/2 - 1;

    // merge
    childs[idx]->keys[minkey] = keys[idx];
    childs[idx]->values[minkey] = values[idx];

    for(int i=0; i<minkey; i++){
        childs[idx]->keys[minkey+1+i] = childs[idx+1]->keys[i];
        childs[idx]->values[minkey+1+i] = childs[idx+1]->values[i];
    }
    childs[idx]->numKeys = 2*minkey + 1;

    if(childs[idx]->leaf == false){
        for(int i=0; i<minkey+1; i++){
            childs[idx]->childs[minkey+1+i] = childs[idx+1]->childs[i];
        }
    }

    delete childs[idx+1]; // 오른쪽 자식 삭제

    // key shifting
    for(int i=idx; i<numKeys-1; i++){
        keys[i] = keys[i+1];
        values[i] = values[i+1];
    }
    for(int i=idx+1; i<numKeys; i++){
        childs[i] = childs[i+1];
    }
    numKeys--;
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
        string key = line.substr(0, delimiter);
        string value = line.substr(delimiter+1);
        if(value[0] >= '0' && value[0] <= '9') 
            data->emplace_back(stoi(key), stoi(value));
        else
            data->emplace_back(stoi(key), NULL);
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

void insertion(BTree* tree, vector<pair<int,int> > *data){
    string filename;

    cout << "> file path for insertion: ";
    cin >> filename;

    parsing(filename, data);

    // insertion
    vector<pair<int,int> >::iterator itr;
    for(itr=data->begin(); itr!=data->end(); itr++){
        tree->insertion(itr->first, itr->second);
    }
    cout << "insertion completed\n";

    // search
    ofstream file("searched.csv");

    for(itr=data->begin(); itr!=data->end(); itr++){
        auto ans = tree->search(itr->first);
        if(ans.first == NULL) 
            file << itr->first << " " << "N/A\n";
        else{
            int val = ans.first->values[ans.second];
            file << itr->first << " " << val << '\n';
        }
    }

    file.close();
    cout << "saved the result file at 'searched.csv'\n";

    // compare
    vector<pair<int,int> > results;
    parsing("searched.csv", &results);

    if(compare(data, &results))
        cout << filename + " equals searched.csv\n";
    else
        cout << filename + " doesn't equal searched.csv\n";
}

void deletion(BTree* tree, vector<pair<int,int> > *input){
    string filename;

    cout << "> file path for deletion: ";
    cin >> filename;

    vector<pair<int,int> > data;
    parsing(filename, &data);

    // deletion
    vector<pair<int,int> >::iterator itr;
    for(itr=data.begin(); itr!=data.end(); itr++){
        tree->deletion(itr->first);
        if(tree->search(itr->first).first != NULL){
            cout << "failed to delete " << itr->first << '\n';
        }
    }
    cout << "deletion completed\n";

    // search
    ofstream file("delete_results.csv");
    for(itr=input->begin(); itr!=input->end(); itr++){
        auto ans = tree->search(itr->first);
        if(ans.first == NULL) 
            file << itr->first << " " << "N/A\n";
        else{
            int val = ans.first->values[ans.second];
            file << itr->first << " " << val << '\n';
        }
    }
    file.close();
    cout << "saved the result file at 'delete_results.csv'\n";

    // compare
    cout << "> file path for comparison: ";
    cin >> filename;

    vector<pair<int,int> > compareData, results;
    parsing(filename, &compareData);
    parsing("delete_results.csv", &results);

    if(compare(&compareData, &results))
        cout << filename + " equals delete_results.csv\n";
    else
        cout << filename + " doesn't equal delete_results.csv\n";
}

int main(int argc, char *argv[]){
    int degree = argc > 1 ? *argv[1] : -1;
    while(degree < 2){
        cout << "> input the degree of a tree: ";
        cin >> degree;
    }

    BTree tree(degree);
    vector<pair<int,int> > data;

    while(1){
        int command;
        cout << "> input command(1: insertion 2: deletion 3: quit): ";
        cin >> command;

        if(command == 1){ // insertion
            insertion(&tree, &data);
        }
        else if(command == 2){ // deletion
            deletion(&tree, &data);
        }
        else return 0;
    }
}