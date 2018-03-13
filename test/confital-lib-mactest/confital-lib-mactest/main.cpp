//
//  main.cpp
//  many-tutor-test
//
//  Created by Chris Morgan on 16/1/18.
//  Copyright © 2018 Chris Morgan. All rights reserved.
//

// Example showing how to read and write images
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "MTDetector.h"

#define DETECTION_MODE MT_EDGE_DETECT_TREES
//#define DETECTION_MODE MT_EDGE_DETECT_CANNY

const char* IMAGE_PATHS[] = {
    "messed.png",
    "skewed-nolines.png",
    "skewed-nolines2.png",
    "skewed-failed-to-detect-edge.png",
    "skewed1.png",
    "table-detect-half-page.png",
    "table-failed-to-detect.png",
    "table-failed-to-detect2.png",
    "table-no-detect3.png"
};

const size_t NUM_IMAGES = sizeof(IMAGE_PATHS) / sizeof(const char*);
const char* IMAGE_DIR = "../../../../../test/images";
const char* SAVED_DIR = "../../../../../test/images_saved";
const char* MODEL_FILE = "../../../../../src/model.yml.gz";
const int MAX_DIFFERENCE_BETWEEN_POINTS = 10;

const char* RESULTS_FILE_NAME = "../../../../../test-results-canny.csv";

typedef std::vector<cv::Point> Points;

class DetectionResult {
public:
    DetectionResult(const char* _name, const Points& _points) :
        name(_name), points(_points) {}
    
    DetectionResult(std::ifstream& stream) {
        std::string x;
        std::string y;
        
        for (int i = 0;i < 4;i++) {
            std::getline(stream, x, ',');
            std::getline(stream, y, ',');
            if (stream.eof())
                return;
        
            add_point(x, y);
        }
        
        std::getline(stream, name);
    }
    
    bool compare(const Points& test_points) const {
        //if (test_points.size() != points.size())
         //   return false;
        
        for (int i = 0;i < points.size();i++) {
            if (!points_identical(test_points[i], points[i]))
                return false;
        }
        
        return true;
    }
    
    void overwrite_points(const Points& new_points) {
        points = new_points;
    }
    
    void write(std::ostream& out) {
        for (int i = 0;i < points.size();i++) {
            out << points[i].x << "," << points[i].y << ",";
        }
        
        out << name << "\n";
    }
    
    bool is_valid() const {
        return (points.size() == 4 && name.size() != 0);
    }
    
private :
    bool points_identical(const cv::Point& point1, const cv::Point& point2) const {
        if (abs(point1.x - point2.x) > MAX_DIFFERENCE_BETWEEN_POINTS)
            return false;
        
        if (abs(point1.y - point2.y) > MAX_DIFFERENCE_BETWEEN_POINTS)
            return false;
        
        return true;
    }
    
    void add_point(const std::string& x, const std::string& y) {
        cv::Point point = cv::Point(std::stoi(x), std::stoi(y));
        points.push_back(point);
    }
    
    std::string name;
    Points points;
};

class DetectionResults {
public :
    DetectionResults() {
        read_saved_results();
    }
    
    bool has_result(int index) {
        return !(index < 0 || index >= results.size());
    }
    
    DetectionResult get(int index) {
        return results[index];
    }
    
private :
    void read_saved_results() {
        std::ifstream results_file(RESULTS_FILE_NAME);
        if (!results_file) {
            std::cerr << "Could not open results file " << RESULTS_FILE_NAME;
            return;
        }
        
        while (!results_file.eof()) {
            DetectionResult result = DetectionResult(results_file);
            if (result.is_valid())
                results.push_back(result);
        }
    }
    
    std::vector<DetectionResult> results;
};

class Test {
public :
    Test(bool _save_images) : fails(0), save_images(_save_images) {
        load_images();
        detector.initialize(MODEL_FILE);
    }
    
    void test_all() {
        for (int i = 0;i < images.size();i++) {
            test_one(i);
        }
    }
    
    void test_one(int ordinal) {
        srand(42);
        
        detector.reset();
        
        cv::Mat test_image;
        images[ordinal].copyTo(test_image);
        
        detector.processImage(test_image, DETECTION_MODE);
        
        check_detection(ordinal);
        
        get_document_image(ordinal);
        
        if (save_images) {
            save_detected_image(test_image, ordinal);
        }
    }
    
    cv::Mat get_document_image(int ordinal) {
        cv::Mat result;
        detector.getDocumentImage(result);
        return result;
    }
    
    int get_number_fails() const {
        return fails;
    }
    
    void report_fails() const {
        std::cout << "Completed with " << get_number_fails() << " fails.\n";
    }
    
private :
    void load_images() {
        for (int i = 0;i < NUM_IMAGES;i++) {
            char path[0x100];
            cv::Mat image;
            sprintf(path, "%s/%s", IMAGE_DIR, IMAGE_PATHS[i]);
            image = cv::imread(path);
            images.push_back(image);
        }
    }
    
    void check_detection(int ordinal) {
        Points points;
        detector.getDocumentPoints(points);
        
        if (saved_results.has_result(ordinal)) {
            DetectionResult detection_result = saved_results.get(ordinal);
            if (!detection_result.compare(points)) {
                std::cout << "Test failed for image " \
                << IMAGE_PATHS[ordinal] << "\n";
                fails++;
            }
            
        } else {
            DetectionResult detection_result = DetectionResult(
                                            IMAGE_PATHS[ordinal], points);
            detection_result.write(std::cout);
        }
    }
    
    void save_detected_image(const cv::Mat& image, int ordinal) const {
        char path[0x100];
        sprintf(path, "%s/%s", SAVED_DIR, IMAGE_PATHS[ordinal]);
        cv::imwrite(path, image);
    }
    
    std::vector<cv::Mat> images;
    DetectionResults saved_results;
    MTDetector detector;
    int fails;
    bool save_images;
};

void help() {
    std::cout \
    << "Usage:\n" \
    << "    -c <number> : Multiple passes through images, for speed tests\n" \
    << "    -n <number> : Test only image number\n";
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        Test tester = Test(true);
        tester.test_all();
        tester.report_fails();
    }
    else if (argc != 3) {
        help();
    }
    else {
        const char* command = argv[1];
        int number = std::stoi(argv[2]);
        
        if (strcmp(command,"-c")) {
            Test tester = Test(false);
            for (int i = 0; i < number;i++) {
                tester.test_all();
            }
        }
        else if (strcmp(command,"-n")) {
            Test tester = Test(true);
            tester.test_one(number);
            tester.report_fails();
        }
        else {
            help();
        }
    }
    
    return 0;
}




