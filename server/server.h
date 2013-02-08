#ifndef SERVER_H
#define SERVER_H

#include <QtCore>
#include <QObject>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

class Server : public QObject{
    Q_OBJECT

    private:
        int clientID;
        QHash<int, QLocalSocket*> clients;
        QLocalServer *m_server;

    public:
        explicit Server(QString servername, QObject *parent = 0);
        ~Server();

    public slots:
        void disconnectClient(int clientID);
        void sendMessageToClient(int clientID, QString message);

    signals:
        void clientConnected(int clientID);
        void newMessageFromClient(int clientID, QString message);
        void clientDisconnected(int clientID);
};

#endif // SERVER_H
