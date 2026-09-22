#ifndef CHUNKED_ARRAY_H
#define CHUNKED_ARRAY_H

#include "proxy.h"
#include <memory>
#include <array>


template <typename T, size_t chunk_size = 1028>
class chunked_array {
    private:
        using Chunk = array<T, chunk_size>;
        vector<unique_ptr<Chunk>> ptr_array;

    public:
        chunked_array() = default;
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
};

#endif