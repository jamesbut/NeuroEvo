#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

/*
    Shared memory class - can be used with any type T
*/

#include <string>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


template <typename T>
class SharedMemory {

public:

    SharedMemory(const unsigned int SIZE, const std::string FILE_NAME);

    void write_data(const T& data, const unsigned int mem_location);
    T read_data(const unsigned int mem_location);

private:

    const int _SIZE;

    const std::string _FILE_NAME;
    int _file_descriptor;

    T* _memory;

};

template <typename T>
SharedMemory<T>::SharedMemory(const unsigned int SIZE, const std::string FILE_NAME) :
    _SIZE(SIZE),
    _FILE_NAME(FILE_NAME) {

    //Create shared memory file descriptor
    _file_descriptor = ::shm_open(_FILE_NAME.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    //Check initialisation has been correctly done
    if(_file_descriptor < 0) {

        ::perror(_FILE_NAME.c_str());
        exit(EXIT_FAILURE);

    }

    //Resize
    size_t mem_size = _SIZE * sizeof(T);
    ::ftruncate(_file_descriptor, mem_size);

    //Get pointer
    _memory = reinterpret_cast<T*>(
        ::mmap(NULL,
               mem_size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               _file_descriptor,
               0));

}

template <typename T>
void SharedMemory<T>::write_data(const T& data, const unsigned int mem_location) {

    if(mem_location >= _SIZE) throw std::out_of_range("SharedMemory.write_data(): index is out of range");

    _memory[mem_location] = data;

}

template <typename T>
T SharedMemory<T>::read_data(const unsigned int mem_location) {

    if(mem_location >= _SIZE) throw std::out_of_range("SharedMemory.read_data(): index is out of range");

    return _memory[mem_location];

}

#endif
