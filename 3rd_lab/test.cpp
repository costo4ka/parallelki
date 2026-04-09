#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>

constexpr double EPS = 1e-9;

bool approx_equal(double a, double b)
{
    return std::abs(a - b) < EPS;
}

int test_sin(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return 1;
    }

    std::string line;
    int total = 0, passed = 0, failed = 0;

    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        double arg, result;
        char comma;
        if (!(iss >> arg >> comma >> result) || comma != ',')
        {
            std::cerr << "  PARSE ERROR: " << line << std::endl;
            ++failed;
            continue;
        }

        double expected = std::sin(arg);
        ++total;
        if (approx_equal(result, expected))
        {
            ++passed;
        }
        else
        {
            ++failed;
            if (failed <= 5)
                std::cerr << "  FAIL: sin(" << arg << ") = " << result
                          << ", expected " << expected << std::endl;
        }
    }

    std::cout << "  sin:  " << passed << "/" << total << " passed";
    if (failed > 0)
        std::cout << " (" << failed << " FAILED)";
    std::cout << std::endl;
    return failed;
}

int test_sqrt(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return 1;
    }

    std::string line;
    int total = 0, passed = 0, failed = 0;

    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        double arg, result;
        char comma;
        if (!(iss >> arg >> comma >> result) || comma != ',')
        {
            std::cerr << "  PARSE ERROR: " << line << std::endl;
            ++failed;
            continue;
        }

        double expected = std::sqrt(arg);
        ++total;
        if (approx_equal(result, expected))
        {
            ++passed;
        }
        else
        {
            ++failed;
            if (failed <= 5)
                std::cerr << "  FAIL: sqrt(" << arg << ") = " << result
                          << ", expected " << expected << std::endl;
        }
    }

    std::cout << "  sqrt: " << passed << "/" << total << " passed";
    if (failed > 0)
        std::cout << " (" << failed << " FAILED)";
    std::cout << std::endl;
    return failed;
}

int test_pow(const std::string& filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        std::cerr << "ERROR: Cannot open " << filename << std::endl;
        return 1;
    }

    std::string line;
    int total = 0, passed = 0, failed = 0;

    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        double x, y, result;
        char c1, c2;
        if (!(iss >> x >> c1 >> y >> c2 >> result) || c1 != ',' || c2 != ',')
        {
            std::cerr << "  PARSE ERROR: " << line << std::endl;
            ++failed;
            continue;
        }

        double expected = std::pow(x, y);
        ++total;
        if (approx_equal(result, expected))
        {
            ++passed;
        }
        else
        {
            ++failed;
            if (failed <= 5)
                std::cerr << "  FAIL: pow(" << x << ", " << y << ") = " << result
                          << ", expected " << expected << std::endl;
        }
    }

    std::cout << "  pow:  " << passed << "/" << total << " passed";
    if (failed > 0)
        std::cout << " (" << failed << " FAILED)";
    std::cout << std::endl;
    return failed;
}

int main()
{
    std::cout << std::fixed << std::setprecision(15);
    std::cout << "=== Verification Test ===" << std::endl;
    std::cout << "Tolerance: " << EPS << std::endl;
    std::cout << std::endl;

    int total_failures = 0;
    total_failures += test_sin("results/sin_results.csv");
    total_failures += test_sqrt("results/sqrt_results.csv");
    total_failures += test_pow("results/pow_results.csv");

    std::cout << std::endl;
    if (total_failures == 0)
        std::cout << "ALL TESTS PASSED" << std::endl;
    else
        std::cout << "TESTS FAILED: " << total_failures << " error(s)" << std::endl;

    return (total_failures == 0) ? 0 : 1;
}
