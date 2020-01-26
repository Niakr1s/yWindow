from pathlib import Path, PurePath
import os
import shutil
import zipfile
import sys

PROJECT_NAME = "yWindow"
ROOT_DIR = "release"

CMAKE_CONFIG = "MinSizeRel"
CMAKE_CONFIG_CMD = "--config "+CMAKE_CONFIG+" -j4"

X86_ARCH = "x86"
X64_ARCH = "x64"

CMAKE_ARCH = {X86_ARCH: "Win32", X64_ARCH: "x64"}


class Architecture:
    def __init__(self, arch=X86_ARCH):
        self.arch = arch
        self.build_dir = ROOT_DIR + "/" + arch + "_build"
        self.cmake_pre_cmd = 'cmake . -B "' + \
            self.build_dir + '" -A ' + CMAKE_ARCH[arch]
        self.cmake_cmd = 'cmake --build "' + self.build_dir + '" ' + CMAKE_CONFIG_CMD
        self.actual_build_dir = self.build_dir + "/bin/" + CMAKE_CONFIG + "/"
        self.included_files = [(self.actual_build_dir + "jsoncpp.dll", "jsoncpp.dll"),
                               (self.actual_build_dir + "yWindow.dll", "yWindow.dll"), ]
        self.included_dirs = [(self.build_dir + "/bin/yWindow", "yWindow"), ]


def clean():
    if Path(ROOT_DIR).exists():
        print("removing " + ROOT_DIR + " dir")
        shutil.rmtree(ROOT_DIR)


def cmake(architecture, clean=False):
    print(architecture.cmake_pre_cmd)
    os.system(architecture.cmake_pre_cmd)
    print(architecture.cmake_cmd)
    os.system(architecture.cmake_cmd)


def findFile(src, filename):
    for dirpath, _, filenames in os.walk(src):
        for file in filenames:
            if file.lower() == filename:
                print("Found " + filename + " in " + dirpath)
                return dirpath + "\\" + file
    print(filename + " not found")
    quit()


def addToZip(architecture, version=""):
    zipPath = ROOT_DIR + "/" + PROJECT_NAME + "_" +\
        architecture.arch + "_" + version + ".zip"
    if Path(zipPath).exists():
        os.remove(zipPath)
    with zipfile.ZipFile(zipPath, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=9) as releaseZip:
        for included_dir in architecture.included_dirs:
            for dirpath, _, filenames in os.walk(included_dir[0]):
                for filename in filenames:
                    fullPath = dirpath + "/" + filename
                    fullPathWithoutReleaseDir = fullPath.replace(
                        included_dir[0], included_dir[1])
                    print("Adding " + fullPath + " to zip")
                    releaseZip.write(dirpath + "/" + filename,
                                     fullPathWithoutReleaseDir)
        for toZip in architecture.included_files:
            print("Adding " + toZip[0] + " to zip " + toZip[1])
            releaseZip.write(toZip[0], toZip[1])
    print('Zip created in "' + zipPath + '"')


def getProjectVersion(architecture):
    versionPath = findFile(architecture.build_dir + "/a", "version.h")
    if versionPath == "":
        return
    with open(versionPath) as f:
        for line in f:
            if (line.find("PROJECT_VER") != -1):
                projectVersion = line.split('"')[1]
                print("Got Project Version = " + projectVersion)
                return projectVersion
    return


if __name__ == "__main__":
    x86 = Architecture(X86_ARCH)
    x64 = Architecture(X64_ARCH)

    # clean()

    cmake(x86)
    cmake(x64)

    addToZip(x86, getProjectVersion(x86))
    addToZip(x64, getProjectVersion(x64))
