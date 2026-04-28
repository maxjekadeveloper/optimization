#include <vector>
#include <random>
#include <iostream>
#include <cassert>
#include <chrono>
#include <future>

class MatrixOptimization {
public:
    // Basic matrix multiplication - poor cache locality
    static void matrixMultiplyBasic(const std::vector<std::vector<double>>& A,
                                   const std::vector<std::vector<double>>& B,
                                   std::vector<std::vector<double>>& C) {
        int n = static_cast<int>(A.size());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                C[i][j] = 0.0;
                for (int k = 0; k < n; ++k) {
                    C[i][j] += A[i][k] * B[k][j];  // Poor cache locality
                }
            }
        }
    }
    
    static void matrixMultiplyOptimized(const std::vector<std::vector<double>>& A,
                                      const std::vector<std::vector<double>>& B,
                                      std::vector<std::vector<double>>& C) {
        // Reorder loops for better cache performance (i-k-j instead of i-j-k)
        int n = static_cast<int>(A.size());
        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < n; ++k) {
                C[i][k] = 0.0;
                for (int j = 0; j < n; ++j) {
                    C[i][j] += A[i][k] * B[k][j];  // rich cache locality
                }
            }
        }
    }
    
    // Basic vector dot product
    static double dotProductBasic(const std::vector<double>& a, const std::vector<double>& b) {
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    static double dotProductUnrolled(const std::vector<double>& a, const std::vector<double>& b) {
        double sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;

        size_t i = 0;
        size_t n = a.size();

        for (; i + 3 < n; i += 4) {
        sum0 += a[i] * b[i];
        sum1 += a[i + 1] * b[i + 1];
        sum2 += a[i + 2] * b[i + 2];
        sum3 += a[i + 3] * b[i + 3];
        }

        double result = sum0 + sum1 + sum2 + sum3;

        for (; i < n; ++i) {
        result += a[i] * b[i];
        }

        return result;
    }

    static void generateTestData(std::vector<std::vector<double>>& A) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(1.0, 100.0);
    
        for (int k = 0; k < A.size(); ++k) {
            for (int j = 0; j < A.size(); ++j) {
                    A[k][j] = dis(gen);
            }
        }
    }

    static void generateTestData2(std::vector<double>& A) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(1.0, 100.0);
    
        for (int j = 0; j < A.capacity(); ++j) {
                A.push_back(dis(gen));
        }
    }
    
    template<class Func, class... Args>
    static void countTime(Func &&func, Args&&... args)
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        func(std::forward<Args>(args)...);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "duration = " << duration << std::endl;
    }

    static void printVec(const std::vector<std::vector<double>>& vec)
    {
        std::cout << std::endl;
        for(const auto& internalVec : vec)
        {
            for(auto value : internalVec)
            {
                std::cout << value << ' ';
            }
            std::cout << std::endl;
        }
    }

    static void printVec(const std::vector<double>& vec)
    {
        std::cout << std::endl;
        int i = 0;
        for(const auto& value : vec)
        {
            std::cout << value << ' ';
            i++;
            if(i >= 10) {
                std::cout << std::endl;
                i = 0;
            }
        }
    }
};

int main()
{
    {
    size_t size = 1000;
    std::vector<std::vector<double>> A(size, std::vector<double>(size)), B(size, std::vector<double>(size));
    std::vector<std::vector<double>> poor_C(size, std::vector<double>(size));
    std::vector<std::vector<double>> rich_C(size, std::vector<double>(size));

    auto future1 = std::async(std::launch::async, &MatrixOptimization::generateTestData, std::ref(A));
    auto future2 = std::async(std::launch::async, &MatrixOptimization::generateTestData, std::ref(B));

    future1.get(); future2.get();
    
    MatrixOptimization::countTime(&MatrixOptimization::matrixMultiplyBasic, A, B, poor_C);
    MatrixOptimization::countTime(&MatrixOptimization::matrixMultiplyOptimized, A, B, rich_C);
    }

    {
    size_t size = 100000000;
    std::vector<double> A, B; A.reserve(size); B.reserve(size);

    auto future1 = std::async(std::launch::async, &MatrixOptimization::generateTestData2, std::ref(A));
    auto future2 = std::async(std::launch::async, &MatrixOptimization::generateTestData2, std::ref(B));

    future1.get(); future2.get();

    MatrixOptimization::countTime(&MatrixOptimization::dotProductBasic, A, B);
    MatrixOptimization::countTime(&MatrixOptimization::dotProductUnrolled, A, B);
    }

    return 0;
}

