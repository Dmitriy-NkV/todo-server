# Todo List API

RESTful API для управления задачами, написанное на C++.

## Технологии

- **C++23**
- **Boost.Beast** - HTTP сервер
- **PostgreSQL** - база данных  
- **libpqxx** - клиент PostgreSQL для C++
- **nlohmann/json** - работа с JSON
- **Google Test** - unit-тестирование
- **Docker** - контейнеризация
- **CMake** - система сборки

## Возможности

- CRUD операции
- Многопоточная и асинхронная обработка запросов
- Docker контейнеризация

## Запуск

### Docker (рекомендуется)
# Запуск приложения
```
docker-compose --profile app up
```
# Запуск тестов
```
docker-compose --profile tests up
```

## Локальная сборка
```
mkdir build && cd build
cmake -DCMAKE_CXX_STANDARD=23 ..
make -j$(nproc)
./src/Server
```
