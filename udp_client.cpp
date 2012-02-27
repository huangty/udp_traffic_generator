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
#include <fcntl.h>
#include "MyThread.h"
#include "msg_info.h"
using namespace std;

#define MAXPAYLOAD  1458 //1500 - 14 - 20 - 8 = 1458
float delay = 0; //in ms, 1;
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
 	if(fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0){
    	perror("fcntl() error");
    	exit(-1);
  	}


	printf("Start sendout \n");

	char buffer[MAXPAYLOAD];
	struct timeval tim;
	unsigned long long int seqnumber = 0;	  

	double time_prev = 0;
	while(1){
	 seqnumber++;
	 gettimeofday(&tim, NULL);
	 double time_stamp=tim.tv_sec+(tim.tv_usec/1000000.0);
	 
// 	 if(time_prev != 0){
// 		 printf("send_time_diff: %lf ms\n",(time_stamp - time_prev)*1000);
// 	 }
	 memset(&buffer, '\0' , sizeof(buffer));
	 
	 MSG_INFO msg;
	 msg.seqnumber = seqnumber;
	 msg.time_stamp = time_stamp;
	 msg.payload_size = MAXPAYLOAD - sizeof(MSG_INFO);
	 
	 time_prev = time_stamp;
	 double time_start=tim.tv_sec+(tim.tv_usec/1000000.0); //in second
	 memcpy(buffer, (char *)&msg, sizeof(MSG_INFO));
	 int retries = 0;
	 while( sendto(sockfd, buffer, MAXPAYLOAD,0, (const struct sockaddr *) &servaddr, sizeof(servaddr)) == -1){
	   retries++;
	   if (errno !=  EAGAIN){
	     printf("Other Error (%d:%s)\n", errno,strerror(errno));
	     exit(1);
	   }
	 }
// 	     int microsecond_sleep = 1;
// 		printf("Sendto Error: ");
// 		if( errno == ENOBUFS){
// 			printf("ENOBUFS => Sleep for %dus, packet #%lld meets no buffer errors\n", microsecond_sleep, seqnumber);
// 		}else{
		
// 		  printf("Other Error (%d:%s)=> sleep for %dus anyway\n", errno,strerror(errno),microsecond_sleep);
// 		}
// 	 	// usleep(microsecond_sleep);		
// 		exit(1);
//	 }
	 if( seqnumber % 500 == 0){
	   printf("send out packet %llu after %d retries\n",seqnumber,retries);
	 }

	 double space = delay; //switch to second, default sleep 4ms (i.e., 3Mbps)
	 space/= 1000.0;
	 
	 gettimeofday(&tim, NULL);
	 double time_now;
	 while(1){
	   gettimeofday(&tim, NULL);
	   time_now = tim.tv_sec+(tim.tv_usec/1000000.0) ;
	   if( (time_now - time_start) > space)
		 break;
	 }
	 if (seqnumber == 5000){
	   //system("hercules2.sh");
	   printf("changing interface!!\n");
	   system("sudo iwconfig ath3 essid herculesap2 ap 00:80:48:5F:35:9A");
	   sleep(10);
	 }

	}

  return 0;
}
