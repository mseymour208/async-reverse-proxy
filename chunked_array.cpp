#include "chunked_array.h"

template <typename T, size_t chunk_size = 1028>
void push_back(const T& fd) {
    // Where we allocate data
    int outer_idx = fd / chunk_size;
    int inner_idx = fd % chunk_size;

    // We don't have outer_idx spaces in outer vector
    if (ptr_array.size() < outer_idx) {
        // resize
        ptr_array.resize(outer_idx, make_unique<Chunk>());
    }

    // ptr at outer_idx is empty
    if (ptr_array[outer_idx] == nullptr) {
        ptr_array.push_back(make_unique<Chunk>());
    }

    (*ptr_array[outer_idx])[inner_idx] = fd;

}

T& operator[](size_t index) {
    size_t outer_idx = index / chunk_size;
    size_t inner_idx = index % chunk_size;
    return (*ptr_array[outer_idx])[inner_idx];
}

const T& operator[](size_t index) const {
    size_t outer_idx = index / chunk_size;
    size_t inner_idx = index % chunk_size;
    return (*ptr_array[outer_idx])[inner_idx];
}
