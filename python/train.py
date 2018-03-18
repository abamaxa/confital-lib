import confital
from PIL import Image

def main() :
    detector = confital.PyDetector()
    
    image = Image.open("test.jpg")
    
    markedup_image = detector.process_image(image)
    
    print("Found document:", detector.found_document())
    
    points = detector.get_document_points()
    print(points)
        
    if detector.found_document() :
        doc_image = detector.copy_deskewed_doc_region(image)
        doc_image.save("detected-document.jpg")

if __name__ == "__main__" :
    main()