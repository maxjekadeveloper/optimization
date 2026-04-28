#include <vector>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <chrono>

template<typename T>
class CustomMemoryPool {
private:
    // Add member variables for pool implementation
    std::vector<T> pool;
    std::vector<uint8_t> available;
    
public:
    CustomMemoryPool(size_t poolSize) : pool(poolSize), available(poolSize, true) {
        // Initialize pre-allocated memory pool
    }
    
    T* allocate() {
        // Return pointer to available memory slot
        // Mark slot as used
        for (size_t i = 0; i < available.size(); ++i) {
            if (available[i]) {
                available[i] = false;
                return &pool[i];
            }
        }
        return nullptr;
    }
    
    void deallocate(T* ptr) {
        if (ptr >= &pool[0] && ptr < &pool[0] + pool.size()) {
            size_t index = ptr - &pool[0];
            available[index] = true;
        }
    }
    
    size_t availableSlots() const {
        // Return count of available memory slots
        return std::count(available.begin(), available.end(), true);
    }
};

// Test object for memory pool
struct TestObject {
    double data[8];  // 64 bytes
    int id;
    TestObject(int id_val = 1) : id(id_val) {
        for (int i = 0; i < 8; ++i) data[i] = id_val * i;
    }
};

template<class Func, class... Args>
void countTime(Func &&func, Args&&... args)
{
        auto startTime = std::chrono::high_resolution_clock::now();

        func(std::forward<Args>(args)...);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "duration = " << duration << std::endl;
}

int main()
{
    CustomMemoryPool<TestObject> pool(1);
    countTime([&pool]{
        for(int i{0}; i < 1000000; ++i)
        {
            auto ptr = pool.allocate();
            pool.deallocate(ptr);
        }
    });

    countTime([]{
        for(int i{0}; i < 1000000; ++i)
        {
            TestObject *obj = new TestObject();
            delete obj;
        }
    });

    return 0;
}