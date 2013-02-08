#include <QtCore/QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    Server *server = new Server("ipc");

    QObject::connect(server, &Server::clientConnected, [server](int clientID){
        qDebug() << "Client" << clientID << "connected.";
    });

    QObject::connect(server, &Server::newMessageFromClient, [server](int clientID, QString message){
        qDebug() << "Message received:" << message;
        qDebug() << "Sending reply: 'Message you sent me:" << message << "'";
        server->sendMessageToClient(clientID, "Message you sent me: " + message);
        server->disconnectClient(clientID);
    });

    QObject::connect(server, &Server::clientDisconnected, [](int clientID){
        qDebug() << "Client" << clientID << "disconnected.\n\n";
    });
    
    return a.exec();
}
