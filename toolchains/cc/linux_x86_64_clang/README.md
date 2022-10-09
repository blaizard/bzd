# Clang

Build the compiler.

```bash
version=15.0.0
name=clang_llvm_$(version)
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git checkout tags/$(version) -b $(version)
rm -rfd build; mkdir build
rm -rfd $(name); mkdir $(name)
cmake -G Ninja -S llvm -B build -DCMAKE_BUILD_TYPE=Release \
                                -DCMAKE_INSTALL_PREFIX=$(pwd)/$(name) \
                                -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \
                                -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
                                -DLLVM_RUNTIME_TARGETS="x86_64-unknown-linux-gnu"
ninja -C build clang clang-format runtimes
ninja -C build install
# Create an archive.
tar -czvf $(name).tar.gz $(name)
```
