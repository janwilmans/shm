//
// shm is a simple shared memory libary
// For more information see https://github.com/janwilmans/shm
//
// Copyright (c) 2021 Jan Wilmans
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "sharedmemory/mmap.h"
#include "sharedmemory/shmopen.h"

#include "gsl/gsl-lite.hpp"

#include <algorithm>
#include <type_traits>

#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

namespace shm {

enum class SharedMode
{
    Create,
    Connect
};

template <typename T, std::size_t N>
gsl::span<const gsl::byte> make_view(const T (&data)[N])
{
    return gsl::make_span(reinterpret_cast<const gsl::byte *>(data), N * sizeof(T));
}

template <typename T>
gsl::span<const gsl::byte> make_view(std::vector<const T> & t)
{
    static_assert(std::is_trivially_copyable<T>::value, "type T can not be memcopied");
    const auto size = std::distance(t.cbegin(), t.cend()) * sizeof(T);
    return gsl::make_span(reinterpret_cast<const gsl::byte *>(t.data()), size);
}

template <typename T>
gsl::span<const gsl::byte> make_view(const T & t)
{
    static_assert(std::is_trivially_copyable<T>::value, "type T can not be memcopied");
    return gsl::make_span(reinterpret_cast<const gsl::byte *>(&t), sizeof(T));
}

template <typename T, std::size_t N>
gsl::span<gsl::byte> make_span(T (&data)[N])
{
    return gsl::make_span(reinterpret_cast<gsl::byte *>(data), N * sizeof(T));
}

template <typename T>
gsl::span<gsl::byte> make_span(std::vector<T> & t)
{
    static_assert(std::is_trivially_copyable<T>::value, "type T can not be memcopied");
    const auto size = std::distance(t.begin(), t.end()) * sizeof(T);
    return gsl::make_span(reinterpret_cast<gsl::byte *>(t.data()), size);
}

template <typename T>
gsl::span<gsl::byte> make_span(T & t)
{
    static_assert(std::is_trivially_copyable<T>::value, "type T can not be memcopied");
    return gsl::make_span(reinterpret_cast<gsl::byte *>(&t), sizeof(T));
}

using byte_view = gsl::span<const gsl::byte>;
using byte_span = gsl::span<gsl::byte>;

template <class T, int mapsize>
class SharedMemory
{
    static_assert(mapsize > 0, "mapsize must be >0");
    static_assert(std::is_trivially_copyable<T>::value, "type can not be memcopied");

public:
    SharedMemory<T, mapsize>() = delete;
    SharedMemory<T, mapsize>(const SharedMemory<T, mapsize> &) = delete;
    SharedMemory<T, mapsize> & operator=(const SharedMemory<T, mapsize> & other) = delete;

    // allow move
    SharedMemory<T, mapsize>(SharedMemory<T, mapsize> && other) noexcept
    {
        std::swap(m_mmap, other.m_mmap);
        std::swap(m_semaphore, other.m_semaphore);
        std::swap(m_data, other.m_data);
    }

    SharedMemory<T, mapsize> &
        operator=(SharedMemory<T, mapsize> && other) noexcept
    {
        std::swap(m_mmap, other.m_mmap);
        std::swap(m_semaphore, other.m_semaphore);
        std::swap(m_data, other.m_data);
        return *this;
    }

    static int get_map_size() { return sizeof(sem_t) + (sizeof(T) * mapsize); }

    SharedMemory<T, mapsize>(SharedMode mode, const char * idname)
    {
        if (mode == SharedMode::Create)
        {
            create(idname);
        }
        else
        {
            connect(idname);
        }
    }

    int write(int index, shm::byte_view view)
    {
        if (index >= mapsize)
        {
            fmt::print("SharedMemory: out of bounds write, asking for index {}/{}.! \n", index, mapsize);
            return 0;
        }
        if (view.size() > sizeof(T))
        {
            fmt::print("SharedMemory: out of bounds write, asking to write {} bytes to entry of size {}.! \n", view.size(), sizeof(T));
            return 0;
        }

        char * to = m_data + (index * sizeof(T));

        sem_wait(m_semaphore);
        ::memcpy(to, view.data(), view.size());
        sem_post(m_semaphore);

        return view.size();
    }

    int read(int index, shm::byte_span span) const
    {
        if (index >= mapsize)
        {
            fmt::print("SharedMemory: out of bounds read, asking for index {}/{}.! \n", index, mapsize);
            return 0;
        }

        if (span.size() > sizeof(T))
        {
            fmt::print("SharedMemory: out of bounds read, asking for {}, from entry of size {}.! \n", span.size(), sizeof(T));
            return 0;
        }

        const char * from = m_data + (index * sizeof(T));
        sem_wait(m_semaphore);
        ::memcpy(span.data(), from, span.size());
        sem_post(m_semaphore);
        return span.size();
    }

    void create(const char * idname)
    {
        auto handle = shm::open(idname, O_RDWR | O_CREAT, 0777);
        int size = ::ftruncate(handle.get(), sizeof(T));
        if (size == -1)
        {
            throw errno_exception("ftruncate");
        }

        m_mmap = std::make_unique<shm::mmap>(get_map_size(), std::move(handle));
        assign(m_mmap->data());
        sem_init(m_semaphore, 1, 0);
        sem_post(m_semaphore);
    }

    void connect(const char * idname)
    {
        auto handle = shm::open(idname, O_RDWR, 0777);
        m_mmap = std::make_unique<shm::mmap>(get_map_size(), std::move(handle));
        assign(m_mmap->data());
    }

    void assign(char * data)
    {
        m_semaphore = (sem_t *)data;
        m_data = data + sizeof(sem_t);
    }

    std::unique_ptr<shm::mmap> m_mmap;
    sem_t * m_semaphore;
    char * m_data;
};

} // namespace shm