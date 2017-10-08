#include "facedetector.h"

FaceDetector::FaceDetector()
{
    m_detector = dlib::get_frontal_face_detector();
}

void FaceDetector::loadPredictorData(std::string predictorFile)
{
    dlib::deserialize(predictorFile) >> m_predictor;
}

vector<vector<Point>> FaceDetector::detect(const cv::Mat &im)
{
    vector<vector<Point>> faces;

    dlib::cv_image<dlib::bgr_pixel> wrappedIm(im);
    auto rects = m_detector(wrappedIm, 0);

    for (auto rect : rects)
    {
        vector<Point> face;
        auto objDetect = m_predictor(wrappedIm, rect);

        for (auto i = 0u; i < objDetect.num_parts(); ++i)
        {
            auto part = objDetect.part(i);
            face.push_back(Point(part.x(), part.y()));
        }

        faces.push_back(face);
    }
    return faces;
}
