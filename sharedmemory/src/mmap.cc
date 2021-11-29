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

#include "sharedmemory/mmap.h"
#include "sharedmemory/sharedmemory.h"
#include "sharedmemory/shmopen.h"

namespace shm {

mmap::mmap(mmap && other) noexcept
    :
    m_size(other.m_size), m_handle(std::move(other.m_handle)), m_data(other.m_data)
{
    other.m_data = MAP_FAILED;
}

mmap & mmap::operator=(mmap && other) noexcept
{
    std::swap(m_size, other.m_size);
    m_handle = std::move(other.m_handle);
    std::swap(m_data, other.m_data);
    return *this;
}

mmap::mmap(int size, shm::open handle) :
    m_size(size), m_handle(std::move(handle))
{
    m_data = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                    m_handle.get(), 0);

    if (m_data == MAP_FAILED)
    {
        throw errno_exception("mmap");
    }
}

mmap::~mmap()
{
    if (m_data != MAP_FAILED)
    {
        munmap(m_data, m_size);
    }
}

char * mmap::data()
{
    return (char *)m_data;
}

} // namespace shm