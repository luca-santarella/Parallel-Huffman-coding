/*Luca Santarella 22/06/23

HUFFMAN CODING (THREAD PARALLEL):

*/
// C++ parallel program for Huffman Coding
#include <queue>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <chrono>
#include <bitset>
#include <thread>
#include <mutex>
#include <algorithm>
#include "utimer.hpp"
using namespace std;

#define MAX_TREE_HT 1000
#define SIZE 128 //# of possible chars in ASCII

int printFlag = 0;
std::mutex countLock;
std::mutex hufLock;
std::mutex encASCIILock;
//struct representing tree node
struct treeNode
{
    char data;

    int freq;

    //left and right children
    struct treeNode *left, *right;
};

//struct representing whole Huffman tree
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
    struct treeNode* myNewNode = (struct treeNode*) malloc(sizeof(struct treeNode));
    myNewNode->left = nullptr;
    myNewNode->right = nullptr;
    myNewNode->data = data;
    myNewNode->freq = freq;

    return myNewNode;
}

char decToASCII(int decimalValue) {
    return static_cast<char>(decimalValue);
}

int ASCIIToDec(char c) {
    return static_cast<int>(c);
}

void countFreq(int start, int stop, std::string &str, std::vector<int> &freqs){
    std::vector<int> partialFreqs(SIZE, 0);
    for (int i = start; i < stop; i++){
        int pos = ASCIIToDec(str[i]);
        partialFreqs[pos]++;
    }

    countLock.lock();
    for(int i=0; i<SIZE;i++)
        freqs[i] += partialFreqs[i];
    countLock.unlock();
}

std::vector<int> mapCountFreq(int nw, std::string str)
{
    // size of the string 'str'
    int n = str.size();

    //vector used to store # occurrences of the 128 possible characters
    std::vector<int> freqs(SIZE,0);

    //vector used to store tids of threads
    std::vector<std::thread> tids;
    
    int delta = n / nw; //chunk size
    int start, stop;
    
    for(int i=0; i<nw; i++)
    {   
        start = i*delta;
        //check if last chunk to be distributed
        if(i==nw-1) 
          stop = n;
        else
          stop = i*delta + delta;

        tids.push_back(std::thread(countFreq, start, stop, std::ref(str), std::ref(freqs)));

    }
    for(std::thread& t: tids)  // await thread termination
    t.join();


    return freqs;
}


void printFreq(std::vector<int> freqs)
{
    for(int i=0; i<SIZE; i++){

        //there has been at least an occurrence
        if(freqs[i] != 0)
            cout << "key: " <<decToASCII(i) << "  freq: " << freqs[i] << endl;
    }
}

void printMap(std::unordered_map<char, std::string> map)
{
    // Get an iterator pointing to the first element in the map
    std::unordered_map<char, std::string>::iterator it = map.begin();
    while (it != map.end())
    {
        std::cout << "key: " << it->first << ", code: " << it->second << std::endl;
        ++it;
    }
}

template<typename Q>
void initQueue(Q &prior_q, std::vector<int> freqs, tree* &hufTree)
{
    for(int i=0; i < SIZE; i++)
    {
        if(freqs[i] != 0)
        {
            struct treeNode *myNewNode;
            myNewNode = newNode(decToASCII(i), freqs[i]);
            prior_q.push(myNewNode);
            hufTree->size++;            
        }

    }
}

template<typename Q>
void printQueue(std::string_view name, Q q)
{
    for (std::cout << name << ": \n"; !q.empty(); q.pop())
        std::cout << "key: " << q.top()->data << " freq: " << q.top()->freq << std::endl ;
    std::cout << '\n';
}

// function to check if this node is leaf
int isLeaf(struct treeNode* node)
{

    return !(node->left) && !(node->right);
}

//print an array of size n
void printArr(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        std::cout << arr[i];

    std::cout << "\n";
}

void setCode(char data, int arr[], int n, std::unordered_map<char, std::string> &codes)
{
    std::string code;
    for (int i=0; i < n; i++)
    {
        code += to_string(arr[i]);
    }
    codes[data] = code;
}


void traverseTree(struct treeNode* root, int arr[], int top, std::unordered_map<char, std::string> &codes)
{
    //assign 0 to left edge and recur
    if (root->left) {

        arr[top] = 0;
        traverseTree(root->left, arr, top + 1, codes);
    }

    //assign 1 to right edge and recur
    if (root->right) {

        arr[top] = 1;
        traverseTree(root->right, arr, top + 1, codes);
    }

    if (isLeaf(root)) {

        setCode(root->data, arr, top, codes);
    }
}

void freeTree(struct treeNode* &root)
{
    if(root == nullptr)
        return;

    freeTree(root->left);
    freeTree(root->right);

    free(root);
}

template<typename Q>
void buildHufTree(Q &prior_q, tree* &hufTree)
{
    long usecs;
    {utimer t0("build huf tree", &usecs);
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
            hufTree->size++;
        }
    }

    //if(printFlag)
    //    cout << "huf_tree in " << usecs << " usecs" << endl;
}

void HuffmanCoding(int th_id, int start, int stop, std::string &stringToCode, 
    std::unordered_map<char, std::string> &codes, std::vector<std::string> &partialEncodedStrs)
{
    //temporary huffman encoded string which will be 
    //copied into partialEncodedStrs
    std::string tmpStr;
    for(int i=start; i < stop; i++)
    {
        char charToCode = stringToCode[i];
        tmpStr += codes[charToCode];
    }

    hufLock.lock();
    partialEncodedStrs[th_id] = tmpStr;
    hufLock.unlock();
}

std::string mapHufCoding(int nw, std::string stringToCode, std::unordered_map<char, std::string> codes)
{
    // size of the string 
    int n = stringToCode.size();

    //vector used to store tids of threads
    std::vector<std::thread> tids;

    //final Huffman encoded string
    std::string encodedStr;
    //vectors of partial strings
    std::vector<std::string> partialEncodedStrs(nw);
    
    int delta = n / nw; //chunk size
    int start, stop;
    
    for(int i=0; i<nw; i++)
    {   
        start = i*delta;
        //check if last chunk to be distributed
        if(i==nw-1) 
          stop = n;
        else
          stop = i*delta + delta;

        tids.push_back(std::thread(HuffmanCoding, i, start, stop, std::ref(stringToCode), 
            std::ref(codes), std::ref(partialEncodedStrs)));

    }
    for(std::thread& t: tids)  // await thread termination
    t.join();

    for (const std::string& str : partialEncodedStrs)
        encodedStr += str;

    return encodedStr;
}


std::string padEncodedStr(std::string str)
{
    int size = str.size();
    int bits = size % 8;
    bits = 8 - bits;

    //pad the string
    str.append(bits, '0');
    return str;
}

char convertToASCII(std::string binaryString)
{
    int decimalValue = 0;
    for (char bit : binaryString) 
        decimalValue = (decimalValue << 1) + (bit - '0');

    return static_cast<char>(decimalValue);
}

//TODO
void encodeStrASCII(int th_id, int start, int stop, std::string &binaryString, 
    std::vector<std::string> &partialEncodedStrs)
{
    std::string encodedStr;
    for(int i=start; i<stop; i+=8)
        encodedStr += convertToASCII(binaryString.substr(i, 8));

    //mutual exclusion on shared dasta structure
    encASCIILock.lock();
    partialEncodedStrs[th_id] = encodedStr;
    encASCIILock.unlock();

}

std::string mapEncodeStrASCII(int nw, std::string binaryString)
{
    int n = binaryString.size();

    //vector used to store tids of threads
    std::vector<std::thread> tids;

    std::string encodedStr;
    std::vector<std::string> partialEncodedStrs(nw);
    
    int delta = n / nw; //chunk size

    //make sure that delta is a multiple of 8
    if(delta % 8 != 0)
    {
        int bits = delta % 8;
        bits = 8 - bits;
        delta += bits;
    }

    int start, stop;
    
    for(int i=0; i<nw; i++)
    {   
        start = i*delta;
        //check if last chunk to be distributed
        if(i==nw-1) 
          stop = n;
        else
          stop = i*delta + delta;

        tids.push_back(std::thread(encodeStrASCII, i, start, stop, std::ref(binaryString), std::ref(partialEncodedStrs)));

    }
    for(std::thread& t: tids)  // await thread termination
    t.join();

    for (const std::string& str : partialEncodedStrs)
        encodedStr += str;

    return encodedStr;
}


int main(int argc, char* argv[])
{
    if(argc == 2 && strcmp(argv[1],"-help")==0) {
        std::cout << "Usage:\n" << argv[0] << " nw filename -v" << std::endl;
        return(0);
    }
    
    int nw = (argc > 1 ? atoi(argv[1]) : 4);
    std::string inputFilename = (argc > 2 ? argv[2] : "bible.txt");
    if(argc > 3 && strcmp(argv[3],"-v") == 0)
        printFlag = 1;    // flag for printing

    //***READING FROM TXT FILE***
    std::string strFile;
    std::string str;
    std::string encodedStr;
    long usecs;
    long usecsTotal;
    {utimer t0("total", &usecsTotal);
        {utimer t0("reading file", &usecs);
            ifstream inFile("txt_files/"+inputFilename);
            if (!inFile.is_open()) 
            {
                std::cout << "Failed to open the file." << std::endl;
                return 1;
            }            
            while(getline (inFile, str))
            {
                strFile += str;
                //strFile.push_back('\n');
            }

            inFile.close();
        }
        if(printFlag)
            std::cout << "reading in " << usecs << " usecs" << std::endl;
        usecs = 0;


        //***COUNTING FREQUENCIES***
        std::vector<int> freqs;
        long usecsTotalNoIO;
        {utimer t0("total no IO", &usecsTotalNoIO);
            {utimer t1("counting freq", &usecs);
                freqs = mapCountFreq(nw,strFile);
            }
            if(printFlag)
                std::cout << "counting in " << usecs << std::endl;
            usecs = 0;
            //if(printFlag)
            //    printFreq(freqs);

            //***INITIALIZE PRIORITY QUEUE AND BINARY TREE***
            // Max priority to lowest freq node
            std::priority_queue<treeNode*, vector<treeNode*>, node_comparison> prior_q; 

            //representation of the binary tree
            struct tree *hufTree = (struct tree*) malloc (sizeof(struct tree));
            hufTree->size = 0;

            //initialize the priority queue
            initQueue(prior_q, freqs, hufTree);

            //*** BUILD HUFFMAN TREE
            //build the huffman tree using the priority queue
            buildHufTree(prior_q, hufTree);

            //set root 
            struct treeNode* myRoot = prior_q.top();
            hufTree->root = myRoot;

            //array used to get Huffman codes
            int arr[MAX_TREE_HT], top = 0;

            //map <char, huffman code>
            std::unordered_map<char, std::string> codes;

            //*GET HUFFMAN CODES USING HUFFMAN TREE
            //traverse the Huffman tree and set codes
            traverseTree(myRoot, arr, top, codes);
            
            //if(printFlag)
                //printMap(codes);
            //*** HUFFMAN CODING ***
            {utimer t2("huffman coding", &usecs);
                encodedStr = mapHufCoding(nw, strFile, codes);
            }
            if(printFlag)
                cout << "huf_coding in " << usecs << " usecs" << endl;
            usecs = 0;
            //pad the coded string to get a multiple of 8
            if(encodedStr.size() % 8 != 0)
                encodedStr = padEncodedStr(encodedStr);

            //encode binary string (result of Huffman coding) as ASCII characters 
            {utimer t3("encode in ASCII", &usecs);
                encodedStr = mapEncodeStrASCII(nw, encodedStr);
            }
            if(printFlag)
                cout << "ASCII_encoding in " << usecs << " usecs" << endl;
            usecs = 0;
            //*** FREE MEMORY ***
            freeTree(myRoot);
            free(hufTree);
        }
        if(printFlag)
            cout << "total_no_IO in " << usecsTotalNoIO << " usecs" << endl;       
        
        //*** WRITING TO FILE ***
        {utimer t4("writing file", &usecs);
            std::ofstream outFile("out_files/encoded_"+inputFilename);

            if (outFile.is_open()) 
            {
                outFile.write(encodedStr.c_str(), encodedStr.size());
                outFile.close();  // Close the file
            }
            else
                std::cout << "Unable to open the file." << std::endl;
        }
        if(printFlag)
            std::cout << "writing in " << usecs << " usecs" << std::endl;
    }
    if(printFlag)
        std::cout << "total in " << usecsTotal << " usecs" << std::endl;
    return (0);
}
