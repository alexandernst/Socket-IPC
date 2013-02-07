#include <QtCore/QCoreApplication>
#include "client.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    Client *client = new Client("ipc");

    QString msg;
    for(int i = 0; i <= 10; i++)
        msg.append(QChar(rand() % ((90 + 1) - 65) + 65)); //From ASCII 65 to 90 (A-Z)
    client->sendMessage(msg);
    
    return a.exec();
}
