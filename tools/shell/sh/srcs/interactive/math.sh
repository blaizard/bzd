# simple math extension to calculate from the command line
=() {
    py_code="from math import *; print(${@})"
    python -c "${py_code}"
}
