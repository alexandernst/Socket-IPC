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
        qDebug() << "Server started.\n\n";
    clientID = 0;

    connect(m_server, &QLocalServer::newConnection, [this](){
        clientID++;

        QLocalSocket *clientConnection = m_server->nextPendingConnection();
        clients.insert(clientID, clientConnection);
        clientConnection->setProperty("ID", QVariant(clientID));

        connect(clientConnection, &QLocalSocket::disconnected, [this, clientConnection](){
            emit clientDisconnected(clientConnection->property("ID").toInt());
            clientConnection->deleteLater();
        });

        emit clientConnected(clientID);

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

        emit newMessageFromClient(clientID, message);
    });
}

Server::~Server(){
    qDebug() << "Server deletion";
    m_server->close();
    delete m_server;
}

void Server::sendMessageToClient(int clientID, QString message){
    QLocalSocket *clientConnection = clients.value(clientID);
    clientConnection->flush();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_0);
    out << (quint16)message.size() << message;

    qint64 c = clientConnection->write(block);
    clientConnection->waitForBytesWritten();
    if(c == -1)
        qDebug() << "ERROR:" << clientConnection->errorString();

    clientConnection->flush();
}

void Server::disconnectClient(int clientID){
    clients.value(clientID)->disconnectFromServer();
    clients.remove(clientID);
}
