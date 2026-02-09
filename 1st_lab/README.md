
## Выбор типа данных

### Для `float` (по умолчанию):
```bash
cmake -S . -B build
cmake --build build
./build/Lab1
```

### Для `double`:
```bash
cmake -S . -B build -DUSE_DOUBLE=ON
cmake --build build
./build/Lab1
```

## Результаты суммы
- `float`: `-0.0277862`
- `double`: `4.89582e-11`


