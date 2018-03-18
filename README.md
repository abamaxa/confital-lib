# Confital-Lib
---
Confital-Lib is an open source software library for detecting documents in images. The library provides a methods for analyzing an image and returning the pixel coordinates of any document detected within the image. The library also provides a function to extract a deskwed image of the detected document from the original image. The library expects the document to be the dominant feature of the image.

The library is implemented with OpenCV version 3 and is intended primarily for use on mobile devices and embedded systems. It is written in C++ and also provides a Python interface. To date, the code has been compiled and tested with OpenCV version 3.4 on Android 5.x-7.x, iOS 10.x-11.x, MacOS 10.13 and Ubuntu Linux 17.10.

## Installation on MacOS

----

In order to build the library, you will need to install OpenCV 3 first, including the contrib modules. The easiest way to do this is from the terminal using [brew](https://brew.sh/).

If you want python3 support you need to install that first, unless you have already done so.

```
$ brew install python3
```
Followed by
```
$ brew install opencv --c++11 --with-contrib --with-python3
```
However, if you don't want python support, skip this step and just install OpenCV 3:
```
$ brew install opencv --c++11 --with-contrib
```
There is an Xcode project for building the library in the /mac subdirectory - just open it up in Xcode, select the library build and compile.

If you want to build the python3 support, open a terminal and execute the following commands:

```
$ cd python
$ pip3 install -r requirements.txt
$ python setup.py build_ext --inplace
$ python setup.py install
```

## Usage
----
The library is accessed through a class called Detector which provides methods to detect a document and returns the pixel coordinates of the document, if one was detected. For example the following python code will open an image called test.png, detect

```
import confital
from PIL import Image

# Create an instance of the detector
detector = confital.PyDetector()

# Open an image from the test images using PIL/Pillow
image = Image.open("test.jpg")

print("Found document:", detector.found_document())

if detector.found_document() :
    # Get the coordinates of the detected document
    points = detector.get_document_points()
    print(points)

    # Extract and deskew the detected document from the
    # original image and save it.
    doc_image = detector.copy_deskewed_doc_region(image)
    doc_image.save("detected-document.jpg")
```

The result should be this:

<table>
<tr>
<td>Original Image</td><td>Detected Document</td>
</tr>
<tr>
<td>
  <img src="https://github.com/abamaxa/confital-lib/blob/master/python/test.jpg?raw=true" width="400px", height="600px">
</td>
<td>
  <img src="https://raw.githubusercontent.com/abamaxa/confital-lib/master/python/detected-document.jpg" width="400px", height="600px">
</td>
</tr>
</table>
