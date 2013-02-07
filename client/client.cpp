#include "client.h"

Client::Client(QString sok_name, QObject *parent) : QObject(parent){
    sock = new QLocalSocket(this);

    connect(sock, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(sock, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
    qRegisterMetaType<QLocalSocket::LocalSocketError>("QLocalSocket::LocalSocketError");
    connect(sock, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(showError(QLocalSocket::LocalSocketError)));

    sock->connectToServer(sok_name);
    bytes_to_read = 0;
}

Client::~Client(){
    sock->abort();
    delete sock;
}

void Client::sendMessage(QString message){
    qDebug() << "Sending this message:" << message;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    qint64 c = sock->write(block);
    sock->waitForBytesWritten();

    qDebug() << "Number of bytes written:" << c;
    if(c == -1)
        qDebug() << "ERROR:" << sock->errorString();

    sock->flush();
}

void Client::receiveMessage(){
    qDebug() << "New bytes in socket";

    QDataStream in(sock);
    in.setVersion(QDataStream::Qt_5_0);

    if(bytes_to_read == 0){
        if (sock->bytesAvailable() < (int)sizeof(quint16)){
            qDebug() << "Waiting for message length";
            return;
        }
        in >> bytes_to_read;

        qDebug() << "Message length received:" << bytes_to_read;
    }

    if(in.atEnd()){
        qDebug() << "Waiting for message bytes";
        return;
    }

    in >> received_message;

    bytes_to_read = 0;

    qDebug() << "Received message:" << received_message;
}

void Client::socketConnected(){
    qDebug() << "Connected to socket";
}

void Client::socketDisconnected(){
    qDebug() << "Disconnected from socket";
}

void Client::showError(QLocalSocket::LocalSocketError error){
    qDebug() << "Error happened:" << error;
}
