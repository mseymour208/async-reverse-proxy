#include "chunked_array.h"


template <typename T, size_t chunk_size>
T& chunked_array<T, chunk_size>::operator[](size_t index) {
    size_t outer_idx = index / chunk_size;
    size_t inner_idx = index % chunk_size;

    if (outer_idx >= ptr_array.size()) {
        ptr_array.resize(outer_idx + 1);
    }

    if (ptr_array[outer_idx] == nullptr) {
        ptr_array[outer_idx] = make_unique<Chunk>();
    }

    return (*ptr_array[outer_idx])[inner_idx];
}

template <typename T, size_t chunk_size>
const T& chunked_array<T, chunk_size>::operator[](size_t index) const {
    size_t outer_idx = index / chunk_size;
    size_t inner_idx = index % chunk_size;
    return (*ptr_array[outer_idx])[inner_idx];
}


template class chunked_array<Connection>;