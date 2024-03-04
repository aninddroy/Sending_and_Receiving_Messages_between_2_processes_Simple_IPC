/////////////////////////////////////////////////////////////////////////////
//Message_Sender_Receiver_Player.h
//
//Header file for the Player class.
//
//Author - Anind Duttaroy
////////////////////////////////////////////////////////////////////////////


#pragma once

#include "Initiator_Receiver.h"

#include <iostream>
#include <string>

const std::string ping_message = "ping";
const std::string pong_message = "pong";
const std::string null_message = "";
const std::string player_initiator = "Initiator";
const std::string player_receiver = "Receiver";
const std::string null_string = "\0";
const int array_start_index = 0;
const int transmission_counter = 10;
//calculated, assuming, message to be received contains ping1 or pong1
const size_t message_size_offset = 5;
const char null_termination_character = '\0';
const int operation_failed = -1;


enum class Player_Status
{
	initiator = 1,
	receiver = 2
};




class Player
{
public:
	//Constructor
	Player(Player_Status status);

	//Copy Constructor
	Player(const Player& other);

	//Assignment Operator
	Player& operator=(const Player& other);

	//Getter Functions
	std::string GetPlayerName() const;
	std::string GetProcessedMessage() const;
	size_t GetSizeOfMessageToBeSent() const;
	size_t GetSizeOfMessageToBeReceived() const;

	//Process the sent and received messages
	void ProcessMessageToSend();
	void ProcessReceivedMessage(const std::string& msg);

	//Methods for sending and receiving messages in the same process
	void SendMessageInSameProcess(Player& receiver);
	void ReceiveMessageInSameProcess(const std::string& msg);

	//Methods for setting up initiator, receiver, setting up socket connections, binding, accepting, closing etc.
	
	//Setting up the Initiator and the Receiver when they are created in different processes
	int SetUpInitiatorOrReceiver(const char* file_for_transmission, const char* SOCK_PATH);	
	int SetUpSocketConnection(const char* sock_path);	//Setting up the Socket connection required for both Initiator and Receiver
	int CloseAll();	//Closing the socket and the file

	//Methods for sending and receiving messages in different processes
	int SendMessage();	//Using the system call sendmsg() required for both Initiator and Receiver
	int ReceiveMessage();	//Using the system call recvmsg() required for both Initiator and Receiver

private:
	std::string message;
	std::string player_name;
	std::string msg_to_send;
	std::string received_message;
	
	sockaddr_un addr;
	linger linger_time;
	cmsghdr *cmsgp;

	size_t msg_to_send_size;
	size_t msg_to_receive_size;
	size_t linger_size;
	ssize_t send_msg_result;
	ssize_t receive_msg_result;

	int msg_counter;
	int file_descriptor;
	int socket_descriptor;
	int set_socket_options_result;
	int connect_to_socket_result;
	int remove_sock_path_result;
	int bind_descriptor;
	int listen_descriptor;
	int accept_descriptor;
	int initiator_descriptor;
	int receiver_descriptor;

	Player_Status player_status;
	msghdr msgh;
	iovec iov;

	union 
	{
		char   buf[CMSG_SPACE(sizeof(int))];	//Space large enough to hold an 'int'
		struct cmsghdr align;
	} controlMsg;

};

