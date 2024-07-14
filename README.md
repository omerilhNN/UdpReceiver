That is basically a UDPReceiver. When data came from UDPSender, this process creates a new file on the receiver and writes that data into it. 
setsockopt - specifies the RECVBUFSIZE. Specifiying it could cause bottleneck or relaxing to socket.
