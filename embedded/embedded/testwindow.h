#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QGridLayout>
#include <QPushButton>
#include "qcustomplot.h"
#include "fatiguedetector.h"

class TestWindow : public QWidget
{
    Q_OBJECT
private:
    QPushButton *m_resetButton;
    QCustomPlot *m_statPlot;
    QVector<double> m_xData;
    QVector<double> m_avgEAR;
    QVector<double> m_rawEAR;

public:
    TestWindow(QWidget *parent = 0);

private slots:
    void onResetButtonClicked(bool);

public slots:
    void onDetected(bool success, FatigueDetectorStat stat);
};

#endif // CONSOLE_H
