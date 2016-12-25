//
// Created by ahmed on 12/23/16.
//

#include <vector>
#include <sys/stat.h>
#include "FileFragmenter.h"

unsigned int FileFragmenter::NextFragment(void **buffer)
{

    if (!this->has_bytes) {
        (*buffer) = nullptr;
        return 0;
    }
    // Read fragment size from file
    if (this->current_fragment_idx < this->file_fragments) {

        unsigned int current_frag_size = GetNextFragmentSize();

        //cout << "Fragment size:" << current_frag_size << endl;
        (*buffer) = (char *) calloc((size_t) current_frag_size, sizeof(char));
        this->file.read((char *) (*buffer), current_frag_size);

        this->current_fragment_idx++;
        return current_frag_size;
    } else {
        this->has_bytes = false;
        cout << "Fragments ended" << endl;
        return 0;
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
    return GetNextFragmentSize() == 0;
}

FileFragmenter::FileFragmenter(std::string file_path, unsigned int fragment_size)
{
    if (fragment_size < 1) {
        cerr << "Invalid fragment size" << endl;
        return;
    }

    this->fragment_size = fragment_size;
    if (!ValidateFile(file_path)) {
        cerr << "File fragmenter won't work, file isn't valid" << endl;
    }
}

FileFragmenter::FileFragmenter()
{
}

bool FileFragmenter::SetFragmentSize(unsigned int frag_size)
{
    if (frag_size < 1) {
        cerr << "Fragmenter#Invalid fragment size" << endl;
        return false;
    }
    this->fragment_size = frag_size;
    return true;
}


bool FileFragmenter::ValidateFile(string file_path)
{
    this->file.open(file_path.c_str(), ios::in | ios::binary);

    if (this->file.is_open()) {
        cout << "File open success" << endl;
        struct stat file_stat;
        if (stat(file_path.c_str(), &file_stat) == 0) {

            this->file_fragments = ((int) file_stat.st_size / fragment_size) + 1;

            if (file_stat.st_size < 1) {
                cerr << "File is empty" << endl;
                this->has_bytes = false;
                return false;
            }
            this->has_bytes = true;
            this->file_size = (unsigned int) (file_stat.st_size);
            this->current_fragment_idx++;   // Move to fragment 0

            cout << "File fragments:" << this->file_fragments
                 << " File size in bytes:" << file_stat.st_size
                 << endl;
        } else {
            cout << "Failed to get file stats" << endl;
            return false;
        }
    } else {
        cout << "Failed to open file" << endl;
        return false;
    }
    return true;
}

bool FileFragmenter::SetFilePath(string file_path)
{
    if (!ValidateFile(file_path)) {
        cerr << "File fragmenter won't work, file isn't valid" << endl;
        return false;
    }
    return true;
}

int FileFragmenter::GetFragmentCount()
{
    return this->file_fragments;
}

unsigned int FileFragmenter::GetNextFragmentSize()
{
    // TODO optimization, if the current frag idx didn't move, return the last value

    unsigned int frag_size = 0;

    if (file_fragments - current_fragment_idx > 1) {
        frag_size = this->fragment_size;
    } else if (file_fragments - current_fragment_idx == 1) {

        // TODO last fragment, don't read frag size, read the rest of the file
        unsigned int bytes_left = this->file_size - this->file.tellg();
        frag_size = bytes_left;
    } else {
        this->has_bytes = false;
    }


    //last_requested_frag_size_idx = this->current_fragment_idx;

    return frag_size;
}
