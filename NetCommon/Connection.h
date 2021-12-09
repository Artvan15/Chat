#pragma once
#include "ThreadSafeQueue.h"
#include <boost/asio.hpp>
#include "Message.h"


template<typename T, typename Derived>
class Connection
{
protected:
	Connection(boost::asio::io_context& io,
		boost::asio::ip::tcp::socket&& socket)
		: io_(io), socket_(std::move(socket)) {}

public:

	//Close socket. Add this task to token queue in io
	void Disconnect();

	bool IsConnected() const;

	//Add message to output_message queue.
	//If message write loop is stopped - start a new one calling WriteHeader()
	void Send(const Message<T>& message);

protected:
	//ASYNC - Start async_read to temp_message.header
	//if there is no body - add msg to queue and start another read
	//Otherwise resize temp_message.body and call ReadBody()
	//If error occur - close the socket
	void ReadHeader();


	//ASYNC - if message_out isn't empty - start async_write of front message.header
	//if message is without body, popped it from queue and call WriteHeader()
	//Otherwise - call WriteBody()
	void WriteHeader();


	//ASYNC - start async_read to temp_message.body
		//call AddMessageToIncomeQueue via this pointer of derived class
	void ReadBody();


	//ASYNC - start async_write of front message.body
	//popped front message and call WriteHeader
	void WriteBody();


	//called method in derived class
	void AddMessageToIncomeQueue(const Message<T>& message);

protected:
	boost::system::error_code er;
	boost::asio::io_context& io_;
	boost::asio::ip::tcp::socket socket_;

	Message<T> temp_in_message_;
	ThreadSafeQueue<Message<T>> out_message_queue_;
};

//----------------------------------------

template <typename T, typename Derived>
void Connection<T, Derived>::Disconnect()
{
	if(IsConnected())
	{
		boost::asio::post(io_,
			[this]()
			{
				socket_.close();
			});
	}
}

//----------------------------------------

template <typename T, typename Derived>
bool Connection<T, Derived>::IsConnected() const
{
	return socket_.is_open();
}

template <typename T, typename Derived>
void Connection<T, Derived>::Send(const Message<T>& message)
{
	boost::asio::post(io_,
		[this, &message]
		{
			bool WritingLoopStop = out_message_queue_.Empty();
			out_message_queue_.PushBack(message);
			if (WritingLoopStop)
			{
				WriteHeader();
			}
		});
}

//----------------------------------------

template <typename T, typename Derived>
void Connection<T, Derived>::ReadHeader()
{
	boost::asio::async_read(socket_,
		boost::asio::buffer(&temp_in_message_.header_, sizeof(typename Message<T>::MessageHeader)),
		[this](const boost::system::error_code& er, size_t bytes)
		{
			if (!er)
			{
				if (temp_in_message_.DataSize() > 0)
				{
					temp_in_message_.data_.resize(temp_in_message_.header_.message_size);
					ReadBody();
				}
				else
				{
					//add message without a header
					AddMessageToIncomeQueue(temp_in_message_);
					ReadHeader();
				}
			}
			else
			{
				std::cerr << "Reader header fail " << er.message();
				Disconnect();
			}
		});
}

//----------------------------------------

template <typename T, typename Derived>
void Connection<T, Derived>::WriteHeader()
{
	boost::asio::async_write(socket_,
		boost::asio::buffer(&out_message_queue_.Front().header_, sizeof(Message<T>::MessageHeader)),
		[this](const boost::system::error_code& er, size_t bytes)
		{
			if (!er)
			{
				if (out_message_queue_.Front().DataSize() > 0)
				{
					WriteBody();
				}
				else
				{
					//message without body
					out_message_queue_.PopFront();
					WriteHeader();
				}
			}
			else
			{
				std::cerr << "Write Header fail\t" << er.message() << std::endl;
				Disconnect();
			}
		});
}

//----------------------------------------

template <typename T, typename Derived>
void Connection<T, Derived>::ReadBody()
{
	boost::asio::async_read(socket_,
		boost::asio::buffer(temp_in_message_.data_.data(), temp_in_message_.DataSize()),
		[this](const boost::system::error_code& er, size_t bytes)
		{
			if (!er)
			{
				AddMessageToIncomeQueue(temp_in_message_);
				ReadHeader();
			}
			else
			{
				std::cerr << "Read body fail\t" << er.message() << std::endl;
				Disconnect();
			}
		});
}

//----------------------------------------

template<typename T, typename Derived>
void Connection<T, Derived>::WriteBody()
{
	boost::asio::async_write(socket_, boost::asio::buffer(out_message_queue_.Front().data_, out_message_queue_.Front().DataSize()),
		[this](const boost::system::error_code& er, size_t bytes_transferred)
		{
			if (!er)
			{
				out_message_queue_.PopFront();
				WriteHeader();
			}
			else
			{
				std::cerr << "Write header fail\t" << er.message() << std::endl;
				Disconnect();
			}
		});
}

//----------------------------------------

//static polymorphism
template <typename T, typename Derived>
void Connection<T, Derived>::AddMessageToIncomeQueue(const Message<T>& message)
{
	static_cast<Derived*>(this)->AddMessageToIncomeQueue(message);
}