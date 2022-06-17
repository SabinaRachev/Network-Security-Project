#pragma once
#include "Forwards.h"

template <class T>
class ICollector
{
public:
	ICollector() = default;
	virtual ~ICollector() = default;
	virtual List<T> collectData() = 0;	
};
