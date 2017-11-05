#include "fatiguedetector.h"

FatigueDetector::FatigueDetector(QObject *parent) :
    QObject(parent),
    m_movAvg(15)
{

}

bool FatigueDetector::loadPredictorData(const QString filename)
{
    bool success = true;

    try
    {
        m_faceDetector.loadPredictorData(filename);
    }
    catch (...)
    {
        success = false;
    }

    emit predictorDataLoaded(success);
    return success;
}

bool FatigueDetector::openCamera(int index)
{
    m_cap.open(index);
    bool success = m_cap.isOpened();

    emit cameraOpened(success);
    return success;
}

void FatigueDetector::detect()
{
    cv::Mat frame;
    m_cap >> frame;

    cv::pyrDown(frame, frame);
    auto faces = m_faceDetector.detect(frame);

    FatigueDetectorStat stat;
    bool success = false;

    for (auto face : faces)
    {
        auto leftEye = Utils::sliceVect<cv::Point>(face, FL68_LEFT_EYE_START_INDEX, FL68_LEFT_EYE_END_INDEX);
        auto rightEye = Utils::sliceVect<cv::Point>(face, FL68_RIGHT_EYE_START_INDEX, FL68_RIGHT_EYE_END_INDEX);

        double leftEAR = calcEAR(leftEye);
        double rightEAR = calcEAR(rightEye);
        double rawEAR = (leftEAR + rightEAR) / 2.0;
        m_movAvg.push(rawEAR);
        double EAR = m_movAvg.get();
        std::string strEAR = "EAR = " + std::to_string(EAR);
        cv::putText(frame, strEAR, cv::Point(5, 15), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1);
        Utils::drawPoly(frame, leftEye, cv::Scalar(255, 0, 0), 1);
        Utils::drawPoly(frame, rightEye, cv::Scalar(255, 0, 0), 1);

        success = true;
        stat.rawEAR = rawEAR;
        stat.avgEAR = EAR;
        break;
    }

#ifdef APP_TYPE_GUI
    cv::imshow("Frame", frame);
    cv::waitKey(1);
#endif
    emit detected(success, stat);
}

double FatigueDetector::calcEAR(const QVector<cv::Point> &eye)
{
    Q_ASSERT(eye.size() >= 6);
    return (Utils::euclDist(eye[1], eye[5]) + Utils::euclDist(eye[2], eye[4])) / (2.0 * Utils::euclDist(eye[0], eye[3]));
}
