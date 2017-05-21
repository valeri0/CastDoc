#include "client.h"
#include <QApplication>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>



int main(int argc, char* argv[])
{

    QApplication a(argc, argv);
    client w;
    w.show();

    return a.exec();
}
