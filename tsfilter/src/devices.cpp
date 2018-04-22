#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "devices.h"
#include "logger.h"
#include "common.h"

//----- FileInDevice -----
FileInDevice::FileInDevice(const char *file_path)
{
    ALLOC_ARRAY(m_file_path, char, strlen(file_path)+1);
    strcpy(m_file_path, file_path);

    m_fp_in = 0;
}

FileInDevice::~FileInDevice()
{
    DEALLOC_ARRAY(m_file_path);
}

int FileInDevice::Init(void) { return RET_OK; }

int FileInDevice::Open(void)
{
    LOG_DECLARE();
    m_fp_in = open(m_file_path, O_RDONLY);
    if( m_fp_in < 0 )
    {
        LOG_PRINT_FORMAT(LOG_ERROR, "FileIn %s open failed, %s\n", m_file_path, strerror(errno));
        return RET_NOK;
    }
	LOG_PRINT_FORMAT(LOG_DEBUG, "FileIn %s open at fd= %d\n", m_file_path, m_fp_in);
    return RET_OK;
}

int FileInDevice::Close(void)
{
    LOG_DECLARE();
    LOG_CHECK_DECLARE();

    LOG_CHECK_1_START();
    LOG_PRINT_FORMAT(LOG_DEBUG, "Closing file fd= %d\n", m_fp_in);
    LOG_CHECK_END();

    if( m_fp_in < 0 )
        LOG_PRINT_FORMAT(LOG_ERROR, "Invalid file fd= %d\n", m_fp_in);
    close(m_fp_in);
    return RET_OK;
}

int FileInDevice::Read(unsigned char* buffer, int size)
{
    return read(m_fp_in, buffer, size);
}

//----- MulticastInDevice -----
MulticastInDevice::MulticastInDevice(int af_type, int src_port, const char *src_addr)
{
    m_af_type = af_type;
    m_src_port = src_port;
    ALLOC_ARRAY(m_src_ip, char, INET_ADDRSTRLEN);    //INET6_ADDRSTRLEN for IPv6
    strcpy(m_src_ip, src_addr);

    m_sock_in = 0;	
}

MulticastInDevice::~MulticastInDevice()
{
    DEALLOC_ARRAY(m_src_ip);
}

int MulticastInDevice::Init(void) { return RET_OK; }

int MulticastInDevice::Open(void)
{
    #ifdef ip_mreqn
        struct ip_mreqn mgroup;
    #else
    /* according to
        http://lists.freebsd.org/pipermail/freebsd-current/2007-December/081080.html
        in bsd it is also possible to simply use ip_mreq instead of ip_mreqn
        (same as in Linux), so we are using this instead
    */
        struct ip_mreq mgroup;
    #endif
    struct sockaddr_in addr;
    unsigned int addrlen;
    int reuse;
    LOG_DECLARE();
	

    addrlen = sizeof(addr);
	
    memset((char *) &addr, 0, addrlen);
    addr.sin_family = m_af_type;
    addr.sin_port = htons(m_src_port);    //addr.sin_port = htons(atoi("1234"));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
    memset((char *) &mgroup, 0, sizeof(mgroup));
    mgroup.imr_multiaddr.s_addr = inet_addr(m_src_ip);    //mgroup.imr_multiaddr.s_addr = inet_addr("225.0.0.1");
    #ifdef ip_mreqn
        mgroup.imr_address.s_addr = INADDR_ANY;
    #else
        /* this is called 'interface' here */
        mgroup.imr_interface.s_addr = INADDR_ANY;
    #endif
	
	
    m_sock_in = socket(m_af_type, SOCK_DGRAM, 0);
    if (m_sock_in < 0)
    {
        LOG_PRINT_FORMAT(LOG_ERROR, "Socket open failed before %s:%d\n", m_src_ip, m_src_port);
        return RET_NOK;
    }
    
    reuse = 1;
    if( setsockopt(m_sock_in, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0 )
    {
        LOG_PRINT_MSG(LOG_ERROR, "setsockopt() SO_REUSEADDR: error\n");
    }

    if (bind(m_sock_in, (struct sockaddr*)&addr, addrlen) < 0)
    {
        LOG_PRINT_MSG(LOG_ERROR, "bind() error\n");
        close(m_sock_in);
        return RET_NOK;
    }
    
    if (setsockopt(m_sock_in, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mgroup, sizeof(mgroup)) < 0) {
        LOG_PRINT_MSG(LOG_ERROR, "setsockopt() IPPROTO_IP error\n");
        close(m_sock_in);
        return RET_NOK;
    } 

    LOG_PRINT_FORMAT(LOG_DEBUG, "MulticastIn for %s:%d open at fd= %d\n", m_src_ip, m_src_port, m_sock_in);
	
    return RET_OK;
}

int MulticastInDevice::Close(void)
{
    LOG_DECLARE();
    LOG_CHECK_DECLARE();

    LOG_CHECK_1_START();
    LOG_PRINT_FORMAT(LOG_DEBUG, "Closing socket fd= %d\n", m_sock_in);
    LOG_CHECK_END();

    if( m_sock_in < 0 )
        LOG_PRINT_FORMAT(LOG_ERROR, "Invalid socket fd= %d\n", m_sock_in);
    close(m_sock_in);
    return RET_OK;
}

int MulticastInDevice::Read(unsigned char* buffer, int size)
{
    struct sockaddr_in addr;
    unsigned int addrlen;
    addrlen = sizeof(addr);
    return recvfrom(m_sock_in, buffer, size, 0, (struct sockaddr *) &addr, &addrlen);
}

//----- FileOutDevice -----
FileOutDevice::FileOutDevice(const char *file_path)
{
    ALLOC_ARRAY(m_file_path, char, strlen(file_path)+1);
    strcpy(m_file_path, file_path);

    m_fp_out = 0;
}

FileOutDevice::~FileOutDevice()
{
    DEALLOC_ARRAY(m_file_path);
}

int FileOutDevice::Init(void) { }

int FileOutDevice::Open(void)
{
    LOG_DECLARE();
    m_fp_out = open(m_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if( m_fp_out < 0 )
    {
        LOG_PRINT_FORMAT(LOG_ERROR, "FileOut %s open failed, %s\n", m_file_path, strerror(errno));
        return RET_NOK;
    }
	LOG_PRINT_FORMAT(LOG_DEBUG, "FileOut %s open at fd= %d\n", m_file_path, m_fp_out);
    return RET_OK;
}

int FileOutDevice::Close(void)
{
    LOG_DECLARE();
    LOG_CHECK_DECLARE();

    LOG_CHECK_1_START();
    LOG_PRINT_FORMAT(LOG_DEBUG, "Closing file fd= %d\n", m_fp_out);
    LOG_CHECK_END();

    if( m_fp_out < 0 )
        LOG_PRINT_FORMAT(LOG_ERROR, "Invalid file fd= %d\n", m_fp_out);	
    close(m_fp_out);
    return RET_OK;
}

int FileOutDevice::Write(unsigned char* buffer, int size)
{
    return write(m_fp_out, buffer, size);
}


//----- MulticastOutDevice -----
MulticastOutDevice::MulticastOutDevice(int af_type, int dest_port, const char *dest_group)
{
    m_af_type = af_type;
    m_sock_out = 0;	
	
    memset((char *) &m_dest_addr, 0, sizeof(m_dest_addr));
    m_dest_addr.sin_family = m_af_type;
    m_dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);    //m_dest_addr.sin_addr.s_addr=inet_addr(dest_group);
    m_dest_addr.sin_port = htons(dest_port);
}

MulticastOutDevice::~MulticastOutDevice()    {    }

int MulticastOutDevice::Init(void) { return RET_OK; }

int MulticastOutDevice::Open(void)
{
    LOG_DECLARE();	
	
    m_sock_out = socket(m_af_type, SOCK_DGRAM, 0);
    if (m_sock_out < 0)
    {
        LOG_PRINT_MSG(LOG_ERROR, "Socket open failed\n");
        return RET_NOK;
    }
	
	LOG_PRINT_FORMAT(LOG_DEBUG, "MulticastOut open at fd= %d\n", m_sock_out);
	
    return RET_OK;
}

int MulticastOutDevice::Close(void)
{
    LOG_DECLARE();
    LOG_CHECK_DECLARE();

    LOG_CHECK_1_START();
    LOG_PRINT_FORMAT(LOG_DEBUG, "Closing socket fd= %d\n", m_sock_out);
    LOG_CHECK_END();

    if( m_sock_out < 0 )
        LOG_PRINT_FORMAT(LOG_ERROR, "Invalid socket fd= %d\n", m_sock_out);
    close(m_sock_out);
    return RET_OK;
}


int MulticastOutDevice::Write(unsigned char* buffer, int size)
{
    return sendto(m_sock_out, buffer, size, 0, (struct sockaddr *) &m_dest_addr, sizeof(m_dest_addr));
}
