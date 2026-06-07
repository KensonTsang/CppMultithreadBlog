# C++ Multithreading Blog

A hands-on C++17 code companion for learning multithreading concepts — from raw mutex usage to reusable concurrency patterns.

## Topics Covered

### Basic (`src/Basic.cpp`)
Introduces the fundamentals of `std::thread` and `std::mutex`.

| Example | What it shows |
|---------|---------------|
| `Example1` | Interleaved output with and without a mutex lock |
| `Example2` | Race condition in a shared counter; fine-grained lock vs. local accumulation |

### Practical (`src/Practical.cpp`)
Evolving producer/worker architecture applied to parallel prime-number search near 1,000,000,000.

| Example | Pattern |
|---------|---------|
| `Example1` | Single-threaded baseline |
| `Example2` | Static work distribution — 4 threads, each owns a fixed range |
| `Example3` | Producer/worker — workers pull from a shared queue protected by raw `std::mutex` |
| `Example4` | Producer/worker — same logic, mutex hidden behind `MutexProtected<T>` |
| `Example5` | Producer/worker — adds `CondVarProtected<T>` so the main thread waits on a condition variable instead of busy-joining |

### Deadlock (`src/Deadlock.cpp`)
Three approaches to a classic two-account money-transfer scenario.

| Example | Technique |
|---------|-----------|
| `Example1` | Naive locking order — **causes a deadlock** (intentional demo) |
| `Example2` | Lock-by-pointer-order — consistent ordering prevents deadlock |
| `Example3` | `std::try_to_lock` with back-off retry |

## Reusable Patterns

### `MutexProtected<T>` (`include/MutexProtected.h`)
Bundles a `std::mutex` and its protected data together. `scopedLock()` returns a move-only `MyLocked<T>` that holds the lock for its lifetime, accessed through `operator->`.

```cpp
MutexProtected<MyStruct> safe;
auto locked = safe.scopedLock();
locked->field = value;   // mutex held; released when `locked` goes out of scope
```

### `CondVarProtected<T>` (`include/CondVarProtected.h`)
Extends the same pattern with a `std::condition_variable`. The scoped lock exposes a `wait()` method, and the wrapper exposes `notify_all()`.

```cpp
CondVarProtected<MyStruct> safe;
// consumer
auto lk = safe.scopedLock();
lk.wait();               // atomically releases lock and waits

// producer (no lock needed to notify)
safe.notify_all();
```

## Building

Requires CMake 3.10+ and a C++17-capable compiler.

```bash
cmake -B build
cmake --build build
./build/CppMultithreadBlog
```

To switch examples, uncomment the relevant call in `src/main.cpp`.

## Project Structure

```
.
├── include/
│   ├── MutexProtected.h      # RAII mutex wrapper template
│   └── CondVarProtected.h    # RAII condition-variable wrapper template
├── src/
│   ├── main.cpp              # Entry point — uncomment examples to run
│   ├── Basic.cpp             # Mutex basics
│   ├── Practical.cpp         # Parallel prime search, evolving patterns
│   └── Deadlock.cpp          # Deadlock demo and fixes
└── CMakeLists.txt
```
