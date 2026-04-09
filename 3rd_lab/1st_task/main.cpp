#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

// инициализация вектора числами от 0 до n-1
void initializeVector(std::vector<double>& vec, int n) {
    for (int j = 0; j < n; j++) {
        vec[j] = static_cast<double>(j);
    }
}

//   matrix   - ссылка на матрицу(матрица в виде одномерного массива)
//   startRow - первая строка которую обрабатывает этот поток
//   endRow   - первая строка которую НЕ обрабатывает этот поток
//   numCols  - количество столбцов матрицы
// ФУНКЦИЯ ЗАПОЛНЯЕТ ЧАСТЬ МАТРИЦЫ. каждый элемент матрицы вычисляется как сумма индексов строки и столбца: i + j
void initializeMatrix(std::vector<double>& matrix, int startRow, int endRow, int numCols) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < numCols; j++) {
            matrix[i * numCols + j] = static_cast<double>(i + j);
        }
    }
}


// функция умножения части матрицы на вектор
// каждый поток считает только свои строки результата
void computeRows(
        const std::vector<double>& matrix,
        const std::vector<double>& vec,
        std::vector<double>& result,
        int startRow, int endRow, int size)
{
    for (int i = startRow; i < endRow; i++) {
        result[i] = 0.0;  // обнуляем перед накоплением суммы
        for (int j = 0; j < size; j++) {
            // накапливаем скалярное произведение
            result[i] += matrix[i * size + j] * vec[j];
        }
    }
}

int main(int argc, char** argv) {
    // размер матрицы по умолчанию 20000, можно задать вторым аргументом
    int n = 20000;
    int m = 20000;

    // количество потоков берём из аргументов командной строки
    int numThreads = 1;
    if (argc > 1) {
        numThreads = std::stoi(argv[1]);
    }

    // размер матрицы из второго аргумента
    if (argc > 2) {
        n = std::stoi(argv[2]);
        m = n;
    }

    // количество повторений замера (3-й аргумент, по умолчанию 5)
    int numRuns = 5;
    if (argc > 3) {
        numRuns = std::stoi(argv[3]);
    }

    std::vector<double> matrix(n * m);
    std::vector<double> vec(m);
    std::vector<double> result(n);


    // параллельная инициализация
    // считаем сколько строк достаётся каждому потоку
    int itemsPerThread = n / numThreads;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);  // резервируем место под потоки


    for (int i = 0; i < numThreads; i++) {
        int lowBound = i * itemsPerThread;

        // последний поток берёт все оставшиеся строки
        int upBound = (i == numThreads - 1) ? n : lowBound + itemsPerThread;

        // передаём ref() чтобы поток работал с оригинальным вектором, а не с копией
        threads.emplace_back(initializeMatrix, std::ref(matrix), lowBound, upBound, m);
    }

    // вектор vec инициализируем в отдельном потоке параллельно с матрицей
    std::thread vectorThread(initializeVector, std::ref(vec), m);

    // ждём завершения всех потоков инициализации матрицы
    for (auto& t : threads) {
        t.join();
    }

    // ждём завершения потока инициализации вектора
    vectorThread.join();

    // очищаем список потоков чтобы переиспользовать для умножения
    threads.clear();

    // замер последовательного умножения (среднее по numRuns запускам)
    double totalT1 = 0.0;
    for (int run = 0; run < numRuns; run++) {
        const auto startT1 = std::chrono::steady_clock::now();
        computeRows(matrix, vec, result, 0, n, m);
        const auto endT1 = std::chrono::steady_clock::now();
        const std::chrono::duration<double> t1 = endT1 - startT1;
        totalT1 += t1.count();
    }
    double avgT1 = totalT1 / numRuns;

    // замер параллельного умножения (среднее по numRuns запускам)
    double totalTp = 0.0;
    for (int run = 0; run < numRuns; run++) {
        threads.clear();

        const auto startTime = std::chrono::steady_clock::now();

        for (int i = 0; i < numThreads; i++) {
            int lowBound = i * itemsPerThread;
            int upBound = (i == numThreads - 1) ? n : lowBound + itemsPerThread;

            threads.emplace_back(
                    computeRows,
                    std::ref(matrix),
                    std::ref(vec),
                    std::ref(result),
                    lowBound, upBound, m
            );
        }

        // ждём пока все потоки закончат считать
        for (auto& t : threads) {
            t.join();
        }

        const auto endTime = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = endTime - startTime;
        totalTp += elapsed.count();
    }
    double avgTp = totalTp / numRuns;

    double speedup = avgT1 / avgTp;

    // выводим результаты (средние по numRuns запускам)
    std::cout << "потоков: " << numThreads << std::endl;
    std::cout << "замеров: " << numRuns << std::endl;
    std::cout << "T1: " << avgT1 << " сек" << std::endl;
    std::cout << "T" << numThreads << ": " << avgTp << " сек" << std::endl;
    std::cout << "S" << numThreads << ": " << speedup << std::endl;

    // выводим несколько элементов для проверки корректности
    std::cout << "result[0] = " << result[0] << std::endl;
    std::cout << "result[n/2] = " << result[n / 2] << std::endl;
    std::cout << "result[n-1] = " << result[n - 1] << std::endl;

    return 0;
}