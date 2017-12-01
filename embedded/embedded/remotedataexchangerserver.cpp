#include "remotedataexchangerserver.h"

RemoteDataExchangerServer::RemoteDataExchangerServer(QObject *parent) :
    QObject(parent),
    m_clientConnected(false),
    m_readHeader(true),
    m_dataBytesToRead(0)
{
    connect(&m_server, &QTcpServer::newConnection,
            this, &RemoteDataExchangerServer::onNewConnection);
}

void RemoteDataExchangerServer::onNewConnection()
{
    if (!m_clientConnected)
    {
        m_client = m_server.nextPendingConnection();
        connect(m_client, &QTcpSocket::readyRead,
                this, &RemoteDataExchangerServer::onClientReadyRead);
        connect(m_client, &QTcpSocket::disconnected,
                this, &RemoteDataExchangerServer::onClientDisconnected);
        m_clientConnected = true;
        m_readHeader = true;
        emit clientStateChanged(true);
    }
    else
    {
        m_server.nextPendingConnection()->disconnect();
    }
}

void RemoteDataExchangerServer::onClientReadyRead()
{
    while (true)
    {
        if (m_readHeader)
        {
            if (m_client->canReadLine())
            {
                QString line = QString(m_client->readLine());
                m_headerTokens = line.split(TokenSeparator);
                m_dataBytesToRead = m_headerTokens.last().toInt();
                m_headerTokens.pop_back();
                m_readHeader = false;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (m_client->bytesAvailable() >= m_dataBytesToRead)
            {
                QByteArray data = m_client->read(m_dataBytesToRead);
                m_readHeader = true;
                parseMessage(m_headerTokens, data);
            }
        }
    }
}

void RemoteDataExchangerServer::parseMessage(const QStringList &headerTokens, const QByteArray &data)
{
    QVector<QVariant> args;

    if (headerTokens.length() > 0)
    {
        if ("reqVarInfo" == headerTokens[0])
        {
            std::cout << "reqVarInfo" << std::endl;
            emit newRequest(RemoteRequest::ReqVarInfo, args);
        }
        else if ("startVarStream" == headerTokens[0])
        {
            std::cout << "startVarStream" << std::endl;
            emit newRequest(RemoteRequest::StartVarStream, args);
        }
        else if ("stopVarStream" == headerTokens[0])
        {
            std::cout << "stopVarStream" << std::endl;
            emit newRequest(RemoteRequest::StopVarStream, args);
        }
        else if ("startVideoStream" == headerTokens[0])
        {
            emit newRequest(RemoteRequest::StartVideoStream, args);
        }
        else if ("stopVideoStream" == headerTokens[0])
        {
            emit newRequest(RemoteRequest::StopVideoStream, args);
        }
    }
}

void RemoteDataExchangerServer::send(const QStringList &headerTokens, const QByteArray &data)
{
    QString header;
    for (auto i : headerTokens)
    {
        header += i + TokenSeparator;
    }
    header += QString::number(data.length()) + '\n';
    m_client->write(header.toUtf8());
    m_client->write(data);
}

void RemoteDataExchangerServer::send(const QStringList &headerTokens, const QStringList &dataTokens)
{
    QString dataStr;
    for (int i = 0; i < dataTokens.length(); ++i)
    {
        if (i > 0)
        {
            dataStr += TokenSeparator;
        }
        dataStr += dataTokens[i];
    }
    send(headerTokens, dataStr.toUtf8());
}

void RemoteDataExchangerServer::respondVarInfo(const QMap<QString, Variable> &variables)
{
    QStringList headerTokens = { "varInfo" };
    QStringList dataTokens;

    for (auto i : variables.values())
    {
        QString token;
        token += i.name + VarTokenSeparator;
        token += i.type + VarTokenSeparator;
        token += (i.readOnly ? QString("r") : QString("rw")) + VarTokenSeparator;
        if ("float" == i.type)
        {
            token += QString::number(i.value.toDouble(), 'f', 3);
        }
        else
        {
            token += i.value.toString();
        }
        dataTokens.append(token);
    }

    send(headerTokens, dataTokens);
}

void RemoteDataExchangerServer::sendVarStreamValue(const QMap<QString, Variable> &variables)
{
    QStringList headerTokens = { "varStreamValue" };
    QStringList dataTokens;

    for (auto i : variables.values())
    {
        QString token;
        token += i.name + VarTokenSeparator;
        if ("float" == i.type)
        {
            token += QString::number(i.value.toDouble(), 'f', 3);
        }
        else
        {
            token += i.value.toString();
        }
        dataTokens.append(token);
    }

    send(headerTokens, dataTokens);
}

void RemoteDataExchangerServer::sendVideoStreamFrame(const cv::Mat &frame, const int compression)
{
    QStringList headerTokens = { "videoStreamFrame" };
    std::vector<uchar> encodedFrameStdVect;
    if (cv::imencode(".jpg", frame, encodedFrameStdVect, { CV_IMWRITE_JPEG_QUALITY, compression }))
    {
        QByteArray encodedFrame(reinterpret_cast<const char*>(encodedFrameStdVect.data()), int(encodedFrameStdVect.size()));
        send(headerTokens, encodedFrame);
    }
}

void RemoteDataExchangerServer::onClientDisconnected()
{
    m_clientConnected = false;
    m_headerTokens.clear();
    m_dataBytesToRead = 0;
    emit clientStateChanged(false);
}

void RemoteDataExchangerServer::listen(int port)
{
    m_server.listen(QHostAddress::Any, port);
}

QHostAddress RemoteDataExchangerServer::getClientAddress()
{
    QHostAddress addr;
    if (m_clientConnected)
    {
        addr = m_client->peerAddress();
    }
    return addr;
}

int RemoteDataExchangerServer::getClientPort()
{
    int port = 0;
    if (m_clientConnected)
    {
        port = m_client->peerPort();
    }
    return port;
}
