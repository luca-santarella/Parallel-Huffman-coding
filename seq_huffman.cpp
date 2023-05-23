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
void print_queue(std::string_view name, Q q)
{
    // NB: q is passed by value because there is no way to traverse
    // priority_queue's content without erasing the queue.
    for (std::cout << name << ": \n"; !q.empty(); q.pop())
        std::cout << "key: " << q.top()->data << " freq: " << q.top()->freq << endl ;
    std::cout << '\n';
}

int main(int argc, char* argv[])
{
    //***READING FROM TXT FILE***
    string strFile;
    string str;
    long usecs;
    {utimer t0("reading file", &usecs);
        ifstream MyReadFile("txt_files/file_10K.txt");
        while(getline (MyReadFile, str))
        {
            strFile += str;
            strFile.push_back('\n');
        }
    }
    cout << "reading in " << usecs << endl;
    //cout << strFile << endl;
    //***COUNTING FREQUENCIES***
    std::map<char, int> freq;
    freq = countFreq(strFile);
    //printFreq(freq);

    //***INITIALIZE PRIORITY QUEUE AND BINARY TREE***
    std::priority_queue<treeNode*, vector<treeNode*>, node_comparison> prior_q; // Max priority queue
    std::map<char, int>::iterator it = freq.begin();
    while (it != freq.end())
    {
        struct treeNode *myNewNode;
        myNewNode = newNode(it->first, it->second);
        prior_q.push(myNewNode);
        ++it;
    }
    struct treeNode *myRoot = (struct treeNode*) malloc(sizeof(struct treeNode));
    struct tree *binTree = (struct tree*) malloc (sizeof(struct tree));
    binTree->size = 0;
    binTree->root = myRoot;

    //print_queue("priority_queue", prior_q);

    //*** BUILD TREE

    struct treeNode *firstNode = prior_q.top();
    prior_q.pop();
    struct treeNode *secondNode = prior_q.top();
    prior_q.pop();

    int sum = firstNode->freq + secondNode->freq;
    // $ special character to denote internal nodes with no char
    struct treeNode *internalNode = newNode('$', sum);
    internalNode->right = firstNode;
    internalNode->left = secondNode;

    cout << "key: " << internalNode->data << " freq: " << internalNode->freq << endl;


    cout << "key: " << firstNode->data << " freq: " << firstNode->freq << endl;
    cout << "key: " << secondNode->data << " freq: " << secondNode->freq << endl;
 

  return (0);
}
