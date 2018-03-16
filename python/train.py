import confital
from PIL import Image

def main() :
    detector = confital.PyDetector()
    
    image = Image.open("../data/images/messed.png")
    
    markedup_image = detector.process_image(image)
    
    print("Found document:", detector.found_document())
    
    points = detector.get_document_points()
    print(points)
    
    markedup_image.save("markup.png")
    image.save("test.png")
    
    if detector.found_document() :
        doc_image = detector.copy_deskewed_doc_region(image)
    
        doc_image.save("region.png")

if __name__ == "__main__" :
    main()