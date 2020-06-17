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

namespace NeuroEvo {

template <typename T>
class SharedMemory {

public:

    SharedMemory(const unsigned size, const std::string& file_name) :
        _size(size),
        _file_name(file_name) 
    {

        //Create shared memory file descriptor
        _file_descriptor = ::shm_open(_file_name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

        //Check initialisation has been correctly done
        if(_file_descriptor < 0) {

            ::perror(_file_name.c_str());
            exit(EXIT_FAILURE);

        }

        //Resize
        size_t mem_size = _size * sizeof(T);
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

    void write_data(const T& data, const unsigned mem_location) 
    {
        if(mem_location >= _size) 
            throw std::out_of_range("SharedMemory.write_data(): index is out of range");

        _memory[mem_location] = data;
    }

    const T& read_data(const unsigned mem_location)
    {
        if(mem_location >= _size) 
            throw std::out_of_range("SharedMemory.read_data(): index is out of range");

        return _memory[mem_location];
    }

private:

    const unsigned _size;

    const std::string _file_name;
    int _file_descriptor;

    T* _memory;

};

} // namespace NeuroEvo

#endif
