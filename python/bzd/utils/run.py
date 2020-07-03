#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os
import subprocess
import sys
import threading
import selectors

class _ExecuteResult:
    def __init__(self, output, returncode):
        self.output = output
        self.returncode = returncode

    def getStdout(self):
        return "".join([entry[1] for entry in self.output if entry[0] == True])

    def getStderr(self):
        return "".join([entry[1] for entry in self.output if entry[0] == False])

    def getOutput(self):
        return "".join([entry[1] for entry in self.output])

    def getReturnCode(self):
        return self.returncode

"""
Run a process locally.
"""
def localCommand(cmds, inputs = None, ignoreFailure = False, env=None, timeoutS = 60):

    sel = selectors.DefaultSelector()
    output = []
    proc = subprocess.Popen(cmds, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE, env=env)
    timer = threading.Timer(timeoutS, proc.kill)
    sel.register(proc.stdout, selectors.EVENT_READ)
    sel.register(proc.stderr, selectors.EVENT_READ)

    try:
        isRunning = True
        timer.start()
        while isRunning and timer.is_alive():
            for key, _ in sel.select():
                data = key.fileobj.read1(128).decode(errors = "ignore")
                if not data:
                    isRunning = False
                if data != "":
                    output.append(((key.fileobj is proc.stdout), data))

        if proc.poll() == None:
            output.append((False, "Process did not complete"))
        if not timer.is_alive():
            output.append((False, "Execution of '{}' timed out after {}s, terminating process.\n".format(" ".join(cmds), timeoutS)))

    finally:
        timer.cancel()

    result = _ExecuteResult(
            output = output,
            returncode = proc.returncode)

    assert ignoreFailure or proc.returncode == 0, "Return code {}\n{}".format(result.getReturnCode(), result.getOutput())

    return result

"""
Execute a python script locally.
"""
def localPython(script, args = [], **kargs):
    return localCommand([sys.executable, script] + args, **kargs)

"""
Execute a bash script locally.
"""
def localBash(script, args = [], **kargs):
    return localCommand(["bash", "-s", "--"] + args, inputs=script, **kargs)

"""
Execute a bazel binary locally. The caller must run bazel and all runfiles must be already available.
"""
def localBazelBinary(path, args=[], env={}, **kargs):
    env["RUNFILES_DIR"] = os.environ["RUNFILES_DIR"] if "RUNFILES_DIR" in os.environ else os.path.dirname(os.getcwd())
    return localCommand([path] + args, env=env, **kargs)
