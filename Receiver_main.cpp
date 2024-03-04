/////////////////////////////////////////////////////////////////////////////
//Receiver_main.cpp
//
//Defines the entry point for the Receiver Player.
//
//Author - Anind Duttaroy
////////////////////////////////////////////////////////////////////////////


#include "Message_Sender_Receiver_Player.h"

const char* SOCK_PATH = "scm_rights";
const char* file_for_transmission = "myfile.txt";


int main()
{
	std::cout << std::endl << "//////////////////////////////////////////////////////////" << std::endl;
	std::cout << std::endl << "//////////////////////////////////////////////////////////" << std::endl;
	std::cout << std::endl << "//START OF SENDING AND RECEIVING MESSAGES IN DIFFERENT PROCESSES///" << std::endl;

	//Creating receiver player
	Player playerReceiver(Player_Status::receiver);
	int counter = 1;
	int status = -1;
	
	status = playerReceiver.SetUpInitiatorOrReceiver(file_for_transmission, SOCK_PATH);
	if(status == -1)
	{
		std::cout << "Setting up socket connection failed" << std::endl;
		return status;
	}
	
	while (counter <= transmission_counter)
	{	
		status = playerReceiver.ReceiveMessage();
		if(status == -1)
		{
			std::cout << "Receiving message failed" << std::endl;
			return status;
		}
		
		status = playerReceiver.SendMessage();
		if(status == -1)
		{
			std::cout << "Receiving message failed" << std::endl;
			return status;
		}
		
		++counter;
		
	}	

	playerReceiver.CloseAll();

	std::cout << std::endl << "//END OF SENDING AND RECEIVING MESSAGES IN DIFFERENT PROCESSES///" << std::endl;
	std::cout << std::endl << "//////////////////////////////////////////////////////////" << std::endl;
	std::cout << std::endl << "//////////////////////////////////////////////////////////" << std::endl;

	return 0;
}
