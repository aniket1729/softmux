#ifndef DEVICES_H
#define DEVICES_H

#include "common.h"

class Device
{
public:
    virtual ~Device() {}
    virtual int Init(void) = 0;
    virtual int Open(void) = 0;
    virtual int Close(void) = 0;
};

class InputDevice : public Device
{
public:
    virtual ~InputDevice() {}
    virtual int Read(unsigned char* buffer, int size) = 0;
};

class OutputDevice : public Device
{
public:
    virtual ~OutputDevice() {}
    virtual int Write(unsigned char* buffer, int size) = 0;
};


//----- FileInDevice -----
class FileInDevice : public InputDevice
{
    char *m_file_path;
    int m_fp_in;

public:
    FileInDevice(const char *file_path);
    virtual ~FileInDevice();
	
    int Init(void);
    int Open(void);
    int Close(void);
    int Read(unsigned char* buffer, int size);
};

//----- MulticastIntDevice -----
class MulticastInDevice : public InputDevice
{
    int m_af_type;
    int m_src_port;
    char *m_src_ip;

    int m_sock_in;

public:
    MulticastInDevice(int af_type, int src_port, const char *src_addr);
    virtual ~MulticastInDevice();
	
    int Init(void);
    int Open(void);
    int Close(void);
    int Read(unsigned char* buffer, int size);
};


//----- FileOutDevice -----
class FileOutDevice : public OutputDevice
{
    char *m_file_path;
    int m_fp_out;

public:
    FileOutDevice(const char *file_path);
    virtual ~FileOutDevice();
	
    int Init(void);
    int Open(void);
    int Close(void);
    int Write(unsigned char* buffer, int size);
};

//----- MulticastOutDevice -----
class MulticastOutDevice : public OutputDevice
{
	int m_af_type;
    struct sockaddr_in m_dest_addr;
    int m_sock_out;

public:
    MulticastOutDevice(int af_type, int dest_port, const char *dest_group);
    virtual ~MulticastOutDevice();
	
    int Init(void);
    int Open(void);
    int Close(void);
    int Write(unsigned char* buffer, int size);
};

#endif    //DEVICES_H
