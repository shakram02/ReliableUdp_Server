//
// Created by ahmed on 12/23/16.
//

#include <vector>
#include <sys/stat.h>
#include "FileFragmenter.h"

void FileFragmenter::NextFragment(char **buffer)
{

    if (!this->has_bytes) {
        (*buffer) = nullptr;
        return;
    }
    // Read fragment size from file
    if (this->current_fragment_idx < this->file_fragments) {

        int current_frag_size;
        if (file_fragments - current_fragment_idx != 1) {
            current_frag_size = this->fragment_size;
        } else {

            // TODO last fragment, don't read frag size, read the rest of the file
            int bytes_left = this->file_size - (int) this->file.tellg();
            current_frag_size = bytes_left;
            this->has_bytes = false;
        }

        cout << "Fragment size:" << current_frag_size << endl;
        (*buffer) = (char *) calloc((size_t) current_frag_size, sizeof(char));
        this->file.read((*buffer), current_frag_size);

        this->current_fragment_idx++;
    }
}

FileFragmenter::~FileFragmenter()
{
    if (this->file.is_open()) {
        this->file.close();
    }
}

bool FileFragmenter::EndOfFile()
{
    return !this->has_bytes;
}

FileFragmenter::FileFragmenter(std::string file_path, int fragment_size)
{
    if (fragment_size < 1) {
        cerr << "Invalid fragment size" << endl;
    }

    this->fragment_size = fragment_size;
    this->file.open(file_path.c_str(), ios::in | ios::binary);

    if (this->file.is_open()) {
        cout << "File open success" << endl;
        struct stat file_stat;
        if (stat(file_path.c_str(), &file_stat) == 0) {

            this->file_fragments = ((int) file_stat.st_size / fragment_size) + 1;

            if (file_stat.st_size < 1) {
                cerr << "File is empty" << endl;
                this->has_bytes = false;
                return;
            }
            this->has_bytes = true;
            this->file_size = ((int) file_stat.st_size);
            this->current_fragment_idx++;   // Move to fragment 0

            cout << "File fragments:" << this->file_fragments
                 << " File size in bytes:" << file_stat.st_size
                 << endl;
        } else {
            cout << "Failed to get file stats" << endl;
        }
    } else {
        cout << "Failed to open file" << endl;
    }
}
