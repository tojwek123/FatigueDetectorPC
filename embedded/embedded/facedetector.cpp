#include "facedetector.h"

FaceDetector::FaceDetector()
{
    m_detector = dlib::get_frontal_face_detector();
}

void FaceDetector::loadPredictorData(const QString &predictorFile)
{
    dlib::deserialize(predictorFile.toStdString()) >> m_predictor;
}

QVector<QVector<Point>> FaceDetector::detect(const cv::Mat &im)
{
    QVector<QVector<Point>> faces;

    dlib::cv_image<dlib::bgr_pixel> wrappedIm(im);
    auto rects = m_detector(wrappedIm, 0);

    for (auto rect : rects)
    {
        QVector<Point> face;
        auto objDetect = m_predictor(wrappedIm, rect);

        for (auto i = 0u; i < objDetect.num_parts(); ++i)
        {
            auto part = objDetect.part(i);
            face.append(Point(part.x(), part.y()));
        }

        faces.append(face);
    }
    return faces;
}
