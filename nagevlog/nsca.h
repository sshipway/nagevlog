typedef short int16_t;
#include <stdio.h>

/*---------  config.h ---------------*/

#define NSCA_SERVER_PORT     5667     /* default port to use */


#define STDC_HEADERS 1
/* #undef HAVE_SYSLOG_H 0 */
#define HAVE_STRDUP 1
#define HAVE_STRSTR 1
#define HAVE_STRTOUL 1
/* #undef HAVE_INITGROUPS */
/* #undef HAVE_LIMITS_H */
#define HAVE_SYS_RESOURCE_H 1

#define SOCKET_SIZE_TYPE size_t
#define GETGROUPS_T gid_t
#define RETSIGTYPE void


#define SIZEOF_INT 4
#define SIZEOF_SHORT 2
#define SIZEOF_LONG 4

/* stupid stuff for u_int32_t */
/* #undef U_INT32_T_IS_USHORT */
/* #undef U_INT32_T_IS_UINT */
 #define U_INT32_T_IS_ULONG 
/* #undef U_INT32_T_IS_UINT32_T */

#ifdef U_INT32_T_IS_USHORT
typedef unsigned short u_int32_t;
#endif
#ifdef U_INT32_T_IS_ULONG
typedef unsigned long u_int32_t;
#endif
#ifdef U_INT32_T_IS_UINT
typedef unsigned int u_int32_t;
#endif
#ifdef U_INT32_T_IS_UINT32_t
typedef uint32_t u_int32_t;
#endif
/* stupid stuff for int32_t */
/* #undef INT32_T_IS_SHORT */
/* #undef INT32_T_IS_INT */
 #define INT32_T_IS_LONG 

#ifdef INT32_T_IS_USHORT
typedef short int32_t;
#endif
#ifdef INT32_T_IS_ULONG
typedef long int32_t;
#endif
#ifdef INT32_T_IS_UINT
typedef int int32_t;
#endif

#include <malloc.h>
#include <memory.h>

/* #define HAVE_STRINGS_H  */
#define HAVE_STRING_H 1
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/* #undefine HAVE_UNISTD_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


#define HAVE_SIGNAL_H 1
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

/* #define HAVE_SYSLOG_H */
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

/* #define HAVE_SYS_INT_TYPES_H  */
#ifdef HAVE_SYS_INT_TYPES_H
#include <sys/int_types.h>
#endif
#define HAVE_SYS_STAT_H 1
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#define HAVE_FCNTL_H 1
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

/* #define HAVE_STDINT_H 1 */
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/* #define HAVE_SYS_POLL_H 1 */
#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif

#define HAVE_SYS_TYPES_H 1
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* #define HAVE_SYS_WAIT_H 1 */
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#define HAVE_ERRNO_H 1
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
/* needed for the time_t structures we use later... */
#define TIME_WITH_SYS_TIME 0
#define HAVE_SYS_TIME_H 0
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif


/* #define HAVE_SYS_SOCKET_H 1 */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef WINDOWS_NT
#include <winsock.h>
#else
#include <winsock2.h>
#endif

/* #undef HAVE_SOCKET */
#ifdef HAVE_SOCKET_H
#include <socket.h>
#endif

/* #define HAVE_NETINET_IN_H 1 */
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

/* #define HAVE_ARPA_INET_H 1 */
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

/* #define HAVE_NETDB_H 1 */
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#define HAVE_CTYPE_H 1
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#define HAVE_LIBMCRYPT
#ifdef HAVE_LIBMCRYPT
#include "mcrypt.h"
#endif

/* #define HAVE_DB_H 1 */
#ifdef HAVE_DB_H
#include <db.h>
#endif
 
/* #define HAVE_PWD_H 1 */
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

/* #define HAVE_GRP_H 1 */
#ifdef HAVE_GRP_H
#include <grp.h>
#endif

/* #define HAVE_INTTYPES_H 1 */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/* #define HAVE_SYS_CONFIG_H 1 */
#ifdef HAVE_SYS_CONFIG_H
#include <sys/config.h>
#endif

/* #define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1 */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#endif

#define bzero(A,B)	memset(A,0,B)

/* --------------------- common.h ------------------------ */


#define OK              0
#define NSCA_ERROR           -1

#define TRUE            1
#define FALSE           0

#define STATE_UNKNOWN   3       /* service state return codes */
#define STATE_CRITICAL  2
#define STATE_WARNING   1
#define STATE_OK        0


#define DEFAULT_SOCKET_TIMEOUT  10      /* timeout after 10 seconds */

#define MAX_INPUT_BUFFER        2048    /* max size of most buffers we use */

#define MAX_HOST_ADDRESS_LENGTH 256     /* max size of a host address */

#define MAX_HOSTNAME_LENGTH     64
#define MAX_DESCRIPTION_LENGTH  128
#define MAX_PLUGINOUTPUT_LENGTH 512

#define MAX_PASSWORD_LENGTH     512


/********************* ENCRYPTION TYPES ****************/

#define ENCRYPT_NONE            0       /* no encryption */
#define ENCRYPT_XOR             1       /* not really encrypted, just obfuscated
 */

#ifdef HAVE_LIBMCRYPT
#define ENCRYPT_DES             2       /* DES */
#define ENCRYPT_3DES            3       /* 3DES or Triple DES */
#define ENCRYPT_CAST128         4       /* CAST-128 */
#define ENCRYPT_CAST256         5       /* CAST-256 */
#define ENCRYPT_XTEA            6       /* xTEA */
#define ENCRYPT_3WAY            7       /* 3-WAY */
#define ENCRYPT_BLOWFISH        8       /* SKIPJACK */
#define ENCRYPT_TWOFISH         9       /* TWOFISH */
#define ENCRYPT_LOKI97          10      /* LOKI97 */

#define ENCRYPT_RC2             11      /* RC2 */
#define ENCRYPT_ARCFOUR         12      /* RC4 */
#define ENCRYPT_RC6             13      /* RC6 */            /* UNUSED */
#define ENCRYPT_RIJNDAEL128     14      /* RIJNDAEL-128 */
#define ENCRYPT_RIJNDAEL192     15      /* RIJNDAEL-192 */
#define ENCRYPT_RIJNDAEL256     16      /* RIJNDAEL-256 */
#define ENCRYPT_MARS            17      /* MARS */           /* UNUSED */
#define ENCRYPT_PANAMA          18      /* PANAMA */         /* UNUSED */
#define ENCRYPT_WAKE            19      /* WAKE */
#define ENCRYPT_SERPENT         20      /* SERPENT */
#define ENCRYPT_IDEA            21      /* IDEA */           /* UNUSED */
#define ENCRYPT_ENIGMA          22      /* ENIGMA (Unix crypt) */
#define ENCRYPT_GOST            23      /* GOST */
#define ENCRYPT_SAFER64         24      /* SAFER-sk64 */
#define ENCRYPT_SAFER128        25      /* SAFER-sk128 */
#define ENCRYPT_SAFERPLUS       26      /* SAFER+ */
#endif

/******************** MISC DEFINITIONS *****************/

#define TRANSMITTED_IV_SIZE     128     /* size of IV to transmit - must be as b
ig as largest IV needed for any crypto algorithm */


/*************** PACKET STRUCTURE DEFINITIONS **********/

#define NSCA_PACKET_VERSION_3   3               /* packet version identifier */
#define NSCA_PACKET_VERSION_2   2               /* older packet version identifi
ers */
#define NSCA_PACKET_VERSION_1   1

/* data packet containing service check results */
typedef struct data_packet_struct{
        int16_t   packet_version;
        u_int32_t crc32_value;
        u_int32_t timestamp;
        int16_t   return_code;
        char      host_name[MAX_HOSTNAME_LENGTH];
        char      svc_description[MAX_DESCRIPTION_LENGTH];
        char      plugin_output[MAX_PLUGINOUTPUT_LENGTH];
        }data_packet;


/* initialization packet containing IV and timestamp */
typedef struct init_packet_struct{
        char      iv[TRANSMITTED_IV_SIZE];
        u_int32_t timestamp;
        }init_packet;




/*---------------------- utils.h ----------------------- */

struct crypt_instance {
        char transmitted_iv[TRANSMITTED_IV_SIZE];
#ifdef HAVE_LIBMCRYPT
        MCRYPT td;
        char *key;
        char *IV;
        char block_buffer;
        int blocksize;
        int keysize;
        char *mcrypt_algorithm;
        char *mcrypt_mode;
#endif
        };

void generate_crc32_table(void);
unsigned long calculate_crc32(char *, int);

int encrypt_init(char *,int,char *,struct crypt_instance **);
void encrypt_cleanup(int,struct crypt_instance *);

static void generate_transmitted_iv(char *transmitted_iv);

void encrypt_buffer(char *,int,char *,int,struct crypt_instance *);
void decrypt_buffer(char *,int,char *,int,struct crypt_instance *);

void randomize_buffer(char *,int);

void strip(char *);

void clear_buffer(char *,int);

void display_license(void);

/* ---------------- netutils.h ------------- */

int my_tcp_connect(char *,int,SOCKET *);
int my_connect(char *,int,SOCKET *,char *);

int my_inet_aton(register const char *,struct in_addr *);

int sendall(SOCKET,char *,int *,int);
int recvall(SOCKET,char *,int *,int);


