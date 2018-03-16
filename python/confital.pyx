# Build with
# python setup.py build_ext --inplace
# from http://makerwannabe.blogspot.com.es/2013/09/calling-opencv-functions-via-cython.html

import numpy as np
from PIL import Image

cimport numpy as np # for np.ndarray
from libcpp.string cimport string
from libc.string cimport memcpy
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "opencv2/core/core.hpp":
  cdef int  CV_WINDOW_AUTOSIZE
  cdef int CV_8UC3
 
cdef extern from "opencv2/core/core.hpp" namespace "cv":
  cdef cppclass Mat:
    Mat() except +
    void create(int, int, int)
    void* data
    int rows
    int cols
    int channels()

cdef extern from "opencv2/core/core.hpp" namespace "cv":
  cdef cppclass Point:
    Point() except +
    Point(int, int) except +
    int x
    int y

cdef void ary2cvMat(np.ndarray ary, Mat& out):
  assert(ary.ndim==3 and ary.shape[2]==3, "ASSERT::3channel RGB only!!")
  ary = np.dstack((ary[...,2], ary[...,1], ary[...,0])) #RGB -> BGR
   
  cdef np.ndarray[np.uint8_t, ndim=3, mode = 'c'] np_buff = np.ascontiguousarray(ary, dtype = np.uint8)
  cdef unsigned int* im_buff = <unsigned int*> np_buff.data
  cdef int r = ary.shape[0]
  cdef int c = ary.shape[1]
  out.create(r, c, CV_8UC3)
  memcpy(out.data, im_buff, r*c*3)

# from https://github.com/solivr/cython_opencvMat/blob/master/opencv_mat.pyx
cdef extern from "Python.h":
    ctypedef struct PyObject
    object PyMemoryView_FromBuffer(Py_buffer *view)
    int PyBuffer_FillInfo(Py_buffer *view, PyObject *obj, void *buf, Py_ssize_t len, int readonly, int infoflags)
    enum:
        PyBUF_FULL_RO
        
cdef object Mat2np(Mat m):
    # Create buffer to transfer data from m.data
    cdef Py_buffer buf_info
    # Define the size / len of data
    cdef size_t len = m.rows*m.cols*m.channels()*sizeof(CV_8UC3)
    # Fill buffer
    PyBuffer_FillInfo(&buf_info, NULL, m.data, len, 1, PyBUF_FULL_RO)
    # Get Pyobject from buffer data
    Pydata  = PyMemoryView_FromBuffer(&buf_info)

    # Create ndarray with data
    shape_array = (m.rows, m.cols, m.channels())
    ary = np.ndarray(shape=shape_array, buffer=Pydata, order='c', dtype=np.uint8)

    # BGR -> RGB
    ary = np.dstack((ary[...,2], ary[...,1], ary[...,0]))
    # Convert to numpy array
    pyarr = np.asarray(ary)
    return pyarr
    
    
cdef extern from "detector.h" :
    cdef cppclass Detector:
        Detector() except +
        void process_image(Mat&)
        bool found_document() const
        bool copy_deskewed_doc_region(Mat&, Mat&)
        void get_document_points(vector[Point]&) const
        void reset()

cdef class PyDetector:
    cdef Detector c_detector      # hold a C++ instance which we're wrapping
    def __cinit__(self):
        self.c_detector = Detector()
        
    def process_image(self, image):
        cdef Mat m
        ary2cvMat(np.array(image), m)
        self.c_detector.process_image(m)
        return Image.fromarray(Mat2np(m).astype('uint8'))
        
    def found_document(self) :
        cdef bool result
        result = self.c_detector.found_document()
        return result
        
    def copy_deskewed_doc_region(self, src_image) :
        cdef Mat src_m
        cdef Mat dest_m
        
        if self.found_document() :
            ary2cvMat(np.array(src_image), src_m)
            self.c_detector.copy_deskewed_doc_region(src_m, dest_m)
            return Image.fromarray(Mat2np(dest_m).astype('uint8'))
        
    def get_document_points(self) :
        cdef vector[Point] points
        cdef Point point
        
        self.c_detector.get_document_points(points)
        py_points = []
        for i in range(points.size()) :
            point = points.at(i)
            py_points.append((point.x, point.y))
            
        return py_points
    
    def reset(self) :
         self.c_detector.reset()