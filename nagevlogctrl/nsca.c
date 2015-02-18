//**********************************************************************************
// *
//	nsca.c
//	Modified from send_nsca.c by Ethan Galstead.
//
//  Instead of being a standalone program, this now is a separate
//  function to send an NSCA message as per parameters.
//  
//	*
// *********************************************************************************/
 
/* #define NSCADEBUG 1 */
#define _CRT_SECURE_NO_DEPRECATE

#include "nsca.h" 

/*#include "event.h"*/
#include "string.h"
#include <winnt.h>

#include "ntsl.h" 

#define REG_BUFFER_LEN	2048

time_t start_time,end_time;

char password[MAX_INPUT_BUFFER]="";
char nscahostname[MAX_INPUT_BUFFER]="";

char received_iv[TRANSMITTED_IV_SIZE];

int socket_timeout=DEFAULT_SOCKET_TIMEOUT;

int warning_time=0;
int check_warning_time=FALSE;
int critical_time=0;
int check_critical_time=FALSE;
int encryption_method=ENCRYPT_XOR;
time_t packet_timestamp;
struct crypt_instance *CI=NULL;
int debugmode = 0;
int logalerts = 0;

int show_help=FALSE;
int show_license=FALSE;
int show_version=FALSE;

int read_config();
int read_init_packet(int);
void alarm_handler(int);
void clear_password(void);
static void do_cleanup();

/* This is the interface to the Eventlog funtions!
   Call send_nsca to send a message.
   This should init and close down Winsock
   If first call, should read registry to find out settings, and
   also set up the CRC table.
*/
int send_nsca(char * server_name, int server_port,  char * svc_description, int return_code, char * plugin_output , char *error_msg) {

	int sd;
	int rc;
	int result;
	data_packet send_packet;
	int bytes_to_send;
//	char input_buffer[MAX_INPUT_BUFFER];
//	char *temp_ptr;
	int total_packets=0;
	u_int32_t calculated_crc32;
//	char *ptr1, *ptr2, *ptr3, *ptr4;


	/* read the config from the registry (if required) */
	result=read_config();	


	/* generate the CRC 32 table (if required) */
	generate_crc32_table();

	/* initialize alarm signal handling */
//	signal(SIGALRM,alarm_handler);

	/* set socket timeout */
//	alarm(socket_timeout);

	time(&start_time);

	/* try to connect to the host at the given port number */
	result=my_tcp_connect(server_name,server_port,&sd);

	/* we couldn't connect */
	if(result!=STATE_OK){
		sprintf(error_msg,"Error: Could not connect to host %s on port %d (%d)\n",server_name,server_port,result);
		return 1;
	        }


	/* read the initialization packet containing the IV and timestamp */
	result=read_init_packet(sd);
	if(result!=OK){
	sprintf(error_msg,"Error: Could not read init packet from server (%d)",result);
		closesocket(sd);
		return 1;
	        }

#ifdef NSCADEBUG
	ntsl_log_info("Got init packet from server\n");
#endif

	/* initialize encryption/decryption routines with the IV we received from the server */
        if(encrypt_init(password,encryption_method,received_iv,&CI)!=OK){
	sprintf(error_msg,"Error: Failed to initialize encryption libraries for method %d",encryption_method);
		closesocket(sd);
		return 1;
	        }

#ifdef NSCADEBUG
	ntsl_log_info("Initialized encryption routines\n");
#endif


	/**** WE'RE CONNECTED AND READY TO SEND ****/

		/* increment count of packets we're sending */
		total_packets++;

		/* clear the packet buffer */
		memset(&send_packet,0,sizeof(send_packet));

		/* fill the packet with semi-random data */
		randomize_buffer((char *)&send_packet,sizeof(send_packet));

		/* copy the data we want to send into the packet */
		send_packet.packet_version=(int16_t)htons(NSCA_PACKET_VERSION_3);
		send_packet.return_code=(int16_t)htons((int16_t)return_code);
		strcpy(&send_packet.host_name[0],nscahostname);
		strcpy(&send_packet.svc_description[0],svc_description);
		strcpy(&send_packet.plugin_output[0],plugin_output);

		/* use timestamp provided by the server */
		send_packet.timestamp=(u_int32_t)htonl(packet_timestamp);

		/* calculate the crc 32 value of the packet */
		send_packet.crc32_value=(u_int32_t)0L;
		calculated_crc32=calculate_crc32((char *)&send_packet,sizeof(send_packet));
		send_packet.crc32_value=(u_int32_t)htonl(calculated_crc32);

		/* encrypt the packet */
		encrypt_buffer((char *)&send_packet,sizeof(send_packet),password,encryption_method,CI);

		/* send the packet */
		bytes_to_send=sizeof(send_packet);
		rc=sendall(sd,(char *)&send_packet,&bytes_to_send);

		/* there was an error sending the packet */
		if(rc==-1){
//			ntsl_log_error("Error: Could not send data to host\n");
			closesocket(sd);
			return 1;
	                }

		/* for some reason we didn't send all the bytes we were supposed to */
		else if(bytes_to_send<sizeof(send_packet)){
//			ntsl_log_warning("Warning: Sent only %d of %d bytes to host\n",rc,sizeof(send_packet));
			closesocket(sd);
			return 1;
		        }
	        

#ifdef NSCADEBUG
	ntsl_log_info("Done sending data\n");
#endif

	/* close the connection */
	closesocket(sd);

/*	if(debugmode) {
	ntsl_log_info("NSCA: %d data packet(s) sent to host successfully.\n",total_packets);
	}*/

	/* exit cleanly */
	do_cleanup();
	return 0;
}


/* exit */
static void do_cleanup(){

	/* reset the alarm */
//	alarm(0);

	/* encryption/decryption routine cleanup */
	encrypt_cleanup(encryption_method,CI); CI = NULL;

#ifdef NSCADEBUG
	ntsl_log_info("Cleaned up encryption routines\n");
#endif


}


/* reads initialization packet (containing IV and timestamp) from server */
int read_init_packet(int sock){
        int rc;
        init_packet receive_packet;
        int bytes_to_recv;

        /* clear the IV and timestamp */
        bzero(&received_iv,TRANSMITTED_IV_SIZE);
        packet_timestamp=(time_t)0;

        /* get the init packet from the server */
        bytes_to_recv=sizeof(receive_packet);
        rc=recvall(sock,(char *)&receive_packet,&bytes_to_recv,socket_timeout);

         /* recv() error or server disconnect */
        if(rc<=0){
/*                ntsl_log_error("Error: Server closed connection before init packet was received\n");
*/                return rc;
                }

        /* we couldn't read the correct amount of data, so bail out */
        else if(bytes_to_recv!=sizeof(receive_packet)){
/*                ntsl_log_error("Error: Init packet from server was too short (%d bytes received, %d expected)\n",bytes_to_recv,sizeof(receive_packet));
*/                return NSCA_ERROR;
                }

        /* transfer the IV and timestamp */
        memcpy(&received_iv,&receive_packet.iv[0],TRANSMITTED_IV_SIZE);
        packet_timestamp=(time_t)ntohl(receive_packet.timestamp);

        return OK;
        }




/* handle timeouts */
void alarm_handler(int sig){

/* "Error: Timeout after %d seconds\n",socket_timeout);
*/
        }



/* read in the configuration from the registry */
static int done_config = 0;

int read_config(){
	int rc, val,size;
	char buffer[REG_BUFFER_LEN];
	HKEY hReg;

	if( done_config == 9999 ) { return 0; }
	done_config = 9999;

   	_snprintf0(buffer, REG_BUFFER_LEN, "SOFTWARE\\%s\\Nagios", DEV_NAME);
   	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, buffer, 0, KEY_READ, &hReg);
   	if (rc != ERROR_SUCCESS) {
/*		ntsl_log_error("Unable to read registry key.");
*/		return NSCA_ERROR;
	}

	size = sizeof(DWORD);
	rc  = RegQueryValueEx(hReg, "DEBUG", NULL, NULL, (unsigned char*)&debugmode, (unsigned long *)&size);
	size = sizeof(DWORD);
	rc  = RegQueryValueEx(hReg, "logAlerts", NULL, NULL, (unsigned char*)&logalerts, (unsigned long *)&size);

	/* encryption method */
		size = sizeof(DWORD);
	rc  = RegQueryValueEx(hReg, "encryptionMethod", NULL, NULL, (unsigned char*)&val, (unsigned long *)&size);
   	if ( rc == ERROR_SUCCESS  ) {
			encryption_method = val;
	} else { encryption_method = 0; }

	/* password */
	size = sizeof(password);
	rc  = RegQueryValueEx(hReg, "nscaPassword", NULL, NULL, (unsigned char *)password, (unsigned long *)&size);

	/* my hostname for NSCA */
	size = sizeof(nscahostname);
	rc  = RegQueryValueEx(hReg, "MyHostname", NULL, NULL, (unsigned char *)nscahostname, (unsigned long *)&size);

	/* check encryption method */

			switch(encryption_method){
			case ENCRYPT_NONE:
				break;
			case ENCRYPT_XOR:
				break;

#ifdef HAVE_LIBMCRYPT
			case ENCRYPT_DES:
				break;
			case ENCRYPT_3DES:
				break;
			case ENCRYPT_CAST128:
				break;
			case ENCRYPT_CAST256:
				break;
			case ENCRYPT_XTEA:
				break;
			case ENCRYPT_3WAY:
				break;
			case ENCRYPT_BLOWFISH:
				break;
			case ENCRYPT_TWOFISH:
				break;
			case ENCRYPT_LOKI97:
				break;
			case ENCRYPT_RC2:
				break;
			case ENCRYPT_ARCFOUR:
				break;
			case ENCRYPT_RIJNDAEL128:
				break;
			case ENCRYPT_RIJNDAEL192:
				break;
			case ENCRYPT_RIJNDAEL256:
				break;
			case ENCRYPT_WAKE:
				break;
			case ENCRYPT_SERPENT:
				break;
			case ENCRYPT_ENIGMA:
				break;
			case ENCRYPT_GOST:
				break;
			case ENCRYPT_SAFER64:
				break;
			case ENCRYPT_SAFER128:
				break;
			case ENCRYPT_SAFERPLUS:
				break;
#endif
/*			default:
				ntsl_log_error("Invalid encryption method (%d) in registry\n",encryption_method);
#ifndef HAVE_LIBMCRYPT
				if(encryption_method>=2)
					ntsl_log_error("Client was not compiled with mcrypt library, so encryption is unavailable.\n");
#endif
*/			
			      
		        }

	RegCloseKey(hReg);

	return OK;
	}

