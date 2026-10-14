<div align="center">

# 💾 StorageLite

### A virtual SSD simulator written in C — modeling NAND flash internals from the ground up.

[![Language](https://img.shields.io/badge/Language-C11-blue?style=flat-square&logo=c)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Build](https://img.shields.io/badge/Build-CMake-red?style=flat-square&logo=cmake)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)]()

</div>

---

## 📖 Overview

**StorageLite** is a modular, command-line SSD and memory management simulator written entirely in **standard C (C11)**. It faithfully reproduces the internal behavior of a NAND flash SSD—including the Flash Translation Layer (FTL), dynamic page allocation, garbage collection, and wear leveling—all exposed through an interactive CLI shell.

> Built to demonstrate systems programming mastery: memory ownership, pointer safety, algorithmic correctness, and clean modular design.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🗂️ **Virtual Block/Page Management** | Hierarchical SSD model: `SSD → Blocks → Pages` |
| 📁 **File System Layer** | Logical file CRUD mapped to physical page references |
| ⚡ **Dynamic Page Allocation** | FTL-style sequential page allocation with capacity checking |
| ♻️ **Garbage Collection** | Reclaims invalid pages; relocates valid data; resets erase counters |
| ⚖️ **Wear Leveling** | Always writes to the block with the lowest erase count |
| 🔍 **Fragmentation Analysis** | Reports used / free / invalid pages and fragmentation percentage |
| 🛡️ **Memory Safety** | Every allocation has a matching free; validated with Dr. Memory |
| 💬 **Interactive CLI** | Human-readable commands with full error reporting |

---

## 🏗️ Architecture

```
┌─────────────────────────┐
│   Command Line (CLI)    │  ← User-facing interactive shell
└───────────┬─────────────┘
            │
┌───────────▼─────────────┐
│     Command Parser      │  ← Validates commands and arguments
└───────────┬─────────────┘
            │
┌───────────▼─────────────┐
│       SSD Manager       │  ← Global SSD singleton & lifecycle
└──────┬────────┬──────────┘
       │        │
  Allocator   File Manager
   (FTL)      (Filesystem)
       │        │
└──────┴────────┴──────────┐
│   Storage Algorithms     │
│  GC  │  Wear  │  Frag   │
└──────────────────────────┘
```

---

## 📁 Project Structure

```
StorageLite/
├── CMakeLists.txt          # CMake build configuration
├── README.md
├── SRS.md                  # Software Requirements Specification
│
├── include/                # Public API headers
│   ├── ssd.h               # SSD controller interface
│   ├── block.h             # NAND block interface
│   ├── page.h              # Smallest write unit interface
│   ├── file.h              # Virtual filesystem interface
│   ├── allocator.h         # FTL page allocator interface
│   ├── gc.h                # Garbage collector interface
│   ├── wear.h              # Wear leveling interface
│   ├── cli.h               # CLI shell interface
│   └── utils.h             # Common utilities (safe_malloc, safe_strncpy)
│
├── src/                    # Implementation source files
│   ├── main.c              # Entry point — init, CLI loop, cleanup
│   ├── ssd.c
│   ├── block.c
│   ├── page.c
│   ├── file.c
│   ├── allocator.c
│   ├── gc.c
│   ├── wear.c
│   ├── cli.c
│   └── utils.c
│
└── tests/
    └── test_script.txt     # CLI command script for functional testing
```

---

## 🚀 Getting Started

### Prerequisites

| Tool | Version |
|---|---|
| C compiler | GCC / MSVC (C11) |
| CMake | ≥ 3.10 |
| Git | Any recent version |

> **Windows:** Use [MinGW-w64](https://www.mingw-w64.org/) or MSVC via Visual Studio Build Tools.  
> **Linux/WSL:** GCC is typically pre-installed.

---

### 🔨 Build Instructions

#### Windows (MinGW / MSYS2)

```bash
# 1. Clone the repository
git clone https://github.com/<your-username>/StorageLite.git
cd StorageLite

# 2. Configure and build
cmake -B build -G "MinGW Makefiles"
cmake --build build

# 3. Run
./build/storagelite.exe
```

#### Linux / WSL

```bash
# 1. Clone the repository
git clone https://github.com/<your-username>/StorageLite.git
cd StorageLite

# 2. Configure and build
cmake -B build
cmake --build build

# 3. Run
./build/storagelite
```

#### Windows (MSVC)

```powershell
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Debug
.\build\Debug\storagelite.exe
```

---

## 💻 CLI Reference

Once launched, StorageLite starts an interactive shell. The SSD is pre-configured with **16 blocks × 64 pages × 256 bytes per page** (total: 256 KB).

### Commands

| Command | Syntax | Description |
|---|---|---|
| `create` | `create <filename> <size>` | Create a new virtual file and allocate pages |
| `write` | `write <filename> <data>` | Write data to the file's pages (out-of-place) |
| `read` | `read <filename>` | Display stored data for a file |
| `delete` | `delete <filename>` | Mark file's pages as invalid and remove metadata |
| `search` | `search <filename>` | Show file metadata and page allocation info |
| `list` | `list` | List all stored files |
| `status` | `status` | Print SSD capacity and page statistics |
| `fragmentation` | `fragmentation` | Display fragmentation report |
| `gc` | `gc` | Run the garbage collector |
| `wear` | `wear` | Show per-block erase counts |
| `help` | `help` | Print command usage guide |
| `exit` | `exit` | Safely terminate the simulator |

### Example Session

```
storagelite> create notes.txt 512
Created file 'notes.txt' (Size: 512 bytes, Pages: 2).

storagelite> write notes.txt HelloWorld
Wrote 10 bytes to 'notes.txt'.

storagelite> read notes.txt
Data for 'notes.txt': HelloWorld

storagelite> status
--- SSD Status ---
Blocks: 16 | Pages/Block: 64 | Page Size: 256 bytes
Total Capacity: 262144 bytes
...

storagelite> fragmentation
--- Fragmentation Report ---
Total Pages : 1024
Used Pages  : 2
Free Pages  : 1022
Invalid Pages: 0
Fragmentation: 0.00%

storagelite> delete notes.txt
Deleted file 'notes.txt'.

storagelite> gc
Garbage collection complete.

storagelite> wear
--- Wear Leveling Stats ---
Block 0: Erase Count = 1
Block 1: Erase Count = 0
...

storagelite> exit
```

---

## 🧠 Design Highlights

### Memory Ownership Model

Memory lifetime is strictly hierarchical — mirroring the hardware itself:

```
SSD  →  owns  →  Block[]
Block  →  owns  →  Page[]
FileManager  →  owns  →  FileEntry[] (linked list)
```

Every allocation has exactly one corresponding `free`. Verified clean with **Dr. Memory**.

### NAND Flash Realism

Unlike HDD simulators, StorageLite enforces NAND rules:
- **No in-place overwrites** — writes always go to a fresh page (out-of-place update)
- **Block-level erase only** — garbage collection erases entire blocks
- **Wear leveling** — the allocator always prefers the least-erased block

### Wear Leveling Algorithm

```c
// Always prefer the block with the lowest erase_count that still has free pages
uint32_t wear_get_best_block(SSD* ssd);
```

### Garbage Collection Flow

```
1. Scan all blocks for invalid pages
2. If valid pages exist in the block, relocate them to a fresh block
3. Erase the reclaimed block (reset all pages to FREE)
4. Increment erase_count for the block
5. Update all FileEntry page references
```

---

## 🧪 Testing

A functional test script is included at [`tests/test_script.txt`](tests/test_script.txt).

**Running automated tests (pipe input):**

```bash
# Linux/WSL
./build/storagelite < tests/test_script.txt

# Windows
.\build\storagelite.exe < tests\test_script.txt
```

**Test coverage includes:**
- ✅ Normal CRUD operations (create, write, read, delete)
- ✅ Boundary cases (full SSD, duplicate filenames, missing files)
- ✅ Garbage collection and wear leveling under load
- ✅ Invalid commands and argument validation

---

## 🔬 Memory Verification

This project was verified with **Dr. Memory** on Windows:

```bash
drmemory.exe -- .\build\storagelite.exe < tests\test_script.txt
```

**Result:** No memory leaks, invalid reads/writes, double frees, or use-after-free errors.

---

## 📐 Module Reference

| Module | Header | Responsibility |
|---|---|---|
| SSD Controller | `ssd.h` | Root hardware model; lifecycle and global state |
| Block Manager | `block.h` | NAND block: page grouping, erase count tracking |
| Page Manager | `page.h` | Smallest writeable unit; state machine (FREE→ALLOCATED→INVALID) |
| File Manager | `file.h` | Virtual filesystem; linked-list of `FileEntry` structs |
| Allocator (FTL) | `allocator.h` | Page allocation and invalidation |
| Wear Leveling | `wear.h` | Best-block selection by minimum erase count |
| Garbage Collector | `gc.h` | Reclaim invalid pages; preserve valid data |
| CLI Shell | `cli.h` | Interactive command parsing and dispatch |
| Utilities | `utils.h` | `safe_malloc`, `safe_strncpy`, validation helpers |

---

## 📄 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for details.

---

<div align="center">

Built with ❤️ and pure C.

</div>
