//
// Created by ahmed on 12/23/16.
//

#ifndef ENHANCEDUDPSERVER_FILEFRAGMENTER_H
#define ENHANCEDUDPSERVER_FILEFRAGMENTER_H

#include<iostream>
#include <string>
#include <fstream>

using namespace std;

class FileFragmenter
{
public:
    FileFragmenter(std::string file_path, int fragment_size);

    void NextFragment(char **buffer);

    bool EndOfFile();

    ~FileFragmenter();

private:
    ifstream file;
    int file_fragments;
    int fragment_size;
    int file_size;
    bool has_bytes;
    int current_fragment_idx = -1;
};


#endif //ENHANCEDUDPSERVER_FILEFRAGMENTER_H
