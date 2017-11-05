//#include <iostream>
//#include <fstream>
//#include <string>
//#include <thread>
//#include <chrono>
//#include <ctime>
//#include <atomic>
//#include <cstdio>
//#include <regex>
//#include <QApplication>
//#include <qcustomplot.h>
//#include <QTextEdit>
//#include <opencv2/opencv.hpp>
//#include "facedetector.h"
//#include "facelandmark68.h"
//#include "utils.h"

//const std::string PredictorFilename = "C:\\Dev\\dlib-19.7\\resources\\shape_predictor_68_face_landmarks.dat";
//const std::string LogFilename = "D:\\Studia\\Semestr_7\\Inz\\software\\pc\\Log.txt";

//double calcEAR(const std::vector<cv::Point> &eye)
//{
//    return (Utils::euclDist(eye[1], eye[5]) + Utils::euclDist(eye[2], eye[4])) / (2.0 * Utils::euclDist(eye[0], eye[3]));
//}

//void imProc();
//void consoleInput();
//void beeping();

//std::atomic<double> EyeClosedEAR(0.2);
//std::atomic<int> FramesToCloseEye(2);
//std::atomic<int> AlertTimeoutMs(2000);
//std::atomic<bool> CloseApp(false);
//std::atomic<bool> ShowFullFace(false);
//std::atomic<bool> BeepWarn(false);
//std::ofstream LogFile;

//int main(int argc, char **argv)
//{
//    //=========
//    QVector<double> x = {1, 2, 3, 4, 5};
//    QVector<double> y = {1, 2, 3, 2, 3};

//    QApplication app(argc, argv);
//    QCustomPlot plot;
//    plot.addGraph();
//    plot.graph(0)->setData(x, y);
//    plot.replot();
//    plot.show();
//    //=========

//    LogFile.open(LogFilename, std::ios::out);

//    if (!LogFile.is_open())
//    {
//        std::cout << "Cannot open log file. Exiting...";
//        return -1;
//    }

//    std::thread imProcThread(imProc);
//    app.exec();
//    imProcThread.join();
//    LogFile.close();
//}

//void imProc()
//{
//    FaceDetector detector;

//    std::cout <<  "Loading predictor data file...";

//    try
//    {
//        detector.loadPredictorData(PredictorFilename);
//    }
//    catch (...)
//    {
//        std::cout << " Cannot load predictor data file." << std::endl;;
//        return;
//    }

//    std::cout << " File successfully loaded." << std::endl;
//    std::cout << "Opening camera...";
//    cv::VideoCapture cap(0);

//    if (!cap.isOpened())
//    {
//        std::cout << " Cannot open camera.";
//        return;
//    }

//    std::cout << " Camera opened." << std::endl;
//    std::thread consoleInputThread(consoleInput);
//    std::thread beepingThread(beeping);
//    cv::Mat frame;
//    int eyesClosedFramesCnt = 0;
//    bool eyesClosed = false;
//    clock_t alertStartTime = clock();
//    Utils::MovAvgDbl movAvg(15);

//    cv::namedWindow("Frame");
//    cv::moveWindow("Frame", 0, 0);

//    while (!CloseApp)
//    {
//        cap >> frame;
//        //cv::pyrDown(frame, frame);

//        auto faces = detector.detect(frame);

//        for (auto face : faces)
//        {
//            auto leftEye = Utils::sliceVect<cv::Point>(face, FL68_LEFT_EYE_START_INDEX, FL68_LEFT_EYE_END_INDEX);
//            auto rightEye = Utils::sliceVect<cv::Point>(face, FL68_RIGHT_EYE_START_INDEX, FL68_RIGHT_EYE_END_INDEX);
//            double leftEAR = calcEAR(leftEye);
//            double rightEAR = calcEAR(rightEye);

//            movAvg.push((leftEAR + rightEAR) / 2.0);
//            double EAR = movAvg.get();

//            LogFile << EAR << std::endl;

//            std::string strEAR = "EAR = " + std::to_string(EAR);
//            cv::putText(frame, strEAR, cv::Point(5, 15), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1);

//            if (EAR <= EyeClosedEAR && eyesClosedFramesCnt < FramesToCloseEye)
//            {
//                ++eyesClosedFramesCnt;
//            }
//            else if (EAR > EyeClosedEAR && eyesClosedFramesCnt > 0)
//            {
//                --eyesClosedFramesCnt;
//            }
//            if (eyesClosedFramesCnt >= FramesToCloseEye && !eyesClosed)
//            {
//                eyesClosed = true;
//                alertStartTime = clock();
//            }
//            else if (eyesClosedFramesCnt == 0)
//            {
//                eyesClosed = false;
//                BeepWarn = false;
//            }

//            if (eyesClosed && double(clock() - alertStartTime) / double(CLOCKS_PER_SEC) * 1000.0 >= AlertTimeoutMs)
//            {
//                BeepWarn = true;
//            }

//            if (BeepWarn)
//            {
//                cv::putText(frame, "WAKE UP!!!", cv::Point(5, 30), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1);
//            }

//            Utils::drawPoly(frame, leftEye, cv::Scalar(255, 0, 0), 1);
//            Utils::drawPoly(frame, rightEye, cv::Scalar(255, 0, 0), 1);

//            if (ShowFullFace)
//            {
//                auto mouth = Utils::sliceVect<cv::Point>(face, FL68_MOUTH_START_INDEX, FL68_MOUTH_END_INDEX);
//                auto jawline = Utils::sliceVect<cv::Point>(face, FL68_JAWLINE_START_INDEX, FL68_JAWLINE_END_INDEX);
//                auto leftEyebrow = Utils::sliceVect<cv::Point>(face, FL68_LEFT_EYEBROW_START_INDEX, FL68_LEFT_EYEBROW_END_INDEX);
//                auto rightEyebrow = Utils::sliceVect<cv::Point>(face, FL68_RIGHT_EYEBROW_START_INDEX, FL68_RIGHT_EYEBROW_END_INDEX);
//                auto noseVertical = Utils::sliceVect<cv::Point>(face, FL68_NOSE_VERTICAL_START_INDEX, FL68_NOSE_VERTICAL_END_INDEX);
//                auto noseHorizontal = Utils::sliceVect<cv::Point>(face, FL68_NOSE_HORIZONTAL_START_INDEX, FL68_NOSE_HORIZONTAL_END_INDEX);

//                Utils::drawPolyChain(frame, mouth, cv::Scalar(255, 0, 0), 1);
//                Utils::drawPolyChain(frame, jawline, cv::Scalar(255, 0, 0), 1);
//                Utils::drawPolyChain(frame, leftEyebrow, cv::Scalar(255, 0, 0), 1);
//                Utils::drawPolyChain(frame, rightEyebrow, cv::Scalar(255, 0, 0), 1);
//                Utils::drawPolyChain(frame, noseVertical, cv::Scalar(255, 0, 0), 1);
//                Utils::drawPolyChain(frame, noseHorizontal, cv::Scalar(255, 0, 0), 1);
//            }

//            break;
//        }

//        cv::imshow("Frame", frame);
//        cv::waitKey(1);
//    }

//    beepingThread.join();
//    consoleInputThread.join();
//}

//void beeping()
//{
//    while (!CloseApp)
//    {
//        if (BeepWarn)
//        {
//            Utils::beep(1000, 100);
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    }
//}

//void consoleInput()
//{
//    std::cout << "\nEnter command:" << std::endl;
//    std::string input;

//    while (true)
//    {
//        std::cout << ">>";
//        std::getline(std::cin, input);

//        std::regex re("([a-zA-Z]*)(\\s*=\\s*([a-zA-Z0-9\\.]+))?");
//        std::smatch matches;

//        if (std::regex_search(input, matches, re))
//        {
//            std::string cmd = matches[1];
//            std::string arg = matches[3];

//            if ("exit" == cmd)
//            {
//                std::cout << "Closing app..." << std::endl;
//                CloseApp = true;
//                break;
//            }
//            else if ("showFullFace" == cmd)
//            {
//                std::cout << "Full face on" << std::endl;
//                ShowFullFace = true;
//            }
//            else if ("hideFullFace" == cmd)
//            {
//                std::cout << "Full face off" << std::endl;
//                ShowFullFace = false;
//            }
//            else if ("listVar" == cmd)
//            {
//                std::cout << "EyeClosedEAR = " << EyeClosedEAR << std::endl;
//                std::cout << "FramesToCloseEye = " << FramesToCloseEye << std::endl;
//                std::cout << "AlertTimeoutMs = " << AlertTimeoutMs << std::endl;
//            }
//            else if ("EyeClosedEAR" == cmd)
//            {
//                try
//                {
//                    double val = std::stod(arg);
//                    EyeClosedEAR = val;
//                    std::cout << "EyeClosedEAR = " << val << std::endl;
//                }
//                catch (...)
//                {
//                    std::cout << "Invalid arg format" << std::endl;
//                }
//            }
//            else if ("FramesToCloseEye" == cmd)
//            {
//                try
//                {
//                    int val = std::stoi(arg);
//                    FramesToCloseEye = val;
//                    std::cout << "FramesToCloseEye = " << val << std::endl;
//                }
//                catch (...)
//                {
//                    std::cout << "Invalid arg format" << std::endl;
//                }
//            }
//            else if ("AlertTimeoutMs" == cmd)
//            {
//                try
//                {
//                    int val = std::stoi(arg);
//                    AlertTimeoutMs = val;
//                    std::cout << "AlertTimeoutMs = " << val << std::endl;
//                }
//                catch (...)
//                {
//                    std::cout << "Invalid arg format" << std::endl;
//                }
//            }
//            else
//            {
//                std::cout << "Unknown command" << std::endl;
//            }
//        }
//    }
//}
