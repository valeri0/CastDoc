#include "client.h"
#include "ui_client.h"
#include<qfiledialog.h>
#include<qmessagebox.h>
#include <QStyle>
#include <QDesktopWidget>
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
#include <QDebug>

#define CHUNK_SIZE 4096 // vom trimite catre server bucati de 0.5 MB din fisier


int sd;
bool flag1,flag2;

client::client(QWidget *parent) : QMainWindow(parent), ui(new Ui::client){

    // descriptorul de socket
      struct sockaddr_in server;	// structura folosita pentru conectare

      /* stabilim portul */
      int port = 2049;

      /* cream socketul */
      if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
          perror ("Eroare la socket().\n");

        }

      /* umplem structura folosita pentru realizarea conexiunii cu serverul */
      /* familia socket-ului */
      server.sin_family = AF_INET;
      /* adresa IP a serverului */
      server.sin_addr.s_addr = inet_addr("127.0.0.1");
      /* portul de conectare */
      server.sin_port = htons (port);

      /* ne conectam la server */

      if (::connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1){
          perror ("[client]Eroare la connect().\n");

        }

      ui->setupUi(this);

      QMainWindow::setGeometry(QStyle::alignedRect(
                                      Qt::LeftToRight,
                                      Qt::AlignCenter,
                                      this->size(),
                                      qApp->desktop()->availableGeometry()
                                    ));


      ui->pushButton_2->setHidden(true);

      ui->pushButton_3->setHidden(true);

      QPixmap pix("//home/valerio/build-castDoc-Desktop_Qt_5_7_0_GCC_64bit-Debug/logo.png");

      ui->label->setPixmap(pix.scaled(500,300));
}

client::~client(){
    delete ui;
}

void client::on_pushButton_clicked(){


    const char *input_file_extension;

    QString inputFileName=QFileDialog::getOpenFileName(
                this,
                tr("Browse"),
                "/home",
                "Portable Document Format(*.pdf);; LaTeX(*.tex);; Postscript (*.ps);;DOS (*.dos);;UNIX (*.unix);;JPG(*.jpg);;BMP(*.bmp);;GIF(*.gif);;PNG(*.png)"
               );

    ui->lineEdit->setText(inputFileName);

    QFileInfo fi(ui->lineEdit->text());
    QString ext=fi.suffix();

    std::string extension = ext.toStdString().c_str();
    input_file_extension=extension.c_str();



    if(strcmp(input_file_extension,"tex")==0){

        QStringList list=(QStringList()<<"man"<<"html");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }

    else if(strcmp(input_file_extension,"pdf")==0){

        QStringList list=(QStringList()<<"ps"<<"text"<<"html");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }

    else if(strcmp(input_file_extension,"ps")==0){

        QStringList list=(QStringList()<<"ascii"<<"epsi"<<"ps"<<"pdf"<<"pdf12"<<"pdf13"<<"pdf14"<<"pdfwr");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }

    else if(strcmp(input_file_extension,"dos")==0){

        QStringList list=(QStringList()<<"unix");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }

    else if(strcmp(input_file_extension,"unix")==0){

        QStringList list=(QStringList()<<"dos");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }
    else if(strcmp(input_file_extension,"jpg")==0){

        QStringList list=(QStringList()<<"bmp"<<"gif"<<"png");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }
    else if(strcmp(input_file_extension,"bmp")==0){

        QStringList list=(QStringList()<<"jpg"<<"gif"<<"png");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }
    else if(strcmp(input_file_extension,"gif")==0){

        QStringList list=(QStringList()<<"jpg"<<"bmp"<<"png");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }
    else if(strcmp(input_file_extension,"png")==0){

        QStringList list=(QStringList()<<"jpg"<<"bmp"<<"png");
        ui->comboBox->clear();
        ui->comboBox->addItems(list);
    }

    if(!ui->lineEdit->text().isEmpty())
    {
        ui->pushButton_2->setHidden(false);
        ui->pushButton_2->setDisabled(false);
    }
    else
    {
        ui->comboBox->clear();
        ui->pushButton_2->setDisabled(true);
    }

}

void client::on_pushButton_2_clicked(bool checked){

    char *input=NULL;
    char *input_file_extension=NULL;
    char *output_file_extension=NULL;

        ui->pushButton_2->setText("Uploading...");

        QFileInfo fi(ui->lineEdit->text());
        QString ext=fi.suffix();


        std::string str=ui->lineEdit->text().toStdString().c_str();
        input=(char*)str.c_str();

        std::string extension = ext.toStdString().c_str();
        input_file_extension=(char*)extension.c_str();

        QString outExt = ui->comboBox->currentText();
        std::string strExt=outExt.toStdString().c_str();
        output_file_extension=(char*)strExt.c_str();


        int input_file_extension_size=strlen(input_file_extension);
        int output_file_extension_size=strlen(output_file_extension);

        QApplication::processEvents();

        if (::write (sd,&input_file_extension_size,sizeof(int)) <= 0){

            perror ("[client]Eroare la write() spre server.\n");
        }

        //trimitem extensia fisierului de input

        if (::write (sd,input_file_extension,input_file_extension_size) <= 0){
            perror ("[client]Eroare la write() spre server.\n");
        }


        //trimitem dimensiunea extensiei fisierului de output
        if (::write (sd,&output_file_extension_size,sizeof(int)) <= 0){

            perror ("[client]Eroare la write() spre server.\n");
        }

        //trimitem extensia fisierului de output

        if (::write (sd,output_file_extension,output_file_extension_size) <= 0){

            perror ("[client]Eroare la write() spre server.\n");
        }

        FILE* document=fopen(input,"rb");

        if(document==NULL){

            printf("Nu s-a putut deschide!\n");
        }

        //setam cursorul la inceputul fisierului pentru a putea masura dimensiunea totala a fisierului

        fseek(document,0,SEEK_END);

        unsigned long documentSize=ftell(document);

        rewind(document);


        //trimitem serverului lungimea totala a fisierului

        if (write (sd,&documentSize,sizeof(unsigned long)) <= 0){

            perror ("[client]Eroare la write() spre server.\n");
        }


        char buffer[CHUNK_SIZE];

        size_t chunk;

        unsigned int sent=0;

        ui->pushButton_2->setText("Uploading...");

        do{

            chunk = fread(buffer,1,sizeof(buffer),document);

            if(chunk <=0 ) break;

            if (write (sd,&chunk,sizeof(size_t)) <= 0){

                perror ("[client]Eroare la write() spre server.\n");
            }

            if (write (sd,&buffer,sizeof(buffer)) <= 0){

                perror ("[client]Eroare la write() spre server.\n");
            }

            sent+=chunk;

        }while(sent<documentSize);//do while

        fclose(document);

        int downloadFlag;

        if (read (sd, &downloadFlag,sizeof(int)) <= 0){

            perror ("[client]Eroare la read() de la server.\n");

        }

         QApplication::processEvents();

       if(downloadFlag==1){

           ui->pushButton->setDisabled(true);

           ui->pushButton_2->setHidden(true);

           ui->pushButton_3->setHidden(false);
       }


}

void client::on_lineEdit_textChanged(const QString &arg1){

    flag1=true;

}

void client::on_comboBox_currentTextChanged(const QString &arg1)
{
    flag2=true;
}

void client::on_pushButton_3_clicked()
{

    char buffer[CHUNK_SIZE];

    size_t chunk;



    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                    QFileDialog::ShowDirsOnly

                                            );
    QString outExt = ui->comboBox->currentText();

    QString strExt=outExt.toStdString().c_str();

    QFileInfo fi(ui->lineEdit->text());


    QString baseName = fi.baseName().toStdString().c_str();
    baseName.append(".");


    dir=dir+"/";
    dir=dir+baseName;


    if(strcmp(strExt.toStdString().c_str(),"html")==0)

        dir=dir+"zip";
    else

        dir=dir+strExt;

            //primim fisierul convertit;

            unsigned long fileReceivedFromServerSize;
            FILE* rez= fopen(dir.toStdString().c_str(),"wb");

            int received=0;

            if(rez==NULL){
                perror("fopen");
            }

            //am primit de la server lungimea totala a fisierului
            if (read (sd, &fileReceivedFromServerSize,sizeof(unsigned long)) <= 0){

                perror ("[client]Eroare la read() de la server.\n");

            }


            do{

                if (read (sd, &chunk,sizeof(size_t)) <= 0){

                    perror ("[client]Eroare la read() de la server.\n");

                }

                if(chunk<=0) break;

                if (read (sd, &buffer,sizeof(buffer)) <= 0){

                    perror ("[client]Eroare la read() de la server.\n");

                }

                fwrite(buffer,1,sizeof(buffer),rez);

                received+=chunk;


            }while(received<fileReceivedFromServerSize);


            fclose(rez);

            QMessageBox::information(this,"Notification","Your file downloaded succesfully!");

            ui->lineEdit->clear();

            ui->comboBox->clear();

            ui->pushButton->setDisabled(false);

            ui->pushButton_2->setText("Upload");

            ui->pushButton_2->setDisabled(true);

            ui->pushButton_3->hide();

            ui->pushButton_2->show();



}
