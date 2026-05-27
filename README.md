# Ches

## Build and Run

#### Requirements

- **C++20**
- [**CMake 3.15**](https://cmake.org/download/) - build system generator
- [**raylib 5.5**](https://www.raylib.com/) - videogame programming library (auto fetched by CMake)

```bash
git clone https://github.com/Ka-raS/ches.git
cd ches

cmake --preset debug
cmake --build --preset debug
ctest --test-dir out/build/debug

(cd out/build/debug/ches && ./ches)
```

By <a href="https://commons.wikimedia.org/wiki/User:Cburnett" title="User:Cburnett">Cburnett</a> - <span class="int-own-work" lang="en">Own work</span>, <a href="http://creativecommons.org/licenses/by-sa/3.0/" title="Creative Commons Attribution-Share Alike 3.0">CC BY-SA 3.0</a>, <a href="https://commons.wikimedia.org/w/index.php?curid=1499801">Link</a>