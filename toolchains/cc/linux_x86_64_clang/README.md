# Clang

How is it build?

git clone https://github.com/llvm/llvm-project.git
cd llvm-project
rm -rfd build; mkdir build
rm -rfd dist; mkdir dist
cmake -G Ninja -S llvm -B build -DCMAKE_BUILD_TYPE=Release \
                                -DCMAKE_INSTALL_PREFIX=$(pwd)/dist \
                                -DLLVM_ENABLE_PROJECTS="clang" \
                                -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
                                -DLLVM_RUNTIME_TARGETS="x86_64-unknown-linux-gnu"
ninja -C build runtimes
ninja -C build install-runtimes
