#ifndef REMOTEDATAEXCHANGERSERVER_H
#define REMOTEDATAEXCHANGERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <vector>
#include "variable.h"
#include "utils.h"
#include "config.h"

enum class RemoteRequest {
    ReqVarInfo,
    StartVarStream,
    StopVarStream,
    StartVideoStream,
    StopVideoStream,
    SetVarValue
};

class RemoteDataExchangerServer : public QObject
{
    Q_OBJECT
private:
    QTcpServer m_server;
    QTcpSocket *m_client;
    bool m_clientConnected;
    bool m_readHeader;
    QStringList m_headerTokens;
    int m_dataBytesToRead;

    static const char TokenSeparator = ' ';
    static const char VarTokenSeparator = ';';

    QVariant varStrToVariant(const QString &valueStr, const VarType &type);
    QString varTypeToStr(const VarType &type);
    QString valueToStr(const QVariant &value, const VarType &type);
    void parseMessage(const QStringList &headerTokens, const QByteArray &data);
    void send(const QStringList &headerTokens, const QByteArray &data);
    void send(const QStringList &headerTokens, const QStringList &dataTokens);

public:
    explicit RemoteDataExchangerServer(QObject *parent = 0);
    void listen(int port);
    inline bool isClientConnected() { return m_clientConnected; }
    QHostAddress getClientAddress();
    int getClientPort();
    void respondVarInfo(const QMap<QString, Variable> &variables);
    void sendVarStreamValue(const QMap<QString, Variable> &variables);
    void sendVideoStreamFrame(const cv::Mat &frame, const int compression);

signals:
    void clientStateChanged(bool connected);
    void newRequest(RemoteRequest request, QVector<QVariant> args);

private slots:
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();

public slots:
};

#endif // REMOTEDATAEXCHANGERSERVER_H
