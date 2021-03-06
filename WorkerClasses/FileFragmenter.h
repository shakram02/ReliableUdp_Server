//
// Created by ahmed on 12/23/16.
//

#ifndef ENHANCEDUDPSERVER_FILEFRAGMENTER_H
#define ENHANCEDUDPSERVER_FILEFRAGMENTER_H

#include<iostream>
#include <string>
#include <fstream>
#include <UdpLibGlobaldefs.h>

using namespace std;

class FileFragmenter
{
public:
    FileFragmenter(std::string file_path);

    FileFragmenter();

    bool SetFilePath(string file_path);

    unique_ptr<ByteVector> NextFragment();

    inline unsigned int GetNextFragmentSize();

    bool EndOfFile();

    int GetFragmentCount();

    ~FileFragmenter();

private:
    ifstream file;
    int file_fragments = 0;
    unsigned int file_size = 0;
    bool has_bytes = false;
    int current_fragment_idx = -1;
    int last_requested_frag_size_idx = -1;

    bool ValidateFile(string file_path);

    inline ByteVector *CopyToVector(char container[], int length);
};


#endif //ENHANCEDUDPSERVER_FILEFRAGMENTER_H
