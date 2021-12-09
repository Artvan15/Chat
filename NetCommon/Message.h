#pragma once
#include <vector>
#include <stdexcept>
#include <concepts>
#include <memory>
#include "IMessageData.h"


// data represented as std::vector<uint8_t>

using ErrorMessage = std::pair<bool, std::string>;

template<typename T>
class Message
{
public:
	struct MessageHeader;
public:
	Message() = default;
	Message(T message_type)
		: header_{message_type} {}

	uint32_t DataSize() const;

	Message& FromMessageData(const std::shared_ptr<IMessageData>& data);
	Message& ToMessageData(const std::shared_ptr<IMessageData>& data);

private:
	MessageHeader header_;
	std::vector<uint8_t> data_;
	
	//for simple data types
	template<typename V, typename SimpleDataType>
	friend Message<V>& operator<<(Message<V>& message, const SimpleDataType& data);

	template<typename V, typename SimpleDataType>
	friend Message<V>& operator>>(Message<V>& message, SimpleDataType& data);

	template<typename V, typename Derived>
	friend class Connection;
};

#include "MessageHeader.h"

//----------------------------------------------------

template <typename T>
uint32_t Message<T>::DataSize() const
{
	return header_.message_size ;
}

//----------------------------------------------------

template <typename T>
Message<T>& Message<T>::FromMessageData(const std::shared_ptr<IMessageData>& data)
{
	data_ = data->Encode();
	header_.message_size = data_.size();
	return *this;
}

//----------------------------------------------------

template <typename T>
Message<T>& Message<T>::ToMessageData(const std::shared_ptr<IMessageData>& data)
{
	data->Decode(data_);
	data_.resize(header_.message_size);
	return *this;
}

//----------------------------------------------------

template<typename T>
concept Condition = std::is_standard_layout_v<T>;

//----------------------------------------------------

template<typename V, typename SimpleDataType>
requires Condition<SimpleDataType>
Message<V>& operator<<(Message<V>& message, const SimpleDataType& data)
{
	message.data_.resize(message.header_.message_size + sizeof(SimpleDataType));
	memcpy(message.data_.data() + message.header_.message_size, &data, sizeof(SimpleDataType));
	
	message.header_.message_size = message.data_.size();
	return message;
}

//----------------------------------------------------

template<typename V, typename SimpleDataType>
requires Condition<SimpleDataType>
Message<V>& operator>>(Message<V>& message, SimpleDataType& data)
{
	auto begin_of_copying_data = message.data_.data() + message.data_.size() - sizeof(SimpleDataType);
	memcpy(&data, begin_of_copying_data, sizeof(SimpleDataType));
	message.header_.message_size = message.data_.size() - sizeof(SimpleDataType);
	message.data_.resize(message.header_.message_size);
	return message;
}
