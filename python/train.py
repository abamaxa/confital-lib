import confital
from PIL import Image

def main() :
    # Create an instance of the detector
    detector = confital.PyDetector()
    
    # Open an image from the test images using PIL/Pillow
    image = Image.open("test.jpg")
    
    # The detect method attempt to detect a document within the image.
    # The original image is unchanged but the method returns an images
    # with a green rectangle drawn around the document, if one was detected.
    image_with_detection_rectangle = detector.detect(image)

    if detector.found_document() :
        # Get the coordinates of the detected document
        points = detector.get_document_points()
        print(points)
    
        # Extract and deskew the detected document from the
        # original image and save it.
        doc_image = detector.copy_deskewed_doc_region(image)
        doc_image.save("detected-document.jpg")

if __name__ == "__main__" :
    main()