/////////////////////////////////////////////////////////////////////////////
// Message_Sender_Receiver_Player.cpp
//
//Implementation file for the Player class.
//
//Author - Anind Duttaroy
////////////////////////////////////////////////////////////////////////////


#include "Message_Sender_Receiver_Player.h"

//Constructor
Player::Player(Player_Status status)
	:	message(""),
		msg_to_send(""),
		received_message(""),
		msg_to_send_size(0),
		msg_to_receive_size(0),
		linger_size(0),
		send_msg_result(-1),
		receive_msg_result(-1),
		msg_counter(0),
		file_descriptor(-1),
		socket_descriptor(-1),
		set_socket_options_result(-1),
		connect_to_socket_result(-1),
		remove_sock_path_result(-1),
		bind_descriptor(-1),
		listen_descriptor(-1),
		accept_descriptor(-1),
		initiator_descriptor(-1),
		receiver_descriptor(-1),
		player_status(status)
{
	if (player_status == Player_Status::initiator)
	{
		player_name = player_initiator;
	}
	else if (player_status == Player_Status::receiver)
	{
		player_name = player_receiver;
	}
}

//Copy Constructor
Player::Player(const Player& other)
	:	message(other.message),
		msg_to_send(other.msg_to_send),
		received_message(other.received_message),
		msg_to_send_size(other.msg_to_send_size),
		msg_to_receive_size(other.msg_to_receive_size),
		linger_size(other.linger_size),
		send_msg_result(other.send_msg_result),
		receive_msg_result(other.receive_msg_result),
		msg_counter(other.msg_counter),
		file_descriptor(other.file_descriptor),
		socket_descriptor(other.socket_descriptor),
		set_socket_options_result(other.set_socket_options_result),
		connect_to_socket_result(other.connect_to_socket_result),
		remove_sock_path_result(other.remove_sock_path_result),
		bind_descriptor(other.bind_descriptor),
		listen_descriptor(other.listen_descriptor),
		accept_descriptor(other.accept_descriptor),
		initiator_descriptor(other.initiator_descriptor),
		receiver_descriptor(other.receiver_descriptor),
		player_status(other.player_status)
{
}

//Assignment Operator
Player& Player::operator=(const Player& other)
{
	message = other.message;
	msg_to_send = other.msg_to_send;
	received_message = other.received_message;
	msg_to_send_size = other.msg_to_send_size;
	msg_to_receive_size = other.msg_to_receive_size;
	linger_size = other.linger_size;
	send_msg_result = other.send_msg_result;
	receive_msg_result = other.receive_msg_result;
	msg_counter = other.msg_counter;
	file_descriptor = other.file_descriptor;
	socket_descriptor = other.socket_descriptor;
	set_socket_options_result = other.set_socket_options_result;
	connect_to_socket_result = other.connect_to_socket_result;
	remove_sock_path_result = other.remove_sock_path_result;
	bind_descriptor = other.bind_descriptor;
	listen_descriptor = other.listen_descriptor;
	accept_descriptor = other.accept_descriptor;
	initiator_descriptor = other.initiator_descriptor;
	receiver_descriptor = other.receiver_descriptor;
	player_status = other.player_status;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////
//Getter Functions
///////////////////////////////////////////////////////////////////////////////////////

std::string Player::GetPlayerName() const
{
	return player_name;
}

std::string Player::GetProcessedMessage() const
{
	return message;
}

size_t Player::GetSizeOfMessageToBeSent() const
{
	return (message.size() + 1); //allowing for null character at the end for a C-style array
}

size_t Player::GetSizeOfMessageToBeReceived() const
{
	if(msg_counter < transmission_counter)
	{
		//allowing for null character at the end for a C-style array
		return (message.size() + message_size_offset + 1); 
	}
	else
	{
		//allowing for null character at the end for a C-style array
		return (message.size() + message_size_offset + 1 + 1); //the extra one is due to "ping10" or "pong10" having a extra digit
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//Process Sent and Received Messages
///////////////////////////////////////////////////////////////////////////////////////

void Player::ProcessMessageToSend()
{
	++msg_counter;
	std::string counter_str = std::to_string(msg_counter);
	if (player_status == Player_Status::initiator)
	{
		if (msg_counter == 1)
		{
			message = ping_message;
		}
		else
		{
			message += ping_message;
		}

	}
	else if (player_status == Player_Status::receiver)
	{
		message += pong_message;
	}

	message += counter_str;
	
}

void Player::ProcessReceivedMessage(const std::string& msg)
{
	message = msg;
}



///////////////////////////////////////////////////////////////////////////////////////
//Methods for sending and receiving messages in the same process
///////////////////////////////////////////////////////////////////////////////////////

void Player::SendMessageInSameProcess(Player& receiver)
{
	ProcessMessageToSend();
	std::cout << player_name << " is sending message " << message << std::endl;
	receiver.ReceiveMessageInSameProcess(message);	
}

void Player::ReceiveMessageInSameProcess(const std::string& msg)
{
	ProcessReceivedMessage(msg);
	std::cout << player_name << " is receiving message " << message << std::endl;
}



///////////////////////////////////////////////////////////////////////////////////////
//Methods for sending and receiving messages in different processes
///////////////////////////////////////////////////////////////////////////////////////


//Setting up the Initiator and the Receiver when they are created in different processes
int Player::SetUpInitiatorOrReceiver(const char* file_for_transmission, const char* SOCK_PATH)
{
	int socket_status = operation_failed;
	
	switch(player_status)
	{
		case Player_Status::initiator:
		
			file_descriptor = open(file_for_transmission, O_RDWR);
			printf("File opened = %d\n", file_descriptor);
			perror("open()");
		
			if (file_descriptor == operation_failed)
			{
				std::cout << "File could not be opened" << std::endl; 
				return operation_failed;
			}
			
			socket_status = SetUpSocketConnection(SOCK_PATH);
			if (socket_status == operation_failed)
			{
				std::cout << "Socket could not be set up" << std::endl; 
				return socket_status;
			}
			
			linger_time.l_onoff = 1;
			linger_time.l_linger = 1;
			linger_size = sizeof(linger_time);

			set_socket_options_result = setsockopt(socket_descriptor, SOL_SOCKET, SO_LINGER,
			(char *)&linger_time, linger_size);
			printf("Socket options set = %d\n", set_socket_options_result);
			perror("setsockopt()");
			
		    	if (set_socket_options_result == operation_failed)
		    	{
		    		std::cout << "Socket options could not be set" << std::endl;
				return operation_failed;
			}
			
		    	connect_to_socket_result = connect(socket_descriptor, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
		    	printf("connection made %d\n", connect_to_socket_result);
			perror("connect()");

		    	if (connect_to_socket_result == operation_failed)
		    	{
		    		std::cout << "Connection could not be made" << std::endl;
				return operation_failed;
			}
			
			initiator_descriptor = socket_descriptor;

			break;
		
			
		case Player_Status::receiver:
		
			remove_sock_path_result = remove(SOCK_PATH);
			printf("remove call %d\n", remove_sock_path_result);
		        perror("remove()");

			socket_status = SetUpSocketConnection(SOCK_PATH);
			if (socket_status == operation_failed)
			{
				std::cout << "Socket could not be set up" << std::endl; 
				return socket_status;
			}

			bind_descriptor = bind(socket_descriptor, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)); 
			printf("bind call %d\n", bind_descriptor);
			perror("bind()");
			
			listen_descriptor = listen(socket_descriptor, 5);
			printf("listen call %d\n", listen_descriptor);
			perror("listen()");
			
			accept_descriptor = accept(socket_descriptor, NULL, NULL);
			printf("Connection accepted = %d\n", accept_descriptor);
			perror("accept()");
			
			if (accept_descriptor == operation_failed)
			{
				std::cout << "Connection could not be accepted" << std::endl;
				return operation_failed;
			}    
		
			receiver_descriptor = accept_descriptor;
			
			break;
	}
	
	return 0;
}

//Setting up the Socket connection required for both Initiator and Receiver
int Player::SetUpSocketConnection(const char* SOCK_PATH)
{
	msgh.msg_name = NULL;
	msgh.msg_namelen = 0;

	//Setting up the data structure to send and receive
	msgh.msg_iov = &iov;
	msgh.msg_iovlen = 1;

	//Setting 'msgh' fields to describe the ancillary data buffer
	msgh.msg_control = controlMsg.buf;
	msgh.msg_controllen = sizeof(controlMsg.buf);

	//The control message buffer must be zero-initialized in order
	//for the CMSG_NXTHDR() macro to work correctly.
	memset(controlMsg.buf, 0, sizeof(controlMsg.buf));

	//Setting message header to describe the ancillary data to be sent
	cmsgp = CMSG_FIRSTHDR(&msgh);
	cmsgp->cmsg_len = CMSG_LEN(sizeof(int));
	cmsgp->cmsg_level = SOL_SOCKET;
	cmsgp->cmsg_type = SCM_RIGHTS;
	memcpy(CMSG_DATA(cmsgp), &file_descriptor, sizeof(int));

	//Connecting to the socket
	memset(&addr, 0, sizeof(struct sockaddr_un));
    	addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
        
        socket_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
        printf("socket created %d\n", socket_descriptor);
        perror("socket()");
      
    	if (socket_descriptor == operation_failed)
    	{
    		std::cout << "Socket could not be set up" << std::endl;
		return operation_failed;
	}
        
	
	return 0;
}

//Using the system call sendmsg() required for both Initiator and Receiver
int Player::SendMessage()
{
	//Process the message to send
	ProcessMessageToSend();
	msg_to_send = GetProcessedMessage();
	msg_to_send_size = GetSizeOfMessageToBeSent();
	
	//Putting the string data in C-style array to send through unix socket
	char send_buffer[msg_to_send_size];
	msg_to_send.copy(send_buffer, msg_to_send_size, array_start_index);
	send_buffer[msg_to_send_size - 1] = null_termination_character;
		
	iov.iov_base = &send_buffer;
	iov.iov_len = msg_to_send_size;
    	
    	//Sending real plus ancillary data
    	switch(player_status)
	{
		case Player_Status::initiator:
			do 
			{
			      send_msg_result = sendmsg(initiator_descriptor, &msgh, 0);
		    	} 
		    	while (send_msg_result < 0 && errno == EINTR);
		
			break;
			
		case Player_Status::receiver:
			do 
			{
			      send_msg_result = sendmsg(receiver_descriptor, &msgh, 0);
		    	} 
		    	while (send_msg_result < 0 && errno == EINTR);
		
			break;
		
	}
	
	printf("sendmsg() returned %zd\n", send_msg_result);
	perror("sendmsg()");

	if (send_msg_result == operation_failed)
	{
		std::cout << "Message could not be sent" << std::endl;
		return -1;
	}

	std::cout << GetPlayerName() << " has sent message " << msg_to_send << std::endl;
	
	return 0;

}

//Using the system call recvmsg() required for both Initiator and Receiver
int Player::ReceiveMessage()
{
	msg_to_receive_size = GetSizeOfMessageToBeReceived();
	
	//Setting up a  C-style array to receive data through unix socket
	char receive_buffer[msg_to_receive_size];

	iov.iov_base = &receive_buffer;
	iov.iov_len = msg_to_receive_size;

	/* Receive real plus ancillary data */
    	switch(player_status)
	{
		case Player_Status::initiator:
			receive_msg_result = recvmsg(initiator_descriptor, &msgh, 0);
			break;
			
		case Player_Status::receiver:
			receive_msg_result = recvmsg(receiver_descriptor, &msgh, 0);		
			break;
	}
	printf("recvmsg() returned %zd\n", receive_msg_result);
	perror("recvmsg()");
	
	if (receive_msg_result == operation_failed)
	{
		std::cout << "Message could not be received by recvmsg" << std::endl;
		return operation_failed;
	}
	
	//Process the received real data
	received_message.assign(receive_buffer);
	ProcessReceivedMessage(received_message);
	
	std::cout << GetPlayerName() << " has received message " << received_message << std::endl;

	return 0;
}

//Closing the socket and the file
int Player::CloseAll()
{
	//Closing the socket
	if (close(socket_descriptor) == operation_failed)
	{
		std::cout << "Socket could not be closed" << std::endl;
		return operation_failed;
	}	
	
	if(player_status == Player_Status::initiator)
	{
		//Closing the file descriptor
		if (close(file_descriptor) == operation_failed)
		{
			std::cout << "File could not be closed" << std::endl;
			return operation_failed;
		}	
	}

	return 0;
}

