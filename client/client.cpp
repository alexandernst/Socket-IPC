#include "client.h"

Client::Client(QString sock_name, QObject *parent) : QObject(parent){
    sock = new QLocalSocket(this);
    sock->setProperty("BTR", QVariant(0));

    QObject::connect(sock, &QLocalSocket::readyRead, [this](){
        QDataStream in(sock);
        in.setVersion(QDataStream::Qt_5_0);

        if(sock->property("BTR").toInt() == 0){
            quint64 btr;
            in >> btr;
            sock->setProperty("BTR", QVariant(btr));
        }

        if(sock->bytesAvailable() < sock->property("BTR").toInt())
            return;

        QString message;
        in >> message;

        sock->setProperty("BTR", QVariant(0));
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
    sock->flush();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_0);

    out << (quint64)0;
    out << message;
    out.device()->seek(0);
    out << (quint64)(block.size() - sizeof(quint64));

    qint64 c = sock->write(block);
    sock->waitForBytesWritten();
    if(c == -1)
        qDebug() << "ERROR:" << sock->errorString();

    sock->flush();
}

void Client::disconnect(){
    sock->disconnectFromServer();
}
