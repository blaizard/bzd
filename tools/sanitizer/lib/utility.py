#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os
import sys
import subprocess
import signal
import multiprocessing

"""
Execute multiple commands in parallel
"""
def executeMultiCommandWrapper(config):
    proc = subprocess.Popen(config["cmd"], cwd=config["cwd"])
    proc.wait()

def executeMultiCommand(configs, cwd="."):

    pool = multiprocessing.Pool()

    # Hook to force aborting the program on Ctrl+C
    def signal_handler(sig, frame):
        pool.terminate()
        pool.join()
        os.kill(os.getpid(), signal.SIGTERM)
    signal.signal(signal.SIGINT, signal_handler)

    pool.map(executeMultiCommandWrapper, configs)
    pool.close()
    pool.join()

    signal.signal(signal.SIGINT, signal.SIG_DFL)

"""
Execute a single command and return the output
"""
def executeCommand(cmd, cwd="."):
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd)

    output, errors = proc.communicate()
    output = output.decode("utf-8")
    errors = errors.decode("utf-8")
    returncode = proc.returncode

    if returncode != 0:
        print("ERROR: Command '%s' failed with code %i" % (str(" ".join(cmd)), returncode))
        print("**** stdout")
        print(output)
        print("**** stderr")
        print(errors)
        sys.exit(1)

    return output

"""
Ensure that a specific tool exists
"""
def assertCommand(cmd, hint = ""):
    try:
        executeCommand(cmd)
    except:
        print("ERROR: Unable to find '%s', is it installed? %s" % (str(cmd[0]), str(hint)))
        sys.exit(1)
