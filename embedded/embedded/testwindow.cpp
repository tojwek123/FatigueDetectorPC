#include "testwindow.h"

TestWindow::TestWindow(QWidget *parent) : QWidget(parent)
{
    m_statPlot = new QCustomPlot(this);
    m_statPlot->addGraph();
    m_statPlot->addGraph();
    m_statPlot->graph(0)->setPen(QPen(Qt::blue));
    m_statPlot->graph(1)->setPen(QPen(Qt::red));
    m_statPlot->resize(640, 480);
    m_statPlot->show();
    m_statPlot->yAxis->setRange(0, 1);

    m_resetButton = new QPushButton("Reset graph", this);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_statPlot, 0, 0, 1, 3);
    layout->addWidget(m_resetButton, 1, 2);
    this->setLayout(layout);
    this->resize(640, 480);

    connect(m_resetButton, &QPushButton::clicked,
            this, &TestWindow::onResetButtonClicked);
}

void TestWindow::onResetButtonClicked(bool)
{
    m_xData.clear();
    m_avgEAR.clear();
    m_rawEAR.clear();
}

void TestWindow::onDetected(bool success, FatigueDetectorStat stat)
{
    if (success)
    {
        m_xData.append(m_avgEAR.size());
        m_avgEAR.append(stat.avgEAR);
        m_rawEAR.append(stat.rawEAR);
        m_statPlot->graph(0)->setData(m_xData, m_avgEAR);
        m_statPlot->graph(1)->setData(m_xData, m_rawEAR);
        m_statPlot->xAxis->setRange(m_xData.first(), m_xData.last());
        m_statPlot->replot();
    }
}
