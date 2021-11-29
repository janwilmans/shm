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

#include "sharedmemory/sharedmemory.h"

#include <gtest/gtest.h>

#include <atomic>
#include <string>
#include <thread>

constexpr uint32_t max_string_length = 500;

struct buffer_t
{
    char text[max_string_length];
};

const char * global_unique_name = "/global_unique_name_buffer";

using namespace shm;
using namespace std::chrono_literals;

static std::atomic<bool> done;

static const int buffersize = 1024;

TEST(SharedMemoryLib, SharedMemoryNativeType)
{
    // process 1
    int in_value = 42;
    SharedMemory<int, 16> creator(SharedMode::Create, global_unique_name);
    creator.write(0, shm::make_view(in_value));

    // process 2
    int out_value = 0;
    SharedMemory<int, 16> peer(SharedMode::Connect, global_unique_name);
    creator.read(0, shm::make_span(out_value));
    EXPECT_EQ(42, out_value);
}

struct PodExample
{
    int counter;
    double pi;
};

TEST(SharedMemoryLib, SharedMemoryPOD)
{
    // process 1
    PodExample in_value{42, 3.14};
    SharedMemory<PodExample, 10> creator(SharedMode::Create, global_unique_name);
    creator.write(0, shm::make_view(in_value));

    // process 2
    PodExample out_value{};
    SharedMemory<PodExample, 10> peer(SharedMode::Connect, global_unique_name);
    creator.read(0, shm::make_span(out_value));
    EXPECT_EQ(::memcmp(&in_value, &out_value, sizeof(PodExample)), 0);
}

TEST(SharedMemoryLib, SharedMemoryConcurrent)
{
    SharedMemory<buffer_t, buffersize> creator(SharedMode::Create,
                                               global_unique_name);

    auto spinwrite = [&]() {
        while (!done)
        {
            creator.write(1, shm::make_view("one"));
            creator.write(2, shm::make_view("two"));
            creator.write(3, shm::make_view("three"));
            creator.write(1, shm::make_view("o-n-e"));
            creator.write(2, shm::make_view("t-w-o"));
            creator.write(3, shm::make_view("t-h-r-e-e"));
        }
    };

    std::thread t_write(spinwrite);

    struct reader
    {
        reader() :
            peer(SharedMode::Connect, global_unique_name), t([&]() { spin(); }) {}

        ~reader()
        {
            if (t.joinable())
            {
                t.join();
            }
        }

        void spin()
        {
            char text[max_string_length];

            while (!done)
            {
                peer.read(1, shm::make_span(text));
                std::string s1(text);
                EXPECT_TRUE(s1 == "one" || s1 == "o-n-e");

                peer.read(2, shm::make_span(text));
                std::string s2(text);
                EXPECT_TRUE(s2 == "two" || s2 == "t-w-o");

                peer.read(3, shm::make_span(text));
                std::string s3(text);
                EXPECT_TRUE(s3 == "three" || s3 == "t-h-r-e-e");
            }
        }

        SharedMemory<buffer_t, buffersize> peer;
        std::thread t;
    };

    reader t0;
    reader t1;
    reader t2;
    reader t3;
    reader t4;
    reader t5;
    reader t6;
    reader t7;
    reader t8;
    reader t9;

    std::this_thread::sleep_for(2s);
    done = true;
    t_write.join();
}
