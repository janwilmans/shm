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

#include "sharedmemory/shmopen.h"

namespace shm {

open::open(open && other) noexcept
{
    std::swap(m_idname, other.m_idname);
    m_handle = other.m_handle;
    other.m_handle = -1;
}

open & open::operator=(open && other) noexcept
{
    std::swap(m_idname, other.m_idname);
    std::swap(m_handle, other.m_handle);
    return *this;
}

open::open(const char * name, int oflag, mode_t mode) :
    m_idname(name)
{
    m_handle = ::shm_open(m_idname.data(), oflag, mode);
    if (m_handle == -1)
    {
        throw errno_exception("shm_open");
    }
}

open::~open()
{
    if (m_handle != -1)
    {
        shm_unlink(m_idname.data());
    }
}

int open::get() const
{
    return m_handle;
}

} // namespace shm