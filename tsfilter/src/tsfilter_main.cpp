/*

MPEG Transport Stream Filter & PID analysis


Usage:
    For Local File input =>    tsfilter -inputfile filename.ts    +pid1 +pid2 ... +pid5 read_buffer_size
    For Multicast input  =>    tsfilter -inputudp  225.0.0.1 1234 +pid1 +pid2 ... +pid5 read_buffer_size

                               +pid keeps 'pid' packets

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "logger.h"
#include "common.h"
#include "devices.h"

#define UDP_MAXIMUM_SIZE 65535 /* theoretical maximum size */
#define TS_PACKET_SIZE 188
#define MAX_PID 8192

#define OUTPUT_TEST 1

void Usage(void) {
    printf("Usage: tsfilter -inputfile filename.ts    +pid1 +pid2 ... +pid5 read_buffer_size\n");
    printf("Usage: tsfilter -inputudp  225.0.0.1 1234 +pid1 +pid2 ... +pid5 read_buffer_size\n");
    printf("+pid keeps 'pid' packets\n");
}

int device_test_main(int argc, char* argv[]) {
    InputDevice *devIn = NULL;
    devIn = new FileInDevice("../AC3-5.1-384Kbps-48Khz_audioOnly.ts");
    printf("devIn= %p\n", devIn);
    delete(devIn);
    return 0;
}

int main(int argc, char* argv[]) {

    LOG_DECLARE();
    InputDevice  *devIn = NULL;
    int byte_read;
#if OUTPUT_TEST
    OutputDevice *devOut = NULL;
    int byte_wrote;
#endif
    unsigned char udp_packet[UDP_MAXIMUM_SIZE];

    unsigned char* packet_buffer;
    unsigned char* current_packet;
    unsigned int buffer_size;

    unsigned char pid_table[MAX_PID];	/* valid PID table */
    unsigned short pid_limit = 0;
    unsigned short pid;
    unsigned int i;

    //Validation of arguments
    if (argc < 3) {
        LOG_PRINT_MSG(LOG_WARN, "Invalid argments passed\n");    //Less than minimum expected arguments count
        Usage();
        return 0;

    } else if (strcmp(argv[1], "-inputfile") == 0) {
        ALLOC_POINTER_ARGS(devIn, FileInDevice, argv[2]);
        i = 3;

        buffer_size = 1;
        buffer_size *= TS_PACKET_SIZE*7;

    } else if (strcmp(argv[1], "-inputudp") == 0 && argc >= 4) {
        ALLOC_POINTER_ARGS(devIn, MulticastInDevice, AF_INET, atoi(argv[3]), argv[2]);
        i =4;
		
        buffer_size = UDP_MAXIMUM_SIZE;

    } else {
        LOG_PRINT_MSG(LOG_WARN, "Invalid argments passed\n");    //Invalid input argument or less arguments count
        Usage();
        return 2;
    }

    if (i > (argc-1)) {
        LOG_PRINT_MSG(LOG_WARN, "Invalid argments\n");    //Needs atleast a pid to filter or read buffer size
        Usage();
        return 2;
    }

    while (i < argc) {
        if (argv[i][0] == '+') {
            if( pid_limit < 5 ) {
                pid =  atoi(&(argv[i][1]));
                if ( pid < MAX_PID) {
                    LOG_PRINT_FORMAT(LOG_INFO, "Keeping pid %d\n", pid);
                    pid_table[pid] = 1;
                    i++;
                } else {
                    LOG_PRINT_FORMAT(LOG_WARN, "Pid range should be from 0 to %d\n", MAX_PID);
                    return 2;
                }
                ++pid_limit;
            }
            else {
                LOG_PRINT_FORMAT(LOG_INFO, "Considering only first pids. Skipping pid data %s\n", &(argv[i][1]));    //Considering only first 5 PIDs
                ++i;
            }
        } else {
            buffer_size = atoi(&(argv[i][0]));
            i++;
        }
    }


    if( devIn == NULL ) {
        return 2;
    }	
    if ( devIn->Open() != RET_OK ) {
        DEALLOC_POINTER(devIn);
        return 2;
    }
	
#if OUTPUT_TEST
    ALLOC_POINTER_ARGS(devOut, FileOutDevice, "output.ts");
    if (devOut == NULL) {
        devIn->Close();
        DEALLOC_POINTER(devIn);		
		
        return 2;
    }
    if (devOut->Open() != RET_OK) {
        DEALLOC_POINTER(devOut);		
		
        devIn->Close();
        DEALLOC_POINTER(devIn);		
		
        return 2;
    }
#endif

    ALLOC_ARRAY(packet_buffer, unsigned char, buffer_size);
    if (packet_buffer == NULL) {
        LOG_PRINT_MSG(LOG_ERROR, "Out of memory\n");
        goto EXIT_PROG;
    }
	
    while (1) {
        byte_read = devIn->Read(packet_buffer, buffer_size);
		
#if OUTPUT_TEST
        if (byte_read < 0) {
            LOG_PRINT_FORMAT(LOG_ERROR, "InputDevice Read error byte_read= %d\n", byte_read);

        } else if( byte_read > 0 ) {
            byte_wrote = devOut->Write(packet_buffer, byte_read);

        }
#endif
        if (byte_read < 0)
            break;

        if (byte_read < buffer_size) {
            LOG_PRINT_FORMAT(LOG_TRACE, "Read bytes= %d less than buffer size= %d\n", byte_read, buffer_size);
        }

        if (byte_read % TS_PACKET_SIZE != 0) {
            LOG_PRINT_FORMAT(LOG_WARN, "Read bytes= %d not multiple of %d bytes\n", byte_read, TS_PACKET_SIZE);
        }

        for (i = 0 ; i < byte_read ; i += TS_PACKET_SIZE) {
            current_packet = packet_buffer + i;

            if (current_packet[0] != 0x47 ) {    //Sync Byte
                LOG_PRINT_MSG(LOG_WARN, "Missing Sync Byte\n");
            }
        }
    }
	
EXIT_PROG:
    devIn->Close();
#if OUTPUT_TEST
    devOut->Close();
#endif
	
    DEALLOC_POINTER(devIn);
#if OUTPUT_TEST
    DEALLOC_POINTER(devOut);
#endif
	
    return 0;
}

