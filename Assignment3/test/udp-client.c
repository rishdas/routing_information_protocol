
/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

//#include <main.h>

int main(int argc, char**argv)
{
   int sockfd,n,i,a,b,c,d;
   struct sockaddr_in servaddr,cliaddr;
   char sendline[1000];
   char recvline[1000];
   char *ptr;

   if (argc != 2)
   {
      printf("usage:  udpcli <IP address>\n");
      exit(1);
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(argv[1]);
   servaddr.sin_port=htons(8080);

   i = 0x7A0B0CEF;
   sendline[0]  = ( (i & 0xFF000000)>>24);
   //sendline[0] = (char )a;
   sendline[1] = ( (i & 0x00FF0000)>>16);
   sendline[2] = ( (i & 0x0000FF00)>>8);
   sendline[3] = ( (i & 0x000000FF)); 
   printf ("i = %d\n",i);
   ptr = (sendline+4);
   strcpy (ptr,"GET /arq.txt\r\n");  

      sendto(sockfd,sendline,22,0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
/*      n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stdout);
/*
   while (fgets(sendline, 10000,stdin) != NULL)
   {
      sendto(sockfd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
      n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
      recvline[n]=0;
      fputs(recvline,stdout);
   }
*/
}
