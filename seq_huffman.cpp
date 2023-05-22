/*Luca Santarella 22/06/23

HUFFMAN CODING (SEQUENTIAL):

*/
// C++ program for Huffman Coding
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <fstream>
#include "utimer.cpp"
#include <chrono>
using namespace std;

#define SIZE 26

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
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
        ++it;
    }
}

int main(int argc, char* argv[])
{
    string strFile;
    string str;
    {utimer("reading file");
        ifstream MyReadFile("txt_files/file_10M.txt");
        while(getline (MyReadFile, str))
        {
            strFile += str;
            strFile.push_back('\n');
        }
    }
    std::map<char, int> freq;
    freq = countFreq(strFile);
    //printFreq(freq);

  return (0);
}
