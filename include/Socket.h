#ifndef Socket_h
#define Socket_h

#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // internet address family (for sockaddr_in)
#include <arpa/inet.h> // definitions for internet operations
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

#include <errno.h>
#include <fcntl.h>

#include <set>
#include <sstream>
#include <iostream>

#include "Exception.h"
#include "SocketMessage.h"

#define SOCKET_ERROR(x) 10000+x
#define MAX_WORD_LENGTH 5000
#define MAX_SOCKET_ATTEMPTS 2

/**
 * \defgroup Socket Socket communication objects
 */
/**
 * General object providing all useful method to
 * connect/bind/send/receive information through system sockets.
 * \brief Base socket object from which clients/master from a socket inherit
 *
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 23 Mar 2015
 * \ingroup Socket
 */
class Socket
{
  public:
    /**
     * \brief Type of actor playing a role on the socket
     */
    typedef enum { INVALID=-1, MASTER=0, WEBSOCKET_CLIENT, CLIENT, DETECTOR, DQM, DAQ } SocketType;
    typedef std::set< std::pair<int,SocketType> > SocketCollection;
  
  public:
    inline Socket() {;}
    Socket(int port);
    virtual ~Socket();
    
    /// Terminates the socket and all attached communications
    void Stop();
    
    inline void SetPort(int port) { fPort=port; }
    /// Retrieve the port used for this socket
    inline int GetPort() const { return fPort; }
  
    /**
     * Set the socket to accept connections any client transmitting through the
     * socket
     * \brief Accept connection from a client
     * \param[inout] socket Master/client object to enable on the socket
     */
    void AcceptConnections(Socket& socket);
    /**
     * Register all open file descriptors to read their communication through the
     * socket
     */
    void SelectConnections();
    
    inline void SetSocketId(int sid) { fSocketId=sid; }
    inline int GetSocketId() const { return fSocketId; }
    
    inline SocketType GetSocketType(int sid) const { 
      // FIXME need to find a more C++-like method...
      for (SocketCollection::const_iterator it=fSocketsConnected.begin(); it!=fSocketsConnected.end(); it++) {
        if (it->first==sid) return it->second;
      }
      std::ostringstream o; o << "Client # " << sid << " not found in listeners list";
      throw Exception(__PRETTY_FUNCTION__, o.str(), JustWarning);
      return INVALID;
    }
    inline bool IsWebSocket(int sid) const { return GetSocketType(sid)==WEBSOCKET_CLIENT; }

    void DumpConnected() const;
    
  protected:
    /**
     * Launch all mandatory operations to set the socket to be used
     * \brief Start the socket
     * \return Success of the operation
     */
    bool Start();
    /**
     * \brief Bind a name to a socket
     * \return Success of the operation
     */
    void Bind();
    void PrepareConnection();
    /**
     * Set the socket to listen to any message coming from outside
     * \brief Listen to incoming messages
     */
    void Listen(int maxconn);
    
    /**
     * \brief Send a message on a socket
     */
    void SendMessage(Message message, int id=-1) const;
    /**
     * \brief Receive a message from a socket
     * \return Received message as a std::string
     */
    Message FetchMessage(int id=-1) const;

  private:
    /**
     * A file descriptor for this socket, if \a Create was performed beforehand.
     */
    int fSocketId;
    
  protected:
    int fPort;
    char fBuffer[MAX_WORD_LENGTH];
    SocketCollection fSocketsConnected;
    /// Master file descriptor list
    fd_set fMaster;
    /// Temp file descriptor list for select()
    fd_set fReadFds;
    
  private:
    /**
     * \brief Create an endpoint for communication
     */
    void Create();
    /**
     * \brief Configure the socket object for communication
     */
    void Configure();

    struct sockaddr_in fAddress;

};

#endif
