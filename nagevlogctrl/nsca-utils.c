/****************************************************************************
 *
 * UTILS.C - Utility functions for NSCA
 *
 * License: GPL
 * Copyright (c) 2000-2002 Ethan Galstad (nagios@nagios.org)
 *
 * Last Modified: 10-09-2002
 *
 * Description:
 *
 * This file contains common unctions used in nsca and send_nsca
 *
 * License Information:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ****************************************************************************/
#define _CRT_SECURE_NO_DEPRECATE


#include "nsca.h"

/*
extern void ntsl_log_error(char *, ...);
extern void ntsl_log_warning(char *, ...);
extern void ntsl_log_info(char *, ...);
*/


/*#define NSCADEBUG*/

static unsigned long crc32_table[256];
static int created_crc = 0;



/* build the crc table - must be called before calculating the crc value */
void generate_crc32_table(void){
	unsigned long crc, poly;
	int i, j;

	if( created_crc == 9999 ) { return; }
	created_crc = 9999;

	poly=0xEDB88320L;
	for(i=0;i<256;i++){
		crc=i;
		for(j=8;j>0;j--){
			if(crc & 1)
				crc=(crc>>1)^poly;
			else
				crc>>=1;
		        }
		crc32_table[i]=crc;
                }

	return;
        }


/* calculates the CRC 32 value for a buffer */
unsigned long calculate_crc32(char *buffer, int buffer_size){
	register unsigned long crc;
	int this_char;
	int current_index;

	crc=0xFFFFFFFF;

	for(current_index=0;current_index<buffer_size;current_index++){
		this_char=(int)buffer[current_index];
		crc=((crc>>8) & 0x00FFFFFF) ^ crc32_table[(crc ^ this_char) & 0xFF];
	        }

	return (crc ^ 0xFFFFFFFF);
        }



/* initializes encryption routines */
int encrypt_init(char *password,int encryption_method,char *received_iv,struct crypt_instance **CIptr){
#ifdef HAVE_LIBMCRYPT
        int i;
        int iv_size;
#endif
        struct crypt_instance *LCI;

        LCI=malloc(sizeof(struct crypt_instance));
        *CIptr=LCI;

        if(LCI==NULL){
/*                ntsl_log_error( "Could not allocate memory for crypt instance"); */
                return NSCA_ERROR;
                }
		bzero(LCI,sizeof(struct crypt_instance));

	/* server generates IV used for encryption */
        if(received_iv==NULL)
                generate_transmitted_iv(LCI->transmitted_iv);

	/* client recieves IV from server */
        else
                memcpy(LCI->transmitted_iv,received_iv,TRANSMITTED_IV_SIZE);

#ifdef HAVE_LIBMCRYPT
        LCI->blocksize=1;                        /* block size = 1 byte w/ CFB mode */
        LCI->keysize=7;                          /* default to 56 bit key length */
        LCI->mcrypt_mode="cfb";                  /* CFB = 8-bit cipher-feedback mode */
        LCI->mcrypt_algorithm="unknown";
#endif
		LCI->IV = (char *)0;
		LCI->key = (char *)0;

        /* XOR or no encryption */
        if(encryption_method==ENCRYPT_NONE || encryption_method==ENCRYPT_XOR)
                return OK;

#ifdef HAVE_LIBMCRYPT

        /* get the name of the mcrypt encryption algorithm to use */
        switch(encryption_method){
        case ENCRYPT_DES:
                LCI->mcrypt_algorithm=MCRYPT_DES;
                break;
        case ENCRYPT_3DES:
                LCI->mcrypt_algorithm=MCRYPT_3DES;
                break;
        case ENCRYPT_CAST128:
                LCI->mcrypt_algorithm=MCRYPT_CAST_128;
                break;
        case ENCRYPT_CAST256:
                LCI->mcrypt_algorithm=MCRYPT_CAST_256;
                break;
        case ENCRYPT_XTEA:
                LCI->mcrypt_algorithm=MCRYPT_XTEA;
                break;
        case ENCRYPT_3WAY:
                LCI->mcrypt_algorithm=MCRYPT_3WAY;
                break;
        case ENCRYPT_BLOWFISH:
                LCI->mcrypt_algorithm=MCRYPT_BLOWFISH;
                break;
        case ENCRYPT_TWOFISH:
                LCI->mcrypt_algorithm=MCRYPT_TWOFISH;
                break;
        case ENCRYPT_LOKI97:
                LCI->mcrypt_algorithm=MCRYPT_LOKI97;
                break;
        case ENCRYPT_RC2:
                LCI->mcrypt_algorithm=MCRYPT_RC2;
                break;
        case ENCRYPT_ARCFOUR:
                LCI->mcrypt_algorithm=MCRYPT_ARCFOUR;
                break;
        case ENCRYPT_RIJNDAEL128:
                LCI->mcrypt_algorithm=MCRYPT_RIJNDAEL_128;
                break;
        case ENCRYPT_RIJNDAEL192:
                LCI->mcrypt_algorithm=MCRYPT_RIJNDAEL_192;
                break;
        case ENCRYPT_RIJNDAEL256:
                LCI->mcrypt_algorithm=MCRYPT_RIJNDAEL_256;
                break;
        case ENCRYPT_WAKE:
                LCI->mcrypt_algorithm=MCRYPT_WAKE;
                break;
        case ENCRYPT_SERPENT:
                LCI->mcrypt_algorithm=MCRYPT_SERPENT;
                break;
        case ENCRYPT_ENIGMA:
                LCI->mcrypt_algorithm=MCRYPT_ENIGMA;
                break;
        case ENCRYPT_GOST:
                LCI->mcrypt_algorithm=MCRYPT_GOST;
                break;
        case ENCRYPT_SAFER64:
                LCI->mcrypt_algorithm=MCRYPT_SAFER_SK64;
                break;
        case ENCRYPT_SAFER128:
                LCI->mcrypt_algorithm=MCRYPT_SAFER_SK128;
                break;
        case ENCRYPT_SAFERPLUS:
                LCI->mcrypt_algorithm=MCRYPT_SAFERPLUS;
                break;

        default:
                LCI->mcrypt_algorithm="unknown";
                break;
                }

#ifdef NSCADEBUG
        ntsl_log_info("Attempting to initialize '%s' crypto algorithm...",LCI->mcrypt_algorithm);
#endif

        /* open encryption module */
        if((LCI->td=mcrypt_module_open(LCI->mcrypt_algorithm,NULL,LCI->mcrypt_mode,NULL))==MCRYPT_FAILED){
/*                ntsl_log_error("Could not open mcrypt algorithm '%s' with mode '%s'",CI->mcrypt_algorithm,CI->mcrypt_mode); */
                return NSCA_ERROR;
                }

#ifdef NSCADEBUG
        ntsl_log_info("Using '%s' as crypto algorithm...",LCI->mcrypt_algorithm);
#endif

        /* determine size of IV buffer for this algorithm */
        iv_size=mcrypt_enc_get_iv_size(LCI->td);
        if(iv_size>TRANSMITTED_IV_SIZE){
/*                ntsl_log_error("IV size for crypto algorithm exceeds limits"); */
                return NSCA_ERROR;
                }

        /* allocate memory for IV buffer */
        if((LCI->IV=(char *)malloc(iv_size))==NULL){
/*                ntsl_log_error("Could not allocate memory for IV buffer"); */
                return NSCA_ERROR;
                }

        /* fill IV buffer with first bytes of IV that is going to be used to crypt (determined by server) */
        for(i=0;i<iv_size;i++)
                LCI->IV[i]=LCI->transmitted_iv[i];

        /* get maximum key size for this algorithm */
        LCI->keysize=mcrypt_enc_get_key_size(LCI->td);

        /* generate an encryption/decription key using the password */
        if((LCI->key=(char *)malloc(LCI->keysize))==NULL){
/*                ntsl_log_error("Could not allocate memory for encryption/decryption key"); */
                return NSCA_ERROR;
                }
        bzero(LCI->key,LCI->keysize);

        if(LCI->keysize<(int)strlen(password))
                strncpy(LCI->key,password,LCI->keysize);
        else
                strncpy(LCI->key,password,strlen(password));
        
        /* initialize encryption buffers */
        mcrypt_generic_init(LCI->td,LCI->key,LCI->keysize,LCI->IV);

#endif

        return OK;
        }



/* encryption routine cleanup */
void encrypt_cleanup(int encryption_method, struct crypt_instance *CI){

	/* no crypt instance */
	if(CI==NULL)
		return;

#ifdef HAVE_LIBMCRYPT
        /* mcrypt cleanup */
        if(encryption_method!=ENCRYPT_NONE && encryption_method!=ENCRYPT_XOR)
                mcrypt_generic_end(CI->td);


        free(CI->key);
		free(CI->IV);
#endif
		free(CI);

        return;
        }



/* generates IV to use for encrypted communications (function is called by server only, client uses IV it receives from server) */
static void generate_transmitted_iv(char *transmitted_iv){
        
        int x;
        int seed=0;

        /*********************************************************/
        /* fill IV buffer with data that's as random as possible */ 
        /*********************************************************/
        /* else fallback to using the current time as the seed */
      
                seed=(int)time(NULL);

        /* generate pseudo-random IV */
        srand(seed);
        for(x=0;x<TRANSMITTED_IV_SIZE;x++)
                transmitted_iv[x]=(char)((int)((256.0*rand())/(RAND_MAX+1.0)));

        return;
        }



/* encrypt a buffer */
void encrypt_buffer(char *buffer,int buffer_size, char *password, int encryption_method, struct crypt_instance *CI){
        int x;
        int y;
        int password_length;

#ifdef NSCADEBUG
        ntsl_log_info("Encrypting with algorithm #%d",encryption_method);
#endif

	/* no crypt instance */
	if(CI==NULL)
		return;

        /* no encryption */
        if(encryption_method==ENCRYPT_NONE)
                return;

        /* simple XOR "encryption" - not meant for any real security, just obfuscates data, but its fast... */
        else if(encryption_method==ENCRYPT_XOR){

                /* rotate over IV we received from the server... */
                for(y=0,x=0;y<buffer_size;y++,x++){

                        /* keep rotating over IV */
                        if(x>=TRANSMITTED_IV_SIZE)
                                x=0;

                        buffer[y]^=CI->transmitted_iv[x];
                        }

                /* rotate over password... */
                password_length=strlen(password);
                for(y=0,x=0;y<buffer_size;y++,x++){

                        /* keep rotating over password */
                        if(x>=password_length)
                                x=0;

                        buffer[y]^=password[x];
                        }

                return;
                }

#ifdef HAVE_LIBMCRYPT
        /* use mcrypt routines */
        else{

                /* encrypt each byte of buffer, one byte at a time (CFB mode) */
                for(x=0;x<buffer_size;x++)
                        mcrypt_generic(CI->td,&buffer[x],1);
                }
#endif

        return;
        }


/* decrypt a buffer */
void decrypt_buffer(char *buffer,int buffer_size, char *password, int encryption_method, struct crypt_instance *CI){
        int x=0;

#ifdef NSCADEBUG
        ntsl_log_info("Decrypting with algorithm #%d",encryption_method);
#endif

	/* no crypt instance */
	if(CI==NULL)
		return;

        /* no encryption */
        if(encryption_method==ENCRYPT_NONE)
                return;

        /* XOR "decryption" is the same as encryption */
        else if(encryption_method==ENCRYPT_XOR)
                encrypt_buffer(buffer,buffer_size,password,encryption_method,CI);

#ifdef HAVE_LIBMCRYPT
        /* use mcrypt routines */
        else{

                /* encrypt each byte of buffer, one byte at a time (CFB mode) */
                for(x=0;x<buffer_size;x++)
                        mdecrypt_generic(CI->td,&buffer[x],1);
                }
#endif

        return;
        }



/* fill a buffer with semi-random data */
void randomize_buffer(char *buffer,int buffer_size){
	
	int x;
	int seed;

	/**** FILL BUFFER WITH RANDOM ALPHA-NUMERIC CHARACTERS ****/

	/***************************************************************
	   Only use alpha-numeric characters becase plugins usually
	   only generate numbers and letters in their output.  We
	   want the buffer to contain the same set of characters as
	   plugins, so its harder to distinguish where the real output
	   ends and the rest of the buffer (padded randomly) starts.
	***************************************************************/


	/* else fallback to using the current time as the seed */

		seed=(int)time(NULL);

	srand(seed);
	for(x=0;x<buffer_size;x++)
		buffer[x]=(char)((int)'0'+(int)(72.0*rand()/(RAND_MAX+1.0)));

	return;
        }



/* strips trailing newlines, carriage returns, spaces, and tabs from a string */
void strip(char *buffer){
	int x;
	int index;

	for(x=strlen(buffer);x>=1;x--){
		index=x-1;
		if(buffer[index]==' ' || buffer[index]=='\r' || buffer[index]=='\n' || buffer[index]=='\t')
			buffer[index]='\x0';
		else
			break;
	        }

	return;
        }



/* wipes an area of memory clean */
void clear_buffer(char *buffer, int buffer_length){

	/* NULL all bytes of buffer */
	memset(buffer,'\x0',buffer_length);

	return;
        }


