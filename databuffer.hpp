#pragma once

#include <cstdlib>
#include <stdexcept>
#include <assert.h>

template<typename T>
struct databuffer
{
    databuffer() : mem(nullptr), size(0) {}

    databuffer(size_t size) : databuffer()
    {
        ensuresize(size);
    }

    ~databuffer()
    {
        if (mem)
        {
            free(mem);
        }
    }

    void ensuresize(size_t newsize)
    {
        if (newsize > size)
        {
            T *newmem = mem;
            if (newmem == nullptr)
            {
                newmem = static_cast<T *>(malloc(newsize * sizeof(T)));
            }
            else
            {
                newmem = static_cast<T *>(realloc(newmem, newsize * sizeof(T)));
            }
            if (newmem == nullptr)
            {
                throw std::runtime_error("databuffer: allocation failed");
            }
            mem = memset(newmem + size, 0, (newsize - size) * sizeof(T));
            size = newsize;
        }
    }

    T *mem;
    size_t size;
};
