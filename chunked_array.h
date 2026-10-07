#ifndef CHUNKED_ARRAY_H
#define CHUNKED_ARRAY_H

#include "proxy.h"
#include <memory>
#include <array>


// Our chunks can store 1028 bytes
template <typename T, size_t chunk_size>
class chunked_array {
    private:
        // Chunks are arrays of type T and size 1028
        using Chunk = array<T, chunk_size>;
        // Ptr_array is a vector of smart pointers that point to chunks
        vector<unique_ptr<Chunk>> ptr_array;

    public:
        // Default constructor & overloaded []
        chunked_array() = default;
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
};

#endif