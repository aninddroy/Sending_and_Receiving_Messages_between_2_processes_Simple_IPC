/////////////////////////////////////////////////////////////////////////////
// Initiator_main.cpp
//
//Defines the entry point for the Initiator Player.
//
//Author - Anind Duttaroy
////////////////////////////////////////////////////////////////////////////


#include "Message_Sender_Receiver_Player.h"

const char* SOCK_PATH = "scm_rights";
const char* file_for_transmission = "myfile.txt";


int main()
{
	//Creating initiator player
	Player playerInitiator(Player_Status::initiator);
	int counter = 1;
	int status = -1;
	
	status = playerInitiator.SetUpInitiatorOrReceiver(file_for_transmission, SOCK_PATH);
	if(status == -1)
	{
		std::cout << "Setting up socket connection failed" << std::endl;
		return status;
	}
	
	
	while (counter <= transmission_counter)
	{
		status = playerInitiator.SendMessage();
		if(status == -1)
		{
			std::cout << "Message sending failed" << std::endl;
			return status;
		}
		
		status = playerInitiator.ReceiveMessage();
		if(status == -1)
		{
			std::cout << "Receiving message failed" << std::endl;
			return status;
		}
			
		++counter;
	}

	playerInitiator.CloseAll();

	return 0;	
}
