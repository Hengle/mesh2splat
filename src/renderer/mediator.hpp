#pragma once
#include <string>
#include <utility>
#include <any>
#include "event.hpp"

class IMediator {
public:
    virtual void notify(EventType event) = 0;
    virtual ~IMediator() = default;
};
