#pragma once
#include <memory>


enum class MessageTypes
{
	IdWithText
};

#include "IMessageData.h"
#include "MessageDataIdText.h"

//----------------------------------------------------

std::shared_ptr<IMessageData> CreateMessage(MessageTypes type)
{
	switch (type)
	{
	case MessageTypes::IdWithText:
		return std::make_shared<MessageDataIdText>();
	}
}