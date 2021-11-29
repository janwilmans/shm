# shm

This is a very simple shared memory abstraction.
It is intended as a building block for more user-friendly classes.

Example usage:

```
TEST(SharedMemoryLib, SharedMemoryLib)
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
```

In the example T was `int` but any trivially-copyable structure can be used.
