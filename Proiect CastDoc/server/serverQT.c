#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

/* portul folosit */
#define PORT 2049
#define CHUNK_SIZE 4096 // vom primi de la client bucati de 0.5 MB din fisier

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */

void raspunde(void *);

char* checkConversion(char argument[100]);



int main ()
{
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;			//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	int i=0;
  

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){
      perror ("Eroare la socket().\n");
      return errno;
  }

  /* utilizam optiunea 
  SO_REUSEADDR la nivelul socket-ului (SOL_SOCKET) */
  
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* completam structura folosita de server */

  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;

  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam adresa locala la socket */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1){
      perror ("Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1){
      perror ("Eroare la listen().\n");
      return errno;
    }

  /* servim in mod concurent clientii...folosind thread-uri */
  while (1){
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      printf ("Waiting at port %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0){
	  perror ("Eroare la accept().\n");
	  continue;
	}
	
        /* s-a realizat conexiunea, se astepta mesajul */
    
	int idThread; //id-ul threadului
	int cl; //descriptorul intors de accept

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}//while    
};				
void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);	
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
		fflush (stdout);		 
		pthread_detach(pthread_self());		
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((int)arg);
		return(NULL);	
  		
};


void raspunde(void *arg){
    
	struct thData tdL; 
	tdL= *((struct thData*)arg);



	while(1){

	int input_file_extension_size=0;
	int output_file_extension_size=0;

	//citim dimensiunea extensiei a input-ului
	if (read (tdL.cl, &input_file_extension_size,sizeof(int)) <= 0){
		
		printf("[Thread %d]\n",tdL.idThread);
		perror ("Eroare la read() de la client.\n");
		return;
	}

	char *input_file_extension=malloc(sizeof(char)*(input_file_extension_size+1));
	input_file_extension[input_file_extension_size]=0;

 
 	//citim extensia input-ului
 	if (read (tdL.cl, input_file_extension,input_file_extension_size) <= 0){
		
		printf("[Thread %d]\n",tdL.idThread);
		perror ("Eroare la read() de la client.\n");
	}

 	//citim dimensiunea extensiei a output-ului
 	if (read (tdL.cl, &output_file_extension_size,sizeof(int)) <= 0){
		
		printf("[Thread %d]\n",tdL.idThread);
		perror ("Eroare la read() de la client.\n");
	}
 	
 	char *output_file_extension=malloc(sizeof(char)*(output_file_extension_size+1));

 	//citim extensia output-ului
 	if (read (tdL.cl, output_file_extension,output_file_extension_size) <= 0){
		
		printf("[Thread %d]\n",tdL.idThread);
		perror ("Eroare la read() de la client.\n");
	}

	char requested_conversion[200]={0};

	strcpy(requested_conversion,input_file_extension);
	strcat(requested_conversion,"2");
	strcat(requested_conversion,output_file_extension);

	printf("Requested conversion: %s\n",requested_conversion);
	fflush(stdout);

	//facem convertirea


		char serverTemporaryFile[200]={0};
		strcpy(serverTemporaryFile,"./auxFiles/serverTemporaryFile");
		strcat(serverTemporaryFile,".");
		strcat(serverTemporaryFile,input_file_extension);

	
		char OutputFile[200]={0};
		
		strcpy(OutputFile,"./auxFiles/outputFile");
		strcat(OutputFile,".");

		if(strcmp(output_file_extension,"html")==0)
		{
			strcpy(OutputFile,"./auxFiles/yourFile.zip");
		}
		else
		{
			strcat(OutputFile,output_file_extension);
		}

		printf("Files:\n %s\n%s\n",serverTemporaryFile,OutputFile);
		fflush(stdout);

		int received=0;
		size_t chunk;
		char buffer[CHUNK_SIZE];
		unsigned long documentSize;

		FILE* rez= fopen(serverTemporaryFile,"wb");
		
		if(rez==NULL){
			perror("fopen");
			return errno;
			exit(2);
		}
 		
 		//am primit de la client lungimea totala a fisierului
 		if (read (tdL.cl, &documentSize,sizeof(unsigned long)) <= 0){
		
			printf("[Thread %d]\n",tdL.idThread);
			perror ("Eroare la read() de la client.\n");
		}

		do{

			if (read (tdL.cl, &chunk,sizeof(size_t)) <= 0){
		
				printf("[Thread %d]\n",tdL.idThread);
				perror ("Eroare la read() de la client.\n");
			}

			if(chunk<=0) break;

			if (read (tdL.cl, &buffer,sizeof(buffer)) <= 0){
		
				printf("[Thread %d]\n",tdL.idThread);
				perror ("Eroare la read() de la client.\n");
			}

			fwrite(buffer,1,sizeof(buffer),rez);

			received+=chunk;


		}while(received<documentSize);


		fclose(rez);
 

		//convertirea efectiva

		char command[256]={0};

		snprintf(command,256,"%s %s %s",checkConversion(requested_conversion),serverTemporaryFile,OutputFile);

		printf("command: %s\n",command);
		fflush(stdout);

		system(command);

		unlink(serverTemporaryFile);

		if(strcmp(output_file_extension,"html")==0){

			char command[256]={0};

			system("zip -r yourFile.zip ./auxFiles");
			system("mv yourFile.zip ./auxFiles");


			printf("succesfully zipped!\n");
			
			fflush(stdout); 
		}


		int downloadFlag=1;

		if(write (tdL.cl,&downloadFlag,sizeof(int)) <= 0){
    	  	
    	  	printf("[Thread %d] ",tdL.idThread);
			perror ("[Thread]Eroare la write() catre client.\n");
		}

		//trimitem fisierul convertit la client



		FILE* finalDocument=fopen(OutputFile,"rb");

		if(finalDocument==NULL){

	  		perror("fopen error");
	  	
	  		return errno;
	  		exit(1);
		}


		// setam cursorul la inceputul fisierului pentru a putea masura dimensiunea totala a fisierului
	 	fseek(finalDocument,0,SEEK_END);
	
		unsigned long finalDocumentSize=ftell(finalDocument);

		rewind(finalDocument);

		//trimitem clientului lungimea totala a fisierului
    	if (write (tdL.cl,&finalDocumentSize,sizeof(unsigned long)) <= 0){
    	  	
    	  	printf("[Thread %d] ",tdL.idThread);
			perror ("[Thread]Eroare la write() catre client.\n");
    	}

    	
		int sent=0;	

  		do{

  			chunk = fread(buffer,1,sizeof(buffer),finalDocument);
  	
  			if(chunk <=0 ) break;

  			if (write (tdL.cl,&chunk,sizeof(size_t)) <= 0){
    	  	
    	  		printf("[Thread %d] ",tdL.idThread);
				perror ("[Thread]Eroare la write() catre client.\n");
    		
    		}

    		if (write (tdL.cl,&buffer,sizeof(buffer)) <= 0){
    	  	
    	  		printf("[Thread %d] ",tdL.idThread);
				perror ("[Thread]Eroare la write() catre client.\n");
    		
    		}

  	
  			sent+=chunk;


  		}while(sent<finalDocumentSize);

  		fclose(finalDocument);
  		unlink(OutputFile); 
  		system("rm -r ./auxFiles/*");


  }//while


}

char* checkConversion(char argument[100]){

	char valid_conversion[100];
	char *result=malloc(sizeof(char)*2*strlen(valid_conversion));

	FILE* conversions = fopen("conversions.txt","r");
	
	if(conversions == NULL){
		perror("fopen error");
		return (char*)errno;
	}

	while (EOF != fscanf(conversions, "%s  %s\n", valid_conversion,result)){
			
		if (strcmp(valid_conversion, argument) == 0){


			return result;
		}

	}

	return NULL;

}
