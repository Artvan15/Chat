#pragma once
#pragma once

#include "Connection.h"
#include <boost/asio.hpp>

template<typename T>
class ConnectionClient : public Connection<T, ConnectionClient<T>>
{
public:
	static std::unique_ptr<ConnectionClient<T>> Create(
		boost::asio::io_context& io,
		ThreadSafeQueue<Message<T>>& in_message_queue)
	{
		return std::unique_ptr<ConnectionClient<T>>(new ConnectionClient<T>(io, in_message_queue));
	}

	//ASYNC - asynchronously connect and start reading header
	std::pair<bool, std::string> ConnectToServer(const std::string& host, unsigned port);

private:
	//Send io, new created socket, connected to io, to Connection
	ConnectionClient(boost::asio::io_context& io,
		ThreadSafeQueue<Message<T>>& in_message_queue)

		: Connection<T, ConnectionClient<T>>(io, boost::asio::ip::tcp::socket(io)),
		in_message_queue_(in_message_queue) {}

private:
	//called by "Connection"
	void AddMessageToIncomeQueue(const Message<T>& message)
	{
		in_message_queue_.EmplaceBack(message);
	}

private:
	ThreadSafeQueue<Message<T>>& in_message_queue_;

	friend class Connection<T, ConnectionClient<T>>;
};



template <typename T>
ErrorMessage ConnectionClient<T>::ConnectToServer(const std::string& host, unsigned port)
{
	boost::asio::ip::tcp::resolver resolver(this->io_);
	boost::system::error_code error;
	auto endpoints = resolver.resolve(host, std::to_string(port), error);
	if(error)
	{
		return ErrorMessage{ true, error.message() };
	}

	boost::asio::async_connect(this->socket_, endpoints,
		[this](const boost::system::error_code& er,
			const boost::asio::ip::tcp::endpoint& endpoint)
		{
			if (!er)
			{
				//connection successful, start listening
				this->ReadHeader();
			}
			else
			{
				return ErrorMessage{ true, er.message() };
			}

		});

	return ErrorMessage{ false, {} };
}
