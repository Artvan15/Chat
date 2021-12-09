#pragma once

#pragma once


#include "Message.h"
#include "ConnectionServer.h"

template<typename T>
class MessageWithConnection : public Message<T>
{
public:
	MessageWithConnection(const Message<T>& message,
		const std::shared_ptr<ConnectionServer<T>>& connection_server)
			: Message<T>(message), connection_server_(connection_server) {}

	explicit operator Message<T>() const { return Message<T>(*this); }

private:
	std::weak_ptr<ConnectionServer<T>> connection_server_;

	template<typename V>
	friend std::ostream& operator<<(std::ostream& os, const MessageWithConnection<V>& message);
};

template <typename V>
std::ostream& operator<<(std::ostream& os, const MessageWithConnection<V>& message)
{
	return os << operator<<(os, message);
}
