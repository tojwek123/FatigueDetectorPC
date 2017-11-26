#ifndef CONFIG_H
#define CONFIG_H

/* === Constants for config === */
#define DLIB_FRONTAL_FACE_DETECTOR  0
#define CV_CASCADE_CLASSIFIER       1

/* === App config === */
//#define CFG_APP_TYPE_GUI

/* === Face detector config === */
//#define CFG_FACE_DETECTOR DLIB_FRONTAL_FACE_DETECTOR
#define CFG_FACE_DETECTOR   CV_CASCADE_CLASSIFIER

#ifdef _WIN32
    #define CFG_CV_CASCADE_CLASSIFIER_DATA_PATH "C:\\Dev\\OpenCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml"
    #define CFG_PREDICTOR_DATA_PATH             "C:\\Dev\\dlib-19.7\\resources\\shape_predictor_68_face_landmarks.dat"
#else
    #define CFG_CV_CASCADE_CLASSIFIER_DATA_PATH "/home/pi/FatigueDetector/haarcascade_frontalface_alt.xml"
    #define CFG_PREDICTOR_DATA_PATH             "/home/pi/FatigueDetector/shape_predictor_68_face_landmarks.dat"
#endif

/* === Camera config === */
#define CFG_USE_RASPICAM

#endif // CONFIG_H
