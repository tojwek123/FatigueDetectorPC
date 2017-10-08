#include <iostream>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include "facedetector.h"
#include "facelandmark68.h"
#include "utils.h"
#include <cmath>

const std::string PredictorFile = "C:\\Dev\\dlib-19.7\\resources\\shape_predictor_68_face_landmarks.dat";

double euclDist(const cv::Point &a, const cv::Point &b)
{
    cv::Point diff = a - b;
    return sqrt(diff.x * diff.x + diff.y * diff.y);
}

double calcEAR(const std::vector<cv::Point> &eye)
{
    return (euclDist(eye[1], eye[5]) + euclDist(eye[2], eye[4]) / (2 * euclDist(eye[0], eye[3])));
}

void imProcThread();
void consoleInputThread();

int main()
{
    FaceDetector detector;


}

void imProcThread()
{
    try
    {
        detector.loadPredictorData(PredictorFile);
    }
    catch (...)
    {
        std::cerr << "Cannot load predictor data file." << std::endl;;
        return -1;
    }

    cv::VideoCapture cap(0);
    cv::Mat frame;

    while (cv::waitKey(30) != 'q')
    {
        cap >> frame;
        cv::pyrDown(frame, frame);

        auto faces = detector.detect(frame);

        for (auto face : faces)
        {
            auto leftEye = Utils::sliceVect<cv::Point>(face, FL68_LEFT_EYE_START_INDEX, FL68_LEFT_EYE_END_INDEX);
            auto rightEye = Utils::sliceVect<cv::Point>(face, FL68_RIGHT_EYE_START_INDEX, FL68_RIGHT_EYE_END_INDEX);

            double leftEAR = calcEAR(leftEye);
            double rightEAR = calcEAR(rightEye);
            double EAR = (leftEAR + rightEAR) / 2.0;

            //std::cout << EAR << std::endl;

            Utils::drawPoly(frame, leftEye, cv::Scalar(255, 0, 0), 1);
            Utils::drawPoly(frame, rightEye, cv::Scalar(255, 0, 0), 1);
        }

        imshow("Frame", frame);
    }
}
