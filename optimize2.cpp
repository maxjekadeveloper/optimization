#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <random>
#include <iostream>
#include <cassert>

class SearchOptimization {
public:
    static int linearSearch(const std::vector<int>& data, int target) {
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i] == target) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    static int binarySearch(const std::vector<int>& sortedData, int target) {
        int left = 0, right = static_cast<int>(sortedData.size()) - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (sortedData[mid] == target) return mid;
            else if (sortedData[mid] < target) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }
    
    static void buildHashMap(const std::vector<int>& data, 
                            std::unordered_map<int, int>& hashMap) {
        hashMap.reserve(data.size());
        for(int i{0}; i < data.size(); ++i)
            hashMap[i] = data[i];
    }
    
    static int hashSearch(const std::unordered_map<int, int>& hashMap, int target) {
        auto it = hashMap.find(target);
        if(it != hashMap.end())
            return it->second;
        return -1;
    }
};

// Generate test data
std::vector<int> generateTestData(int size) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, size * 10);
    
    for (int& val : data) {
        val = dis(gen);
    }
    return data;
}

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
    auto vec = generateTestData(10000000);
    std::unordered_map<int, int> hashMap;

    countTime(&SearchOptimization::linearSearch, vec, 100);
    countTime(&SearchOptimization::binarySearch, vec, 100);
    SearchOptimization::buildHashMap(vec, hashMap);
    countTime(&SearchOptimization::hashSearch, hashMap, 100);

    return 0;
}