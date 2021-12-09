#pragma once

#include "IClient.h" 
#include "ConnectionClient.h"
#include "ThreadSafeQueue.h"
#include <thread>

template<typename T>
class ClientBack : public IClient
{
public:
	ClientBack() = default;

	~ClientBack() override;

	ErrorMessage Connect(const std::string& host, unsigned port) override;

	//Stop asynchronous tasks and delete current connection
	bool Disconnect() override;

	//Checks if socket is still open
	bool IsConnected() const override;

private:
	std::unique_ptr<ConnectionClient<T>> connection_ptr_;
	ThreadSafeQueue<Message<T>> in_message_queue_;
	boost::asio::io_context io_;
	std::thread thread_;
};

//----------------------------------------

template <typename T>
ClientBack<T>::~ClientBack()
{
	if(thread_.joinable())
	{
		thread_.join();
	}
	if(connection_ptr_)
	{
		connection_ptr_->Disconnect();
	}
}

template <typename T>
ErrorMessage ClientBack<T>::Connect(const std::string& host, unsigned port)
{
	connection_ptr_ = ConnectionClient<T>::Create(io_, in_message_queue_);
	ErrorMessage error_message = connection_ptr_->ConnectToServer(host, port);

	if(error_message.first)
	{
		connection_ptr_ = nullptr;
	}
	else
	{
		thread_ = std::thread{ [this] { io_.run(); } };
	}
	return error_message;
}

template <typename T>
bool ClientBack<T>::Disconnect()
{
	if(connection_ptr_)
	{
		connection_ptr_->Disconnect();
	}

	io_.stop();
	connection_ptr_ = nullptr;
	return true;
}

template <typename T>
bool ClientBack<T>::IsConnected() const
{
	if(connection_ptr_)
	{
		return connection_ptr_->IsConnected();
	}
	return false;
}
