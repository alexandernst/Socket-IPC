#ifndef CLIENT_H
#define CLIENT_H

#include <QtCore>
#include <QObject>
#include <QtNetwork/QLocalSocket>


class Client : public QObject{
    Q_OBJECT

    private:
        QLocalSocket *sock;
        qint16 bytes_to_read;
        QString received_message;

    public:
        explicit Client(QString sok_name, QObject *parent = 0);
        ~Client();
        void sendMessage(QString message);
        QString getMessage();


    signals:
        void messageReceived();

    public slots:
        void receiveMessage();
        void socketConnected();
        void socketDisconnected();
        void showError(QLocalSocket::LocalSocketError error);
};

#endif // CLIENT_H
