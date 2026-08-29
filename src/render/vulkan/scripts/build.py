#!/bin/python3

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent.resolve()
ROOT_DIR = SCRIPT_DIR.parent
BUILD_DIR = ROOT_DIR / "build"

def run_cmd(cmd,cwd=ROOT_DIR):
    """run shell commands cleanly"""
    print(f"--> Executing: {' '.join(cmd)})")
    result  = subprocess.run(cmd,cwd=cwd)
    if result.returncode != 0:
        print(f"Error: Command failed with code {result.returncode}")
        sys.exit(result.returncode)

def action_clean(args):
    pass

def action_build(args):
    """Configure Built the project"""
    config = "Debug" if args.debug else "Release"
    print(f"Building project ({config})")
    if not args.noconfig:
        run_cmd(["cmake","-B", str(BUILD_DIR), f"-DCMAKE_BUILD_TYPE={config}"])
    run_cmd(["cmake","--build", str(BUILD_DIR), "--parallel"])

def action_run(args):
    """Build (if needed) and run the executable."""
    if not BUILD_DIR.exists() or args.rebuild:
        action_build(args)

    exe_path = BUILD_DIR / "VulkanRenderer/VulkanRenderer.exe"
    run_cmd([str(exe_path)])

def main():

    parser = argparse.ArgumentParser(description="Build & Automation Tool")
    subparsers = parser.add_subparsers(dest="command",required=True)

    build_parser = subparsers.add_parser("build", help="Compile the project")
    _ = build_parser.add_argument("--debug",action="store_true", help="Build in Debug mode")
    _ = build_parser.add_argument("--noconfig",action="store_true", help="Build without Configuring")
    build_parser.set_defaults(func=action_build)

    run_parser = subparsers.add_parser("run", help="Run the build")
    _ = run_parser.add_argument("--debug",action="store_true", help="Build in Debug mode")
    _ = run_parser.add_argument("--rebuild",action="store_true", help="Force rebuild before running")
    _ = run_parser.add_argument("--noconfig",action="store_true", help="Build without Configuring")
    run_parser.set_defaults(func=action_run)


    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
