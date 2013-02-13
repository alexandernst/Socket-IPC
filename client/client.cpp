#include "client.h"

Client::Client(QString sock_name, QObject *parent) : QObject(parent){
    sock = new QLocalSocket(this);

    quint16 btr = 0;
    QObject::connect(sock, &QLocalSocket::readyRead, [this, &btr](){
        QDataStream in(sock);
        in.setVersion(QDataStream::Qt_5_0);

        if(btr == 0)
            in >> btr;

        if(sock->bytesAvailable() < btr)
            return;

        QString message;
        in >> message;

        btr = 0;
        emit newMessageFromServer(message);
    });

    connect(sock, &QLocalSocket::disconnected, [this](){
        emit disconnected();
    });

    sock->connectToServer(sock_name);
}

Client::~Client(){
    sock->abort();
    delete sock;
}

void Client::sendMessage(QString message){
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

void Client::disconnect(){
    sock->disconnectFromServer();
}
