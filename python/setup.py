import os
from distutils.core import setup

from distutils.extension import Extension
from Cython.Build import cythonize
import numpy
import subprocess
 
#proc_libs = subprocess.check_output("pkg-config --libs opencv".split())
#proc_incs = subprocess.check_output("pkg-config --cflags opencv".split())
#libs = [lib for lib in str(proc_libs, "utf-8").split()]

sourcefiles = [
"confital.pyx",
"../src/pipeline_job.cc",
"../src/algorithms/gather_lines.cc",
"../src/algorithms/edge_detector.cc",
"../src/algorithms/best_rectangle.cc",
"../src/algorithms/line_detector.cc",
"../src/algorithms/debug_drawings.cc",
"../src/rectangle.cc",
"../src/document.cc",
"../src/line.cc",
"../src/detector.cc"
]

libraries = [
"opencv_imgcodecs.3.4",
"opencv_imgproc.3.4",
"opencv_core.3.4",
"opencv_ximgproc.3.4",
]

include= [
numpy.get_include(), 
"/usr/local/include",
os.path.realpath("../src"),
os.path.realpath("../src/algorithms")
]

extensions = [
    Extension("confital", sourcefiles,
        include_dirs = include,
        language="c++",
        libraries = libraries,
        library_dirs = ["/usr/local/lib"]),
]
setup(
    name = "Confital",
    ext_modules = cythonize(extensions),
)
