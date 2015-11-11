/* UDP receiving message test */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#include <rip_main.h>

int main(int argc, char**argv)
{
    int sockfd,n,i,recv_number;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   memset(&servaddr,0,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(8080);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   for (;;)
   {
       len = sizeof(cliaddr);
       n = recvfrom(sockfd,message, MAXROUTE * message_entry_t_len, 0,
		    (struct sockaddr *)&cliaddr,&len);
       recv_number = n / message_entry_t_len;
       for (i = 0; i < recv_number; i++) {
	   printf ("IP = %s Cost = %d\n", 
		   rip_net_inet_ntop (message[i].dest_addr), message[i].cost);
       }
   }
}

