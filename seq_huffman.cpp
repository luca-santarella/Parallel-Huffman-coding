/*Luca Santarella 22/06/23

HUFFMAN CODING (SEQUENTIAL):

*/
// C++ program for Huffman Coding
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <fstream>
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

int main(int argc, char* argv[]){
    string str = "this is an example of a huffman tree";
    std::map<char, int> freq;
    freq = countFreq(str);
    // Get an iterator pointing to the first element in the map
    std::map<char, int>::iterator it = freq.begin();
    while (it != freq.end())
    {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
        ++it;
  }
  return (0);
}
