# Setup pre-commit  🏗️

```shell
sudo apt-get install clang-format
pip install pre-commit
pre-commit install
```


# Enforce pre-commit to run 🏃

```shell
pre-commit run --all-files
```

# Get C++ libs 📚
## Setup vcpkg 📦
```shell
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
rm -rf ./vcpkg/.git*
```

## Install libs 📖
Do not hesitate to read error logs, dependencies are supposed to be added
aswell, *supposed ...* 🤡
```shell
./vcpkg/vcpkg install opengl
./vcpkg/vcpkg install glew
./vcpkg/vcpkg install glm
./vcpkg/vcpkg install freeglut
```

# Build 🛠️
Also works when using CLion build
```shell
cmake -B cmake-build-debug -S . -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
cd cmake-build-debug && make -j8 && cd ..
```

# Run 🏃💨
```shell
cd cmake-build-debug && ./LightningGL && cd ..
```
