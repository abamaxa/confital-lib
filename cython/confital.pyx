import numpy as np
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
        
    def found_document(self) :
        cdef bool result
        result = self.c_detector.found_document()
        return result
        
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