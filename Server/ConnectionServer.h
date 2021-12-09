#pragma once

#include "Connection.h"
#include "ThreadSafeQueue.h"
#include "MessageWithConnection.h"

template<typename T>
class ConnectionServer : public Connection<T, ConnectionServer<T>>,
	public std::enable_shared_from_this<ConnectionServer<T>>
{
public:
	static std::shared_ptr<ConnectionServer<T>> Create(
		boost::asio::io_context& io,
		boost::asio::ip::tcp::socket&& socket,
		ThreadSafeQueue<MessageWithConnection<T>>& message_in,
		uint32_t id,
		std::condition_variable& condition_variable
	)
	{
		return std::shared_ptr<ConnectionServer<T>>(
			new ConnectionServer(io, socket, message_in, id, condition_variable)
			);
	}

	uint32_t GetId() const
	{
		return id_;
	}

private:
	ConnectionServer(boost::asio::io_context& io,
		boost::asio::ip::tcp::socket&& socket,
		ThreadSafeQueue<MessageWithConnection<T>>& message_in,
		uint32_t id,
		std::condition_variable& condition_variable)
			: Connection<T, ConnectionServer<T>>(io, std::move(socket)),
				message_in_(message_in), id_(id), condition_variable_(condition_variable) {}

	void AddMessageToIncomeQueue(const Message<T>& message);

private:
	//server stores queue of income message
	ThreadSafeQueue<MessageWithConnection<T>>& message_in_;
	uint32_t id_ = 0;
	std::condition_variable& condition_variable_;

	//friend access only for special Connection class
	friend class Connection<T, ConnectionServer<T>>;

	template<typename V>
	friend bool operator<(const ConnectionServer<V>& lhs, const ConnectionServer<V>& rhs);
};

template <typename V>
bool operator<(const ConnectionServer<V>& lhs, const ConnectionServer<V>& rhs)
{
	return lhs.id_ < rhs.id_;
}

template <typename T>
void ConnectionServer<T>::AddMessageToIncomeQueue(const Message<T>& message)
{
	//Create MessageWithConnection and push into queue
	message_in_.EmplaceBack(message, this->shared_from_this());
	condition_variable_.notify_one();
}
