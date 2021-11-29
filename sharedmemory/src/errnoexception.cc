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

#include "sharedmemory/errnoexception.h"

#include <fmt/format.h>

#include <errno.h>

namespace shm {

std::string get_errno_string(int code)
{
    switch (code)
    {
    case 1:
        return "EPERM, Operation not permitted";
    case 2:
        return "ENOENT, No such file or directory";
    case 30:
        return "ENOENT, Read-only file system";
    }
    return "Error not listed";
}

errno_exception::errno_exception(const char * description) :
    m_errno(errno)
{
    m_what = fmt::format("{}: errno: {} ({})", description,
                         get_errno_string(m_errno), m_errno);
}

const char * errno_exception::what() const noexcept
{
    return m_what.data();
}

} // namespace shm