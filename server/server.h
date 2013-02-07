#ifndef SERVER_H
#define SERVER_H

#include <QtCore>
#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class Server : public QObject{
    Q_OBJECT

    private:
        QLocalServer *m_server;

    public:
        explicit Server(QString servername, QObject *parent = 0);
        ~Server();

    public slots:
        void clientDisconnected();
        void clientConnected();
};

#endif // SERVER_H
