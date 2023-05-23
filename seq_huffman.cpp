/*Luca Santarella 22/06/23

HUFFMAN CODING (SEQUENTIAL):

*/
// C++ program for Huffman Coding
#include <queue>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <fstream>
#include "utimer.hpp"
#include <chrono>
using namespace std;

#define MAX_TREE_HT 1000

struct treeNode
{
    char data;

    int freq;

    //left and right children
    struct treeNode *left, *right;
};

struct tree
{
    int size;

    struct treeNode *root;

};

struct node_comparison 
{
   bool operator()( const treeNode* a, const treeNode* b ) const 
   {
    return a->freq > b->freq;
   }
};

struct treeNode* newNode(char data, int freq)
{
    struct treeNode* myNewNode = (struct treeNode*) malloc(
        sizeof(struct treeNode));
    myNewNode->left = NULL;
    myNewNode->right = NULL;
    myNewNode->data = data;
    myNewNode->freq = freq;

    return myNewNode;
}

std::map<char, int> countFreq(string str)
{
    // size of the string 'str'
    int n = str.size();

    //map used to store the couple <str, #occ>
    std::map<char, int> mapStrOcc;

    // accumulate frequency of each character in 'str'
    for (int i = 0; i < n; i++)
        mapStrOcc[str[i]]++;

    return mapStrOcc;
}

void printFreq(std::map<char, int> freq)
{
    // Get an iterator pointing to the first element in the map
    std::map<char, int>::iterator it = freq.begin();
    while (it != freq.end())
    {
        std::cout << "key: " << it->first << ", freq: " << it->second << std::endl;
        ++it;
    }
}

template<typename Q>
void initQueue(Q &prior_q, std::map<char, int> freq, tree* &binTree)
{
    std::map<char, int>::iterator it = freq.begin();
    while (it != freq.end())
    {
        struct treeNode *myNewNode;
        myNewNode = newNode(it->first, it->second);
        prior_q.push(myNewNode);
        binTree->size++;
        ++it;
    }
}

template<typename Q>
void printQueue(std::string_view name, Q q)
{
    for (std::cout << name << ": \n"; !q.empty(); q.pop())
        std::cout << "key: " << q.top()->data << " freq: " << q.top()->freq << endl ;
    std::cout << '\n';
}

// function to check if this node is leaf
int isLeaf(struct treeNode* node){

    return !(node->left) && !(node->right);
}

//print an array of size n
void printArr(int arr[], int n){
    int i;
    for (i = 0; i < n; ++i)
        cout << arr[i];

    cout << "\n";
}

void printCodes(struct treeNode* root, int arr[],
                int top){

    //assign 0 to left edge and recur
    if (root->left) {

        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    //assign 1 to right edge and recur
    if (root->right) {

        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (isLeaf(root)) {

        cout << root->data << ": ";
        printArr(arr, top);
    }
}

template<typename Q>
void buildHufTree(Q &prior_q, tree* &binTree)
{
    while(prior_q.size() != 1)
    {
        //take first node with the lowest freq
        struct treeNode *firstNode = prior_q.top();

        //remove it from the priority queue
        prior_q.pop();

        //take second node and do the same
        struct treeNode *secondNode = prior_q.top();
        prior_q.pop();

        //compute the sum between the two nodes
        int sum = firstNode->freq + secondNode->freq;

        //create new internal node 
        // $ special character to denote internal nodes with no char
        struct treeNode *internalNode = newNode('$', sum);

        //set children of new internal node
        internalNode->left = firstNode;
        internalNode->right = secondNode;

        //push internal node to priority queue
        prior_q.push(internalNode);

        //increase size of binary tree because of new internal node
        binTree->size++;
    }
}

int main(int argc, char* argv[])
{
    //***READING FROM TXT FILE***
    string strFile;
    string str;
    long usecs;
    {utimer t0("reading file", &usecs);
        ifstream MyReadFile("txt_files/example.txt");
        while(getline (MyReadFile, str))
        {
            strFile += str;
            //strFile.push_back('\n');
        }
    }
    cout << "reading in " << usecs << endl;
    //cout << strFile << endl;

    //***COUNTING FREQUENCIES***
    std::map<char, int> freq;
    freq = countFreq(strFile);
    printFreq(freq);

    //***INITIALIZE PRIORITY QUEUE AND BINARY TREE***
    // Max priority to lowest freq node
    std::priority_queue<treeNode*, vector<treeNode*>, node_comparison> prior_q; 

    //representation of the binary tree
    struct tree *binTree = (struct tree*) malloc (sizeof(struct tree));
    binTree->size = 0;

    initQueue(prior_q, freq, binTree);


    //printQueue("priority_queue", prior_q);

    //*** BUILD HUFFMAN TREE
    //compute until there are just two nodes to be processed
    buildHufTree(prior_q, binTree);

    //set root 
    struct treeNode* myRoot = prior_q.top();
    binTree->root = myRoot;

    //print Huffman codes using
    int arr[MAX_TREE_HT], top = 0;
    printCodes(myRoot, arr, top);
    
    //printQueue("priority_queue", prior_q);
  return (0);
}
