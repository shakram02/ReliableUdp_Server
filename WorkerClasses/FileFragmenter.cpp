//
// Created by ahmed on 12/23/16.
//

#include <vector>
#include <sys/stat.h>
#include <UdpLibGlobaldefs.h>
#include "FileFragmenter.h"

unique_ptr<ByteVector> FileFragmenter::NextFragment()
{
    // Read fragment size from file
    if (this->current_fragment_idx >= this->file_fragments)return nullptr;

    unsigned int current_frag_size = GetNextFragmentSize();

    char temp_container[current_frag_size];
    this->file.read((temp_container), current_frag_size);

    if (this->file.fail()) {
        cerr << "Failed to read file" << endl;
        return nullptr;
    }

    ByteVector *buffer = CopyToVector(temp_container, current_frag_size);

    this->current_fragment_idx++;
    return unique_ptr<ByteVector>(buffer);
}

bool FileFragmenter::EndOfFile()
{
    return GetNextFragmentSize() == 0;
}

FileFragmenter::FileFragmenter(std::string file_path)
{
    if (!ValidateFile(file_path)) {
        cerr << "File fragmenter won't work, file isn't valid" << endl;
    }
}

FileFragmenter::FileFragmenter()
{
}

bool FileFragmenter::ValidateFile(string file_path)
{
    this->file.open(file_path.c_str(), ios::in | ios::binary);

    if (this->file.fail()) {
        cerr << "Failed to open file" << endl;
        return false;
    }

    struct stat file_stat;

    if (stat(file_path.c_str(), &file_stat) != 0) {
        cerr << "Failed to get file stats" << endl;
        return false;
    }

    this->file_fragments = ((int) file_stat.st_size / DATA_FRAGMENT_SIZE) + 1;

    if (file_stat.st_size < 1) {
        cerr << "File is empty" << endl;
        this->has_bytes = false;
        return false;
    }

    this->has_bytes = true;
    this->file_size = (unsigned int) (file_stat.st_size);
    this->current_fragment_idx++;   // Move to fragment 0

    // Debug statements
//    cout << "File fragments:" << this->file_fragments
//         << " File size in bytes:" << file_stat.st_size
//         << endl;

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
        frag_size = DATA_FRAGMENT_SIZE;

    } else if (file_fragments - current_fragment_idx == 1) {

        // last fragment, don't read frag size, read the rest of the file
        unsigned int bytes_left = this->file_size - this->file.tellg();
        frag_size = bytes_left;

    } else {
        this->has_bytes = false;
    }

    return frag_size;
}

ByteVector *FileFragmenter::CopyToVector(char container[], int length)
{
    // C --> C++, unfortunately, copying can't be avoided
    ByteVector *buffer = new ByteVector();
    buffer->reserve((unsigned long) length); // Avoid re-allocations
    for (int i = 0; i < length; ++i) {
        buffer->push_back(std::move((byte) container[i]));
    }
    return buffer;
}


FileFragmenter::~FileFragmenter()
{
    if (this->file.is_open()) {
        this->file.close();
    }
}