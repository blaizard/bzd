# simple math extension to calculate from the command line
bzd_math() {
    py_code="from math import *; print(${@})"
    python -c "${py_code}"
}
