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

#include "sharedmemory/shmopen.h"

#include <sys/mman.h>

namespace shm {

class mmap
{
public:
    mmap() = delete;
    mmap(const mmap &) = delete;
    mmap & operator=(const mmap & other) = delete;

    // allow move
    mmap(mmap && other) noexcept;
    mmap & operator=(mmap && other) noexcept;

    mmap(int size, shm::open handle);
    ~mmap();

    char * data();

private:
    int m_size;
    shm::open m_handle;
    void * m_data;
};

} // namespace shm
