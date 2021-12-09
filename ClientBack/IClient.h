#pragma once

#pragma once
#include <string>

struct IClient
{
	virtual ~IClient() = default;
	virtual std::pair<bool, std::string> Connect(const std::string& host, unsigned port) = 0;
	virtual bool Disconnect() = 0;
	virtual bool IsConnected() const = 0;
};