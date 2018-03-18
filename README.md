# Confital-Lib
---
Confital-Lib is an open source software library for detecting documents in an image and is particularly suited to working with video streams.

The library provides methods for analyzing an image and returning the pixel coordinates of any document detected within the image. The library also provides a function to extract a deskewed image of the detected document from the original image.

The library expects the document to be the dominant feature of the image.

The library is implemented with OpenCV version 3 and is intended primarily for use on mobile devices and embedded systems. It is written in C++ and also provides a Python interface. To date, the code has been compiled and tested with OpenCV version 3.4 on Android 5.x-7.x, iOS 10.x-11.x, MacOS 10.13 and Ubuntu Linux 17.10.

## Installation on MacOS

----

In order to build the library, you will need to install OpenCV 3 first, including the contrib modules. The easiest way to do this is from the terminal using [brew](https://brew.sh/).

If you want python3 support you need to install that first, unless you have already done so:

```shell
$ brew install python3
```
followed by
```shell
$ brew install opencv --c++11 --with-contrib --with-python3
```
However, if you don't want python support, skip these steps and just install OpenCV 3:
```shell
$ brew install opencv --c++11 --with-contrib
```
There is an Xcode project for building the library in the /mac subdirectory - just open it up in Xcode, select the library build and compile.

If you want to build the python3 support, open a terminal and execute the following commands:
```shell
$ cd python
$ pip3 install -r requirements.txt
$ python setup.py build_ext --inplace
$ python setup.py install
```

## Python Usage
----
The library is accessed through a class called Detector which provides methods to detect a document and return the pixel coordinates of the document, if one was detected. For example the following python code will

* open an image called test.png;
* analyze it;
* report the coordinates of the detected document; and
* save the region containing the detected document to a new file.

```python
import confital
from PIL import Image

# Create an instance of the detector
detector = confital.PyDetector()

# Open an image from the test images using PIL/Pillow
image = Image.open("test.jpg")

# The detect method attempt to detect a document within the image.
# The original image is unchanged but the method returns an images
# with a green rectangle drawn around the document, if one was detected.
image_with_detection_rectangle = detector.detect(image)

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

The result should look like this:

<table>
<tr>
<td>Original Image</td><td>Detected Document</td>
</tr>
<tr>
<td>
  <img src="https://github.com/abamaxa/confital-lib/blob/master/python/test.jpg?raw=true" width="400px", height="600px">
</td>
<td>
  <img src="https://github.com/abamaxa/confital-lib/blob/master/python/detected-document.jpg?raw=true" width="400px", height="600px">
</td>
</tr>
</table>


## C++ Usage
---


The C++ interface provides greater flexibility than the python interface. In particular, it is possible to modify or extend the detection pipeline by implementing new classes that implement the library's AbstractAlgorithm interface (see src/algorithm/abstract_algorithm.h").

The C++ interface is aimed towards detecting documents in a video stream from a mobile phone's camera. In my experience, the stream of images from some cameras can vary significantly. Sometimes documents are detected on most but not all frames. In order to provide a better user experience, the detector maintains detection state so that, once a document has been detected, a frame can still be drawn around the document if it is not detected on the next frame. This reduces the chances of the detection rectangle flashing on/off as the detector fails to detect the document on some frames. This state can be reset using the reset() method.

Here is a simple example of the C++ interface processing images received through the OpenCV camera interface:

```c++
void startDetecting() {
    // Remove any state left over from previous detections
    detector.reset();
}

void processImage(cv::Mat &image) {
    // Do the detection.
    detector.detect(image);
    // If you want a green rectangle drawn round it, call this methods
    detector.highlight_most_recent_detected_document(image);
}

void captureDetectedDocument(cv::Mat &image_containing_document) {
    if (detector.found_document())
        detector.copy_deskewed_doc_region(
            image_containing_document, detected_document);
}

```

The code has been tested on Android and iOS. I intend to release sample apps to illustrate using the library on these platforms. In the meantime, here are some tips on building and using the code on iOS and Android.

## iOS
----
It is relatively straight to compile and use the code on iOS. For me, the two main issues are obtaining/building OpenCV and wrapping the C++ code so it can be called from Swift/Objective-C.

As far as I know, there is no suitable prebuilt version of the iOS opencv framework available on the net. I have one which I am willing to share but it's quite large which is why I haven't uploaded it to github - my account has a 100mb file size limit. So, just contact me if you would like me to upload it to your dropbox etc.

In order to be able to call the code from Swift/Objective-C you need to create an Objective-C++ wrapper. To do this, simply create an objective-c++ file in Xcode and wrap the library code as follows:

```objective-c
#ifdef __cplusplus
#import "detector.h"
#endif

...

#ifdef __cplusplus
- (void)processImage:(cv::Mat&)image
{
    detector.detect(image);
}
#endif
```

I also recommend looking at the OpenCV iOS examples.

## Android
----
You will need to write a JNI wrapper in order to use the library on Android. Once you have done that, make sure your Android.mk file contains the following settings:

```shell
APP_CPPFLAGS := -std=c++14
NDK_TOOLCHAIN_VERSION=clang
```
