#include "server.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>
#include <filesystem>

template<typename T>
T fun_sin(T arg)
{
    return std::sin(arg);
}

template<typename T>
T fun_sqrt(T arg)
{
    return std::sqrt(arg);
}

template<typename T>
T fun_pow(T x, T y)
{
    return std::pow(x, y);
}

void client_sin(Server<double>& server, int N, const std::string& filename)
{
    std::ofstream out(filename);
    out << std::fixed << std::setprecision(15);
    out << "# sin results\n";
    out << "# arg,result\n";

    std::mt19937_64 gen(42);
    std::uniform_real_distribution<double> dist(0.0, 6.283185307179586); // [0, 2*pi]

    struct TaskInfo { double arg; size_t id; };
    std::vector<TaskInfo> tasks;
    tasks.reserve(N);

    for (int i = 0; i < N; ++i)
    {
        double arg = dist(gen);
        size_t id = server.add_task([arg]() { return fun_sin(arg); });
        tasks.push_back({arg, id});
    }

    for (auto& t : tasks)
    {
        double result = server.request_result(t.id);
        out << t.arg << "," << result << "\n";
    }
}

void client_sqrt(Server<double>& server, int N, const std::string& filename)
{
    std::ofstream out(filename);
    out << std::fixed << std::setprecision(15);
    out << "# sqrt results\n";
    out << "# arg,result\n";

    std::mt19937_64 gen(123);
    std::uniform_real_distribution<double> dist(0.0, 1000.0);

    struct TaskInfo { double arg; size_t id; };
    std::vector<TaskInfo> tasks;
    tasks.reserve(N);

    for (int i = 0; i < N; ++i)
    {
        double arg = dist(gen);
        size_t id = server.add_task([arg]() { return fun_sqrt(arg); });
        tasks.push_back({arg, id});
    }

    for (auto& t : tasks)
    {
        double result = server.request_result(t.id);
        out << t.arg << "," << result << "\n";
    }
}

void client_pow(Server<double>& server, int N, const std::string& filename)
{
    std::ofstream out(filename);
    out << std::fixed << std::setprecision(15);
    out << "# pow results\n";
    out << "# x,y,result\n";

    std::mt19937_64 gen(456);
    std::uniform_real_distribution<double> dist_x(1.0, 10.0);
    std::uniform_real_distribution<double> dist_y(0.0, 5.0);

    struct TaskInfo { double x; double y; size_t id; };
    std::vector<TaskInfo> tasks;
    tasks.reserve(N);

    for (int i = 0; i < N; ++i)
    {
        double x = dist_x(gen);
        double y = dist_y(gen);
        size_t id = server.add_task([x, y]() { return fun_pow(x, y); });
        tasks.push_back({x, y, id});
    }

    for (auto& t : tasks)
    {
        double result = server.request_result(t.id);
        out << t.x << "," << t.y << "," << result << "\n";
    }
}

int main(int argc, char* argv[])
{
    int N = 100;
    if (argc > 1)
        N = std::atoi(argv[1]);

    if (N < 6 || N > 9999)
    {
        std::cerr << "N must be in range (5, 10000), got " << N << "\n";
        return 1;
    }

    std::filesystem::create_directories("results");

    std::cout << "=== Client-Server Task Processing ===" << std::endl;
    std::cout << "N = " << N << " tasks per client" << std::endl;
    std::cout << "Starting server..." << std::endl;

    Server<double> server;
    server.start();

    auto t_start = std::chrono::high_resolution_clock::now();

    std::thread t1(client_sin,  std::ref(server), N, "results/sin_results.csv");
    std::thread t2(client_sqrt, std::ref(server), N, "results/sqrt_results.csv");
    std::thread t3(client_pow,  std::ref(server), N, "results/pow_results.csv");

    t1.join();
    t2.join();
    t3.join();

    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(t_end - t_start).count();

    std::cout << "All clients finished." << std::endl;
    std::cout << "Total tasks processed: " << 3 * N << std::endl;
    std::cout << "Time elapsed: " << std::fixed << std::setprecision(6) << elapsed << " s" << std::endl;

    server.stop();
    std::cout << "Server stopped." << std::endl;

    std::cout << "\nResults saved to:" << std::endl;
    std::cout << "  results/sin_results.csv" << std::endl;
    std::cout << "  results/sqrt_results.csv" << std::endl;
    std::cout << "  results/pow_results.csv" << std::endl;

    return 0;
}
