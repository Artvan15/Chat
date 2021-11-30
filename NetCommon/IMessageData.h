#pragma once
#include <vector>
#include <iostream>

struct IMessageData
{
	virtual ~IMessageData() = default;
	virtual std::vector<uint8_t> Encode() const = 0;
	virtual void Decode(std::vector<uint8_t> bytes) = 0;
	
};