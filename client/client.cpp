#include "client.h"

Client::Client(QString sock_name, QObject *parent) : QObject(parent){
    sock = new QLocalSocket(this);

    connect(sock, &QLocalSocket::disconnected, [this](){
        emit disconnected();
    });

    connect(sock, &QLocalSocket::readyRead, [this](){
        QDataStream in(sock);
        in.setVersion(QDataStream::Qt_5_0);

        if(bytes_to_read == 0){
            if(sock->bytesAvailable() < (int)sizeof(quint16))
                return;
            in >> bytes_to_read;
        }

        if(in.atEnd())
            return;

        in >> received_message;
        bytes_to_read = 0;

        emit newMessageFromServer(received_message);
    });

    sock->connectToServer(sock_name);
    bytes_to_read = 0;
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
