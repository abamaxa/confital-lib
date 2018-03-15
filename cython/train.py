import confital
from PIL import Image

def main() :
    detector = confital.PyDetector()
    
    image = Image.open("../data/images/messed.png")
    
    detector.process_image(image)
    
    print("Found document:", detector.found_document())
    
    points = detector.get_document_points()
    print(points)

if __name__ == "__main__" :
    main()