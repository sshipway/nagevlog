/* header file for sending nsca message */

#ifdef  __cplusplus
extern "C" {
#endif

extern int send_nsca(char * server_name, int server_port,  char * svc_description, int status, char *message , char *emsg);

#ifdef  __cplusplus
}
#endif

