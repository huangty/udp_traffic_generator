#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<sched.h>
#include<iostream>
#include "MyThread.h"
#include "msg_info.h"
using namespace std;

#define MAXPAYLOAD  1458 //1500 - 14 - 20 - 8 = 1458
int delay = 400;
MyThread recvAckThread;
MyThread sThread;
/*class for arguments used in thread function*/
class FUNC_ARG{ 
  public:
	int sockfd;
	const struct sockaddr *pservaddr;
	socklen_t servlen;

	FUNC_ARG(int sf, const struct sockaddr *ps, socklen_t sl){
	  sockfd = sf;
	  pservaddr = ps;
	  servlen = sl;
	}
	FUNC_ARG(int sf){
	  sockfd = sf;
	}
	FUNC_ARG(){
	}
};

/* Function used in receiving ack thread*/
void process_ack(void* param){
  FUNC_ARG *fa=(FUNC_ARG*)param;
  int sockfd = fa->sockfd;
  int n;
  char buffer[MAXPAYLOAD];
  MSG_INFO *msg;
  
  while(1){
  	memset(&buffer, '\0' , sizeof(buffer));
	n = recvfrom(sockfd, buffer, sizeof(MSG_INFO), 0, NULL, NULL);
	msg = (MSG_INFO*) buffer;
	printf("\nack: %llu %lf\n",msg->seqnumber, msg->time_stamp);
  }
}

/* Function used in sending out request
 *                ------------------------------------------
 * Packet Format: |seqnumber | time_stamp | delay | payload |
 *                ------------------------------------------  */
//void send_packet(int sockfd, const struct sockaddr *pservaddr, socklen_t servlen)
void send_packet(void* param)
{
  	  FUNC_ARG *fa=(FUNC_ARG*)param;
	  int sockfd = fa->sockfd;
	  const struct sockaddr *pservaddr = fa->pservaddr;
	  socklen_t servlen = fa->servlen;

	  char buffer[MAXPAYLOAD];
	  struct timeval tim;
	  unsigned long long int seqnumber = 0;	  

  	  /*Create Thread to process the ack*/
	  FUNC_ARG* f = new FUNC_ARG(sockfd);
	  recvAckThread.createThread((void*)process_ack, (void*)f);
	  /*End of Create Thread*/
	  
	  double time_prev = 0;
	  while(1){
		 seqnumber++;
		 gettimeofday(&tim, NULL);
		 double time_stamp=tim.tv_sec+(tim.tv_usec/1000000.0);
		 
		 if(time_prev != 0){
			 printf("send_time_diff: %lf ms\n",(time_stamp - time_prev)*1000);
		 }
		 memset(&buffer, '\0' , sizeof(buffer));
		 
		 MSG_INFO msg;
		 msg.seqnumber = seqnumber;
		 msg.time_stamp = time_stamp;
		 msg.payload_size = MAXPAYLOAD - sizeof(MSG_INFO);
		 
		 printf("send out packet %llu\n",seqnumber);
		 memcpy(buffer, (char *)&msg, sizeof(MSG_INFO));
		 sendto(sockfd, buffer, MAXPAYLOAD,0, pservaddr, servlen);

		 time_prev = time_stamp;
		 double space = delay; //switch to second, default sleep 40ms
		 space/= 1000;
		 
		 gettimeofday(&tim, NULL);
		 double time_start=tim.tv_sec+(tim.tv_usec/1000000.0); //in second
		 double time_now;
		 while(1){
		   gettimeofday(&tim, NULL);
		   time_now = tim.tv_sec+(tim.tv_usec/1000000.0) ;
		   if( (time_now - time_start) > space)
		     break;
		 }
	  }
}


int main(int argc, char **argv){
  int sockfd;
  struct sockaddr_in servaddr;
  struct hostent *host_ptr = 0;

  if (argc != 2){
         printf("usage: udpcli <RemoteIP>");
	 exit(1);
  }

  /* for scheduling */
  struct sched_param sched_param;
  sched_param.sched_priority = sched_get_priority_max (SCHED_RR);
  sched_setscheduler(0,SCHED_RR,& sched_param);
  /* end for scheduling */

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(9877);

  inet_pton(AF_INET, argv[1], &servaddr.sin_addr); 
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  printf("Start sendout \n");
  /*Create Thread to sendout request*/
  FUNC_ARG* stf = new FUNC_ARG(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  sThread.createThread((void*)send_packet, (void*)stf);
  /*End of Create Thread*/
 
  return 0;
}
