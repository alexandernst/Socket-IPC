#include "server.h"

Server::Server(QString servername, QObject *parent) : QObject(parent){
    qDebug() << "Starting server...";
    m_server = new QLocalServer(this);

    while(!m_server->listen(servername)){
        if(m_server->serverError() == QAbstractSocket::AddressInUseError){
            qDebug() << m_server->serverError();
            qDebug() << "Calling removeServer.";
            m_server->removeServer(servername);
        }else{
            qDebug() << "Not able to start the server";
            qDebug() << m_server->serverError();
        }
    }

    if (m_server->isListening())
        qDebug() << "Server started";

    connect(m_server, SIGNAL(newConnection()), this, SLOT(clientConnected()));
}

Server::~Server(){
    qDebug() << "Server deletion";
    m_server->close();
    delete m_server;
}

void Server::clientDisconnected(){
    qDebug() << "Client disconnected";
}

void Server::clientConnected(){
    qDebug() << "Client connected";

    QLocalSocket *clientConnection = m_server->nextPendingConnection();

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

    while(clientConnection->bytesAvailable() < (int)sizeof(quint16))
        clientConnection->waitForReadyRead();

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_5_0);

    quint16 bytes_to_read;
    in >> bytes_to_read;

    while(clientConnection->bytesAvailable() < bytes_to_read)
        clientConnection->waitForReadyRead();

    QString message;
    in >> message;
    message = message.remove(QChar('\"'));

    qDebug() << "Message received:" << message;

    clientConnection->flush();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    QString returnMsg = "Message you sent me: " + message;

    out.setVersion(QDataStream::Qt_5_0);
	out << (quint16)returnMsg.size() << returnMsg;

    qint64 c = clientConnection->write(block);
    clientConnection->waitForBytesWritten();
    qDebug() << "Number of bytes written" << c;
    if(c == -1)
        qDebug() << "ERROR:" << clientConnection->errorString();

    clientConnection->flush();
    clientConnection->disconnectFromServer();
}
