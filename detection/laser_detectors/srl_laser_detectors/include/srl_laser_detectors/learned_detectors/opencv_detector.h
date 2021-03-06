#ifndef _SRL_LASER_DETECTORS_OPENCV_DETECTOR_H
#define _SRL_LASER_DETECTORS_OPENCV_DETECTOR_H

#include <srl_laser_detectors/learned_detector.h>
#include <srl_laser_features/features/feature.h>
#include <opencv2/ml/ml.hpp>



namespace srl_laser_detectors {

/// Base class for a learned detector based upon the OpenCV Machine Learning (ML) library.
class OpenCvDetector : public LearnedDetector {
public:
    /// Constructor.
    OpenCvDetector(ros::NodeHandle& nodeHandle, ros::NodeHandle& privateNodeHandle);

    /// Detects persons from laser segments. The result is returned as a list of labels and confidences, of the same length as the input list of segments.
    /// Labels must be initialized with all BACKGROUND and confidences must all be set to 1.0 by the caller.
    /// If the detector is non-probabilistic, the returned confidences will be all 1.0.
    virtual void detect(const Segments& segments, Labels& labels, Confidences& confidences);

    /// Trains the detector on the given annotated groundtruth segments, which should contain both positive and negative samples.
    virtual void train(const Segments& segments, const Labels& labels);

    /// Loads a learned model from file. Returns false if loading failed.
    /// If no model filename is specified, it is read from ROS parameter server (_model).
    virtual bool loadModel(const std::string& filename = "");

    /// Saves a learned model to file. Returns false if saving failed.
    virtual bool saveModel(const std::string& filename);


protected:
    /// Unique name identifying the type of the detector.
    virtual const std::string getName() = 0;

    /// Get the underlying statistical model (base interface of all OpenCV classifiers). Must not be NULL.
    virtual CvStatModel* getStatModel() = 0;

    /// Classifies the feature vector of a single segment using the learned model. Returned confidence should be always 1.0 if classifier is non-probabilistic.
    virtual void classifyFeatureVector(const cv::Mat& featureVector, Label& label, double& confidence) = 0;

    /// Trains the classifier on the provided matrix of feature values (one row per segment), and the corresponding vector of segment labels.
    virtual void trainOnFeatures(const cv::Mat& featureMatrix, const cv::Mat& labelVector) = 0;

    /// Creates a binary mask of size numSamples x 1 where all samples with NaN or Inf values are masked out (=0).
    virtual cv::Mat maskSamplesWithNonfiniteValues(const cv::Mat& featureMatrix);


private:
    /// Computes a matrix of feature values (one row per segment) for the given segments.
    /// The number of columns corresponds to the total number of feature dimensions (as each feature can have multiple of them).
    virtual cv::Mat calculateFeatureMatrix(const Segments& segments);

    /// Dump feature matrix into file, for analysis e.g. with Matlab
    virtual void dumpFeatureMatrix(const cv::Mat& featureMatrix, const Labels& labels);

    /// The features that are/were used to train this classifier. One feature can have multiple dimensions.
    srl_laser_features::Features m_features;
    std::vector<srl_laser_features::FeatureDimension> m_featureDimensions; /// list of all dimensions of all used features
};


} // end of namespace srl_laser_detectors

#endif // _SRL_LASER_DETECTORS_OPENCV_DETECTOR_H
