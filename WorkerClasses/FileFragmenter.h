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
    FileFragmenter(std::string file_path, unsigned int frag_size);

    FileFragmenter();

    bool SetFragmentSize(unsigned int frag_size);

    bool SetFilePath(string file_path);

    unsigned int NextFragment(void **buffer);

    unsigned int GetNextFragmentSize();

    bool EndOfFile();

    int GetFragmentCount();

    ~FileFragmenter();

private:
    ifstream file;
    int file_fragments = 0;
    unsigned int fragment_size = 0;
    unsigned int file_size = 0;
    bool has_bytes = false;
    int current_fragment_idx = -1;
    int last_requested_frag_size_idx = -1;

    bool ValidateFile(string file_path);

};


#endif //ENHANCEDUDPSERVER_FILEFRAGMENTER_H
