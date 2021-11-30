#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "IMessageData.h"
#include "MessageTypes.h"

class MessageDataIdText : public IMessageData
{
public:
	MessageDataIdText() = default;
	MessageDataIdText(uint32_t id, std::string text)
		: id_(id), text_(std::move(text)) {}

	void Decode(std::vector<uint8_t> bytes) override;
	std::vector<uint8_t> Encode() const override;
	

	static std::shared_ptr<MessageDataIdText> Create(uint32_t id, std::string text);

public:
	static constexpr MessageTypes enum_type = MessageTypes::IdWithText;

private:
	uint32_t id_ = 0;
	std::string text_;

	friend std::ostream& operator<<(std::ostream& os, const MessageDataIdText& message_data);
};

inline std::ostream& operator<<(std::ostream& os, const MessageDataIdText& message_data)
{
	return os << message_data.id_ << ' ' << message_data.text_;
}

inline void MessageDataIdText::Decode(std::vector<uint8_t> bytes)
{
	//copy id
	memcpy(&id_, bytes.data() + bytes.size() - sizeof(id_), sizeof(id_));
	bytes.resize(bytes.size() - sizeof(id_));

	//copy text
	std::vector<char> vec_chars(bytes.size());
	memcpy(vec_chars.data(), bytes.data(), bytes.size());

	text_.assign(vec_chars.begin(), vec_chars.end());
}

inline std::vector<uint8_t> MessageDataIdText::Encode() const
{
	std::vector<uint8_t> result;
	result.resize(sizeof(id_) + text_.size());
	memcpy(result.data(), text_.data(), text_.size()); //copy text
	memcpy(result.data() + text_.size(), &id_, sizeof(id_)); //copy id
	return result;
}



inline std::shared_ptr<MessageDataIdText> MessageDataIdText::Create(uint32_t id, std::string text)
{
	return std::make_shared<MessageDataIdText>(id, std::move(text));
}
