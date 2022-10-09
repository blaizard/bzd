# Clang

Build the compiler.

```bash
VERSION=15.0.0
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-${VERSION}/llvm-project-${VERSION}.src.tar.xz | tar -xJ
pushd llvm-project-${VERSION}.src
cmake -G "Unix Makefiles" -S llvm -B build -DCMAKE_BUILD_TYPE=Release \
                                -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" \
                                -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
                                -DLLVM_RUNTIME_TARGETS="x86_64-unknown-linux-gnu"
cd build
make -j4
make install DESTDIR="$(pwd)/${PACKAGE}"
# Create an archive.
tar -czvf ${PACKAGE}.tar.gz ${PACKAGE}
```
