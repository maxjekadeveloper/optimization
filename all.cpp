#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <unordered_map>

using namespace std;

// Constants for testing
const int SMALL_SIZE = 1000;
const int LARGE_SIZE = 10000;
const int NUM_ITERATIONS = 5;

// Simple timer class for performance measurement
class SimpleTimer {
private:
    chrono::high_resolution_clock::time_point start_time;
    
public:
    void start() {
        start_time = chrono::high_resolution_clock::now();
    }
    
    double getElapsedMs() {
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000.0;  // Convert to milliseconds
    }
};

// ============================================================================
// PART 1: Memory Optimization Techniques
// ============================================================================

// Unoptimized data structure (Array of Structures)
struct UnoptimizedParticle {
    double x, y, z;      // Position
    double vx, vy, vz;   // Velocity  
    double mass;
    int id;
    char padding[4];     // Unintentional padding
};

// Create optimized data structure using Structure of Arrays (SoA)
class OptimizedParticleSystem {
private:
    std::vector<double> x ,y, z, vx, vy, vz, mass;
    std::vector<int> id;
    
public:
    size_t size() const {
        return id.size();
    }
    
    void resize(size_t n) {
        x.resize(n);
        y.resize(n);
        z.resize(n);
        vx.resize(n);
        vy.resize(n);
        vz.resize(n);
        mass.resize(n);
        id.resize(n);
    }
    
    void setParticle(size_t i, double x, double y, double z, 
                    double vx, double vy, double vz, double mass, int id) {
        this->x[i] = x;
        this->y[i] = y;
        this->z[i] = z;
        this->vx[i] = vx;
        this->vy[i] = vy;
        this->vz[i] = vz;
        this->mass[i] = mass;
        this->id[i] = id;
    }
    
    void updatePositions(double dt) {
        // This should be cache-friendly due to sequential access
        for (int i = 0; i < x.size(); ++i) 
        {
            x[i] += vx[i] * dt;
            y[i] += vy[i] * dt;
            z[i] += vz[i] * dt;
        }
    }
    
    double getTotalKineticEnergy() const 
    {
        double sum = 0.0;

        for (int i = 0; i < x.size(); ++i) 
            sum += 0.5 * mass[i] * (vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i]);

        return sum;
    }
};

template<typename T>
class SimpleMemoryPool {
private:
    vector<T> pool;
    vector<uint8_t> available;
    size_t size;
    
public:
    SimpleMemoryPool(size_t poolSize) : pool(poolSize), available(poolSize, true), size(poolSize) {}
    
    T* allocate() {
        for (size_t i = 0; i < available.size(); ++i) {
            if (available[i]) {
                available[i] = false;
                size--;
                return &pool[i];
            }
        }
        return nullptr; 
    }
    
    void deallocate(T* ptr) {
        if (ptr >= &pool[0] && ptr < &pool[0] + pool.size()) {
            size_t index = ptr - &pool[0];
            available[index] = true;
            size++;
        }
    }
    
    size_t getPoolSize() const {
        return size;
    }
};

// ============================================================================
// PART 2: Algorithm Analysis and Optimization
// ============================================================================

class SearchAlgorithms {
public:
    // Linear search - O(n)
    static int linearSearch(const vector<int>& data, int target) {
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == target) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    // Binary search - O(log n) - requires sorted data
    static int binarySearch(const vector<int>& sortedData, int target) {
        int left = 0, right = static_cast<int>(sortedData.size()) - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (sortedData[mid] == target) return mid;
            else if (sortedData[mid] < target) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
    
    // Hash-based search using unordered_map - O(1) average
    static void buildHashMap(const vector<int>& data, unordered_map<int, int>& hashMap) {
        hashMap.reserve(data.size());
        for(int i{0}; i < data.size(); ++i)
            hashMap[i] = data[i];
    }
    
    static int hashSearch(const unordered_map<int, int>& hashMap, int target) {
        auto it = hashMap.find(target);
        if(it != hashMap.end())
            return it->second;
        return -1;
    }
};

class SortingOptimization {
public:
    static void bubbleSort(vector<int>& data) {
        size_t n = data.size();

        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = 0; j < n - i - 1; ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
    
    static void stlSort(vector<int>& data) {
        std::sort(data.begin(), data.end());
    }
    
    // Optimized for small arrays - hybrid approach
    static void hybridSort(vector<int>& data) {
        if (data.size() <= 20) {
            insertionSort(data);
        } else {
            stlSort(data);
        }
    }
    
private:
    static void insertionSort(vector<int>& data) {
        // Simple insertion sort for small arrays
        for (size_t i = 1; i < data.size(); ++i) {
            int key = data[i];
            int j = i - 1;
            while (j >= 0 && data[j] > key) {
                data[j + 1] = data[j];
                j--;
            }
            data[j + 1] = key;
        }
    }
};

// ============================================================================
// PART 3: Loop and Computational Optimizations
// ============================================================================

class LoopOptimizations {
public:
    // Unoptimized matrix multiplication
    static void matrixMultiplyBasic(const vector<vector<double>>& A,
                                   const vector<vector<double>>& B,
                                   vector<vector<double>>& C) {
        int n = A.size();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                C[i][j] = 0.0;
                for (int k = 0; k < n; ++k) {
                    C[i][j] += A[i][k] * B[k][j];  // Poor cache locality
                }
            }
        }
    }
    
    // Cache-optimized matrix multiplication with loop interchange
    static void matrixMultiplyOptimized(const vector<vector<double>>& A,
                                      const vector<vector<double>>& B,  
                                      vector<vector<double>>& C) {
        int n = A.size();
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n; ++k) {
                C[i][k] = 0.0;
                for (int j = 0; j < n; ++j) {
                    C[i][j] += A[i][k] * B[k][j];  // rich cache locality
                }
            }
        }
    }
    
    static double dotProductBasic(const vector<double>& a, const vector<double>& b) {
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    static double dotProductUnrolled(const vector<double>& a, const vector<double>& b) {
        double result = 0.0;
        size_t n = a.size();
        size_t i = 0;
        
        for (; i + 3 < n; i += 4) {
            result += a[i] * b[i];
            result += a[i + 1] * b[i + 1];
            result += a[i + 2] * b[i + 2];
            result += a[i + 3] * b[i + 3];
        }
        
        for (; i < n; ++i) {
            result += a[i] * b[i];
        }
        
        return result;
    }
};

// ============================================================================
// PART 4: Performance Testing Framework
// ============================================================================

class PerformanceTester {
private:
    SimpleTimer timer;
    
public:
    template<typename Func>
    double measurePerformance(Func function, const string& description) {
        cout << "Testing: " << description << "..." << endl;
        timer.start();
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            function();
        }
        double totalTime = timer.getElapsedMs();
        double avgTime = totalTime / NUM_ITERATIONS;
        
        cout << "  Average time: " << fixed << setprecision(3) << avgTime << " ms" << endl;
        return avgTime;
    }
    
    void comparePerformance(double baselineTime, double optimizedTime, const string& optimization) {
        double speedup = baselineTime / optimizedTime;
        double improvement = ((baselineTime - optimizedTime) / baselineTime) * 100.0;
        
        cout << "  " << optimization << ":" << endl;
        cout << "    Speedup: " << fixed << setprecision(2) << speedup << "x" << endl;
        cout << "    Improvement: " << fixed << setprecision(1) << improvement << "%" << endl;
    }
};

// Test data generators
vector<int> generateRandomData(int size) {
    vector<int> data(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, size * 10);
    
    for (int& val : data) {
        val = dis(gen);
    }
    return data;
}

vector<UnoptimizedParticle> generateParticles(int count) {
    vector<UnoptimizedParticle> particles(count);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> pos(-100.0, 100.0);
    uniform_real_distribution<double> vel(-10.0, 10.0);
    uniform_real_distribution<double> mass(0.1, 10.0);
    
    for (int i = 0; i < count; ++i) {
        particles[i] = {pos(gen), pos(gen), pos(gen),
                       vel(gen), vel(gen), vel(gen),
                       mass(gen), i, {0}};
    }
    return particles;
}

// Main testing functions
void testMemoryOptimizations() {
    cout << "\n=== Memory Optimization Tests ===" << endl;
    
    PerformanceTester tester;
    auto particles = generateParticles(LARGE_SIZE);
    
    // Test Array of Structures vs Structure of Arrays
    auto aosTest = [&particles]() {
        double totalEnergy = 0.0;
        for (const auto& p : particles) {
            double vel2 = p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
            totalEnergy += 0.5 * p.mass * vel2;
        }
    };
    
    OptimizedParticleSystem soaParticles;
    soaParticles.resize(LARGE_SIZE);
    for (size_t i = 0; i < particles.size(); ++i) {
        soaParticles.setParticle(i, particles[i].x, particles[i].y, particles[i].z,
                                particles[i].vx, particles[i].vy, particles[i].vz,
                                particles[i].mass, particles[i].id);
    }
    
    auto soaTest = [&soaParticles]() {
        double totalEnergy = soaParticles.getTotalKineticEnergy();
    };
    
    double aosTime = tester.measurePerformance(aosTest, "Array of Structures");
    double soaTime = tester.measurePerformance(soaTest, "Structure of Arrays");
    
    tester.comparePerformance(aosTime, soaTime, "SoA vs AoS");
    
    // Test memory pool
    cout << "\nTesting memory pool allocation..." << endl;
    SimpleMemoryPool<int> pool(1000);
    
    auto poolTest = [&pool]() {
        vector<int*> ptrs;
        for (int i = 0; i < 100; ++i) {
            ptrs.push_back(pool.allocate());
        }
        for (int* ptr : ptrs) {
            if (ptr) pool.deallocate(ptr);
        }
    };
    
    auto heapTest = []() {
        vector<int*> ptrs;
        for (int i = 0; i < 100; ++i) {
            ptrs.push_back(new int(i));
        }
        for (int* ptr : ptrs) {
            delete ptr;
        }
    };
    
    double poolTime = tester.measurePerformance(poolTest, "Memory Pool");
    double heapTime = tester.measurePerformance(heapTest, "Heap Allocation");
    
    tester.comparePerformance(heapTime, poolTime, "Memory Pool vs Heap");
}

void testAlgorithmOptimizations() {
    cout << "\n=== Algorithm Optimization Tests ===" << endl;
    
    PerformanceTester tester;
    auto data = generateRandomData(LARGE_SIZE);
    auto sortedData = data;
    sort(sortedData.begin(), sortedData.end());
    
    int target = data[data.size() / 2];  // Pick middle element
    
    // Search algorithm comparison
    auto linearTest = [&data, target]() {
        SearchAlgorithms::linearSearch(data, target);
    };
    
    auto binaryTest = [&sortedData, target]() {
        SearchAlgorithms::binarySearch(sortedData, target);
    };
    
    unordered_map<int, int> hashMap;
    SearchAlgorithms::buildHashMap(data, hashMap);
    auto hashTest = [&hashMap, target]() {
        SearchAlgorithms::hashSearch(hashMap, target);
    };
    
    double linearTime = tester.measurePerformance(linearTest, "Linear Search");
    double binaryTime = tester.measurePerformance(binaryTest, "Binary Search");
    double hashTime = tester.measurePerformance(hashTest, "Hash Search");
    
    tester.comparePerformance(linearTime, binaryTime, "Binary vs Linear");
    tester.comparePerformance(linearTime, hashTime, "Hash vs Linear");
    
    // Sorting algorithm comparison
    auto testData = generateRandomData(SMALL_SIZE);
    
    auto bubbleTest = [&testData]() {
        auto data = testData;
        SortingOptimization::bubbleSort(data);
    };
    
    auto stlTest = [&testData]() {
        auto data = testData;
        SortingOptimization::stlSort(data);
    };
    
    double bubbleTime = tester.measurePerformance(bubbleTest, "Bubble Sort");
    double stlTime = tester.measurePerformance(stlTest, "STL Sort");
    
    tester.comparePerformance(bubbleTime, stlTime, "STL vs Bubble");
}

void testLoopOptimizations() {
    cout << "\n=== Loop Optimization Tests ===" << endl;
    
    PerformanceTester tester;
    const int matrixSize = 200;  // Small enough for reasonable test time
    
    // Initialize matrices
    vector<vector<double>> A(matrixSize, vector<double>(matrixSize, 1.0));
    vector<vector<double>> B(matrixSize, vector<double>(matrixSize, 2.0));
    vector<vector<double>> C1(matrixSize, vector<double>(matrixSize, 0.0));
    vector<vector<double>> C2(matrixSize, vector<double>(matrixSize, 0.0));
    
    auto basicMatrixTest = [&]() {
        LoopOptimizations::matrixMultiplyBasic(A, B, C1);
    };
    
    auto optimizedMatrixTest = [&]() {
        LoopOptimizations::matrixMultiplyOptimized(A, B, C2);
    };
    
    double basicTime = tester.measurePerformance(basicMatrixTest, "Basic Matrix Multiply");
    double optimizedTime = tester.measurePerformance(optimizedMatrixTest, "Optimized Matrix Multiply");
    
    tester.comparePerformance(basicTime, optimizedTime, "Loop Interchange");
    
    vector<double> vec1(LARGE_SIZE, 1.5);
    vector<double> vec2(LARGE_SIZE, 2.5);
    
    auto basicDotTest = [&]() {
        LoopOptimizations::dotProductBasic(vec1, vec2);
    };
    
    auto unrolledDotTest = [&]() {
        LoopOptimizations::dotProductUnrolled(vec1, vec2);
    };
    
    double basicDotTime = tester.measurePerformance(basicDotTest, "Basic Dot Product");
    double unrolledDotTime = tester.measurePerformance(unrolledDotTest, "Unrolled Dot Product");
    
    tester.comparePerformance(basicDotTime, unrolledDotTime, "Loop Unrolling");
}

int main() {
    cout << "=== Basic Code Optimization Lab ===" << endl;
    cout << "Learning memory optimization, algorithm analysis, and compiler optimization" << endl;
    
    testMemoryOptimizations();
    testAlgorithmOptimizations();
    testLoopOptimizations();
    
    cout << "\n=== Compiler Optimization Reminder ===" << endl;
    cout << "Remember to test with different compiler flags:" << endl;
    cout << "  -O0 (no optimization)" << endl;
    cout << "  -O2 (standard optimization)" << endl;
    cout << "  -O3 (maximum optimization)" << endl;
    cout << "  -march=native (optimize for your CPU)" << endl;
    
    cout << "\n=== Lab Complete! ===" << endl;
    cout << "You've successfully implemented basic optimization techniques!" << endl;
    
    return 0;
}