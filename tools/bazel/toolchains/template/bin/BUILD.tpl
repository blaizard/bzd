package(default_visibility = ['//visibility:public'])

FILES = {
    'nvcc_wrapper' : ['nvcc_wrapper.py'],
    'cc': ['wrapper-cc', '@%{toolchain_package_name}//:cc'],
    'cpp': ['wrapper-cpp', '@%{toolchain_package_name}//:cpp'],
    'ar': ['wrapper-ar', '@%{toolchain_package_name}//:ar'],
    'nm': ['wrapper-nm', '@%{toolchain_package_name}//:nm'],
    'objdump': ['wrapper-objdump', '@%{toolchain_package_name}//:objdump'],
    'gcov': ['wrapper-gcov', '@%{toolchain_package_name}//:gcov'],
    'as': ['wrapper-as', '@%{toolchain_package_name}//:as']
}

[
    filegroup(
        name = key,
        srcs = value
    )
    for key, value in FILES.items()
]

filegroup(
  name = 'tool_wrappers',
  srcs = [
    'nvcc_wrapper',
    ':cc',
    ':cpp',
    ':ar',
    ':nm',
    ':objdump',
    ':gcov',
    ':as',
  ],
)
