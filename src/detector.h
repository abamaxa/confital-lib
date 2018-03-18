//  Created by Chris Morgan on 22/12/17.
//  Copyright © 2017 Chris Morgan. All rights reserved.

#ifndef Detector_h
#define Detector_h

#ifdef __cplusplus
#include "common.h"
#include "config.h"
#include "document.h"
#include "algorithms/algorithms.h"

enum EDGE_DETECTORS {
    MT_EDGE_DETECT_CANNY,
    MT_EDGE_DETECT_TREES
};

typedef std::shared_ptr<AbstractAlgorithm> Algorithm;
typedef std::vector<std::shared_ptr<AbstractAlgorithm>> AlgorithmPipeline;

class Detector {
public:
    Detector();
    virtual ~Detector();
    
    virtual void set_model_filepath(std::string model_name, std::string path);
    virtual void add_pipeline_stage(Algorithm algorithm);
    virtual void process_image(cv::Mat& original_image);
    virtual bool found_document() const;
    virtual void copy_deskewed_doc_region(cv::Mat& src_image, cv::Mat& dest_image);
    virtual void get_document_points(Points& points) const;
    virtual void draw_most_recent_detected_document(cv::Mat& original_image) const;
    virtual void reset();
    
protected:
    virtual void assemble_default_pipeline();
    
private:
    void calculate_rescale_factor(cv::Mat& original_image);
    void load_algorithm_models();
    void scale_image(cv::Mat& original_image);
    void update_detection_state(const Document& document);
    void update_last_detected_document(cv::Mat& original_image);
    
    AlgorithmPipeline pipeline;
    std::unordered_map<std::string, std::string> model_paths;
    int frame_counter;
    cv::Mat scaled_image;
    Document detected_document;
    Document most_recent_detected_document;
    int num_frames_since_doc_detected;
    int rescale_factor;
};

#endif /* __cplusplus */
#endif /* Detector_h */
