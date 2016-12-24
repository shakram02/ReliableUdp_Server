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
    FileFragmenter(std::string file_path, int frag_size);

    FileFragmenter();

    bool SetFragmentSize(int frag_size);

    bool SetFilePath(string file_path);

    void NextFragment(char **buffer);

    bool EndOfFile();

    ~FileFragmenter();

private:
    ifstream file;
    int file_fragments = 0;
    int fragment_size = 0;
    int file_size = 0;
    bool has_bytes = false;
    int current_fragment_idx = -1;

    bool ValidateFile(string file_path);

};


#endif //ENHANCEDUDPSERVER_FILEFRAGMENTER_H
