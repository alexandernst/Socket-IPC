#include "server.h"

Server::Server(QString servername, QObject *parent) : QObject(parent){
    qDebug() << "Starting server...";
    m_server = new QLocalServer(this);

    while(!m_server->listen(servername)){
        if(m_server->serverError() == QAbstractSocket::AddressInUseError){
            qDebug() << m_server->serverError() << "\nCalling removeServer...";
            m_server->removeServer(servername);
        }else{
            qDebug() << "Not able to start the server!\n" << m_server->serverError();
        }
    }

    if (m_server->isListening())
        qDebug() << "Server started.\n\n";
    clientID = 0;

    connect(m_server, &QLocalServer::newConnection, [this](){
        clientID++;

        QLocalSocket *sock = m_server->nextPendingConnection();
        clients.insert(clientID, sock);
        sock->setProperty("ID", QVariant(clientID));
        sock->setProperty("BTR", QVariant(0));

        QObject::connect(sock, &QLocalSocket::readyRead, [this, sock](){
            QDataStream in(sock);
            in.setVersion(QDataStream::Qt_5_0);

            if(sock->property("BTR").toInt() == 0){
                quint16 btr;
                in >> btr;
                sock->setProperty("BTR", QVariant(btr));
            }

            if(sock->bytesAvailable() < sock->property("BTR").toInt())
                return;

            QString message;
            in >> message;

            sock->setProperty("BTR", QVariant(0));
            emit newMessageFromClient(sock->property("ID").toInt(), message);
        });

        connect(sock, &QLocalSocket::disconnected, [this, sock](){
            emit clientDisconnected(sock->property("ID").toInt());
            sock->deleteLater();
        });

        emit clientConnected(clientID);
    });
}

Server::~Server(){
    qDebug() << "Server deletion";
    m_server->close();
    delete m_server;
}

void Server::sendMessageToClient(int clientID, QString message){
    QLocalSocket *sock = clients.value(clientID);
    sock->flush();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_0);

    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    qint64 c = sock->write(block);
    sock->waitForBytesWritten();
    if(c == -1)
        qDebug() << "ERROR:" << sock->errorString();

    sock->flush();
}

void Server::disconnectClient(int clientID){
    clients.value(clientID)->disconnectFromServer();
    clients.value(clientID)->deleteLater();
    clients.remove(clientID);
}
