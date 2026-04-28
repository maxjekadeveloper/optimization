#include <iostream>
#include <vector>
#include <chrono>
#include <random>

// Current implementation - Array of Structures
struct Particle {
    double x, y, z;      // Position
    double vx, vy, vz;   // Velocity
    double mass;
    int id;
};

class ParticleSystemAoS {
private:
    std::vector<Particle> particles;
    
public:
    void resize(size_t count) {
        particles.resize(count);
    }
    
    void initializeParticles() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> pos(-100.0, 100.0);
        std::uniform_real_distribution<double> vel(-10.0, 10.0);
        std::uniform_real_distribution<double> mass(0.1, 10.0);
        
        for (size_t i = 0; i < particles.size(); ++i) {
            particles[i] = {pos(gen), pos(gen), pos(gen),
                           vel(gen), vel(gen), vel(gen),
                           mass(gen), static_cast<int>(i)};
        }
    }
    
    double calculateTotalKineticEnergy() const {
        double totalEnergy = 0.0;
        for (const auto& p : particles) {
            double vel2 = p.vx * p.vx + p.vy * p.vy + p.vz * p.vz;
            totalEnergy += 0.5 * p.mass * vel2;
        }
        return totalEnergy;
    }
};

class ParticleSystemSoA {
private:
    std::vector<double> x ,y, z, vx, vy, vz, mas;
    std::vector<int> id;
    
public:
    void resize(size_t count) {
        x.resize(count);
        y.resize(count);
        z.resize(count);
        vx.resize(count);
        vy.resize(count);
        vz.resize(count);
        mas.resize(count);
        id.resize(count);
    }
    
    void initializeParticles() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> pos(-100.0, 100.0);
        std::uniform_real_distribution<double> vel(-10.0, 10.0);
        std::uniform_real_distribution<double> mass(0.1, 10.0);
        
        for (size_t i = 0; i < x.size(); ++i) {
            x[i] = pos(gen); y[i] = pos(gen); z[i] = pos(gen);
            vx[i] = vel(gen); vy[i] = vel(gen); vz[i] = vel(gen);
            mas[i] = mass(gen); id[i] = static_cast<int>(i);
        }
    }
    
    
    double calculateTotalKineticEnergy() const {
        double totalEnergy = 0.0;
        for (size_t i = 0; i < x.size(); ++i) {
            double vel2 = vx[i] * vx[i] + vy[i] * vy[i] + vz[i] * vz[i];
            totalEnergy += 0.5 * mas[i] * vel2;
        }
        return totalEnergy;
    }
};

template<class Func>
void countTime(Func &&func)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    func();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "duration = " << duration << std::endl;
}

int main()
{
    const int size = 10000000;
    const int loop = 100;
    ParticleSystemAoS aos;
    aos.resize(size);
    aos.initializeParticles();
    countTime([&aos]{
        double result;
        for(int i=0;i<loop;i++)
        result = aos.calculateTotalKineticEnergy();
        std::cout << "result = " << result << std::endl;
    });


    ParticleSystemSoA soa;
    soa.resize(size);
    soa.initializeParticles();
    countTime([&soa]{
        double result;
        for(int i=0;i<loop;i++)
        result = soa.calculateTotalKineticEnergy();
        std::cout << "result = " << result << std::endl;
    });
    
    return 0;
}