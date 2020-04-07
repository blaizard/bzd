#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import argparse
import http.server
import os
import socketserver
import tarfile
import tempfile
import signal

class WebServer(socketserver.TCPServer):

    allow_reuse_address = True

    def run(self):
        try:
            self.serve_forever()
        except KeyboardInterrupt:
            pass
        finally:
            self.server_close()

def keyboardInterruptHandler(signal, frame):
    # Todo, cleanup (sometimes port is not freed up)
    exit(0)

if __name__== "__main__":

    parser = argparse.ArgumentParser(description="Web server for testing purpose only.")
    parser.add_argument("-p", "--port", dest="port", default=8080, type=int, help="Port to be used.")
    parser.add_argument("path", default=".", nargs='?', help="Serve files from this specific directory or archive, by default the current directory will be used.")

    args = parser.parse_args()

    signal.signal(signal.SIGINT, keyboardInterruptHandler)

    tempPath = None
    try:

        # If archive is set, serve files from this specific archive
        if os.path.isfile(args.path):
            tempPath = tempfile.TemporaryDirectory()
            package = tarfile.open(args.path)
            try:
                print("Extracting content of '{}' to temporary directory '{}'.".format(args.path, tempPath.name))
                package.extractall(tempPath.name)
            finally:
                package.close()
            os.chdir(tempPath.name)

        handler = http.server.SimpleHTTPRequestHandler
        server = WebServer(("", args.port), handler)
        print("Web server ready, serving '{}' at 'http://localhost:{}'.".format(args.path, args.port))
        server.run()

    # Cleanup RAII style
    finally:
        if tempPath:
            print("Cleaning up temporary directory '{}'.".format(tempPath.name))
            tempPath.cleanup()
