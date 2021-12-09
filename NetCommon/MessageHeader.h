#pragma once
#include "Message.h"
#include <cstdint>


template<typename T>
struct Message<T>::MessageHeader
{
	T enum_type;
	uint32_t message_size = 0;
};
