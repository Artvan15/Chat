#include "Message.h"
#include "MessageTypes.h"
#include "MessageDataIdText.h"
#include <iostream>



int main()
{
	auto message_id_text = MessageDataIdText::Create(3, "data");
	Message<MessageTypes> message(MessageDataIdText::enum_type);
	message.FromMessageData(message_id_text);

	//Sending message...


	//Message received
	auto message_data_ptr = CreateMessage(message.header_.enum_type);
	message.ToMessageData(message_data_ptr);

	switch(message.header_.enum_type)
	{
	case MessageTypes::IdWithText:
		auto message_data = std::dynamic_pointer_cast<MessageDataIdText>(message_data_ptr);
		std::cout << *message_data;
	}
	
	
	
}
