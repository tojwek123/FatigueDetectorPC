#include "fatiguedetector.h"

FatigueDetector::FatigueDetector(const FatigueDetectorParams &params, QObject *parent) :
    QObject(parent),
    m_movAvg(params.movAvgSize),
    m_params(params)
{

}

bool FatigueDetector::loadDataFiles(const QString cascadeClassifierDataPath,
                                    const QString predictorDataPath)
{
    bool success = m_faceDetector.loadDataFiles(cascadeClassifierDataPath, predictorDataPath);
    emit predictorDataLoaded(success);
    return success;
}

bool FatigueDetector::openCamera()
{
    bool success = m_grabber.open();
    emit cameraOpened(success);
    return success;
}

void FatigueDetector::setParams(const FatigueDetectorParams params)
{
    m_params = params;
    m_movAvg.setWinSize(params.movAvgSize);
}

void FatigueDetector::detect()
{
    cv::Mat frame;
    m_grabber.grab();
    m_grabber.retrieve(frame);

    QVector<cv::Rect> boundingBoxes;
    QVector<QVector<cv::Point>> faces;
    m_faceDetector.detect(frame, boundingBoxes, faces);

    FatigueDetectorStat stat;

    for (int i = 0; i < faces.length(); ++i)
    {
        auto leftEye = Utils::sliceVect<cv::Point>(faces[i], FL68_LEFT_EYE_START_INDEX, FL68_LEFT_EYE_END_INDEX);
        auto rightEye = Utils::sliceVect<cv::Point>(faces[i], FL68_RIGHT_EYE_START_INDEX, FL68_RIGHT_EYE_END_INDEX);

        double leftEAR = calcEAR(leftEye);
        double rightEAR = calcEAR(rightEye);
        double rawEAR = (leftEAR + rightEAR) / 2.0;
        m_movAvg.push(rawEAR);
        double EAR = m_movAvg.get();

        stat.faceDetected = true;
        stat.EAR = rawEAR;
        stat.avgEAR = EAR;

        stat.boundingBox = boundingBoxes[i];
        stat.leftEye = leftEye;
        stat.rightEye = rightEye;
        stat.eyesClosed = stat.avgEAR < m_params.avgEARLimit;
        break;
    }

    emit newFrame(frame);
    emit detectionFinished(stat);
}

double FatigueDetector::calcEAR(const QVector<cv::Point> &eye)
{
    Q_ASSERT(eye.size() >= 6);
    return (Utils::euclDist(eye[1], eye[5]) + Utils::euclDist(eye[2], eye[4])) / (2.0 * Utils::euclDist(eye[0], eye[3]));
}
