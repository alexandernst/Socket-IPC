#include <QtCore/QCoreApplication>
#include "client.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    Client *client = new Client("ipc");

    QObject::connect(client, &Client::newMessageFromServer, [](QString message){
        qDebug() << "Received message from server:" << message;
    });

    QObject::connect(client, &Client::disconnected, [](){
        qDebug() << "Disconnected from server";
    });

    QString message;
    for(int i = 0; i <= 10; i++)
        message.append(QChar(rand() % ((90 + 1) - 65) + 65)); //From ASCII 65 to 90 (A-Z)
    qDebug() << "Sending this message:" << message;
    client->sendMessage(message);
    
    return a.exec();
}
