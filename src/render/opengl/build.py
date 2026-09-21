#!/bin/python
import argparse
from functools import partial
import subprocess
import sys
from pathlib import Path
import os
import json
from http import server
import threading
import webbrowser


SCRIPT_DIR = Path(__file__).resolve()
ROOT_DIR = SCRIPT_DIR.parent
BUILD_DIR = ROOT_DIR / "build"

CACHE_FILE = ".build_cache.json"

def load_cache():
    """ Loads argument cache from cache fiel"""
    cache = ROOT_DIR / CACHE_FILE
    if cache.exists():
        try:
            with open(cache,"r") as f:
                print("Build cache found!")
                return json.load(f)
        except json.JSONDecodeError:
            return {}
    return {}

def save_cache(args):
    """ Store cache data"""

    cache_data = {
            "debug" : getattr(args,"debug",False),
            "skipconfig" : getattr(args,"skipconfig",False),
            "parallel" : getattr(args,"parallel",False),
            "run" : getattr(args,"run",None),
            "web" : getattr(args,"web",False),
            "port" : getattr(args,"port","8080"),
            }
    cache = ROOT_DIR / CACHE_FILE

    with open(cache,"w") as f:
        json.dump(cache_data,f)

def run_cmd(cmd,cwd=ROOT_DIR):
    """ Run commands """
    if cmd[0] != "echo": # don't echo echos
        print(f"Executing: {' '.join(cmd)}")
    result  = subprocess.run(cmd,cwd=cwd)
    if result.returncode != 0:
        print(f"Error: Command failed with code {result.returncode}")
        sys.exit(result.returncode)


def browse(port=8080,dir=BUILD_DIR):
    """ open build in web. Require web build """
    browser = webbrowser.get();
    if (browser != None):
        print(f"Browser found: {webbrowser.get().name}")
    else:
        print("No browser found")

    print(f"browse_dir: {Path(dir).relative_to(ROOT_DIR)}")

    print(f"Launching server on port {port} at dir {dir}")

    url = str(f"http://localhost:{port}")
    if webbrowser.open(url):
        print(f'opening {url}')
    else:
        print("couldn't open web browser")

    server_directory(port,dir)

def server_directory(port:int,dir:Path)   :
    handler_class= partial(server.SimpleHTTPRequestHandler,directory=dir)
    httpd = server.HTTPServer(("",port), handler_class)

    try:
        print(f"Serving started {port}. Press Ctrl+C to stop.")
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down server...")
    finally:
        httpd.shutdown()
        httpd.server_close()
        print("Server closed.")



def build_cmake(args):

    """ Builds project using cmake """
    config_command= ["cmake", "-B" ,str(BUILD_DIR)]
    build_command= ["cmake", "--build" ,str(BUILD_DIR)]
    run_command= []

    if args.norun:
        args.run = None

    if args.noskip:
        args.skip = False

    if args.noparallel:
        args.parallel = False

    if args.nodebug:
        args.debug = False

    if args.noweb:
        args.web = False


    if args.debug:
        config_command.append("-DCMAKE_BUILD_TYPE=Debug")
    else:
        config_command.append("-DCMAKE_BUILD_TYPE=Release")

    if args.web:
        config_command.insert(0,"emcmake")

    if args.skipconfig:
        config_command = ["echo","skipping config"]

    if args.parallel:
        build_command.append("--parallel")

    if args.run == None:
        run_command = ["echo","No target to run"]
    else:
        root_path = ROOT_DIR / args.run
        build_path = BUILD_DIR / args.run

        if root_path.exists():
            run_command = [str(root_path)]
        elif build_path.exists():
            run_command = [str(build_path)]
        elif Path(args.run).exists(): # Absolute
            run_command = [str(args.run)]
        else:
            print(f"Unable to locate run target {args.run}")
            args.run = None
            # sys.exit(1)


    save_cache(args)

    run_cmd(config_command)
    run_cmd(build_command)
    if args.web:
        browser_path = Path(run_command[0])
        if not browser_path.exists():
            print(f"Could not find path: {browser_path}")
            sys.exit(1)
        if browser_path.is_file():
            browser_path = browser_path.parent

        browse(int(args.port),browser_path)
    else:
        run_cmd(run_command)

def clean_build(args):
    """ Clean """
    run_cmd(["rm","-rf", str(BUILD_DIR)])


def main():
    cache_values = load_cache()
    parser = argparse.ArgumentParser("Build & Automation tool");
    sub_parser = parser.add_subparsers(dest="command",required=True)

    build_parser = sub_parser.add_parser("build",help="Build the project")
    _ = build_parser.add_argument("--debug",action="store_true",help="Build in Debug mode",default=cache_values.get("debug",False))
    _ = build_parser.add_argument("--web",action="store_true",help="Build in using emscripten",default=cache_values.get("web",False))
    _ = build_parser.add_argument("--skipconfig",action="store_true",help="Skip cmake config step. Will fail if not configured",default=cache_values.get("skipconfig",False))
    _ = build_parser.add_argument("--parallel",action="store_true",help="Build in parallel",default=cache_values.get("parallel",False))
    _ = build_parser.add_argument("--run",help="run the result",default=cache_values.get("run",None))
    _ = build_parser.add_argument("--port",help="set port to launch web server",default=cache_values.get("port","8080"))

    _ = build_parser.add_argument("--nodebug",action="store_true",help="default back to release",default=False)
    _ = build_parser.add_argument("--noweb",action="store_true",help="stop using emscripten",default=False)
    _ = build_parser.add_argument("--noparallel",action="store_true",help="don't parallel",default=False)
    _ = build_parser.add_argument("--noskip",action="store_true",help="don't skip config",default=False)
    _ = build_parser.add_argument("--norun",action="store_true",help="don't run result",default=False)

    build_parser.set_defaults(func=build_cmake)

    clean_parser = sub_parser.add_parser("clean",help="Clean build artifacts")
    clean_parser.set_defaults(func=clean_build)


    args = parser.parse_args()
    args.func(args)

if __name__ == "__main__":
    main()
