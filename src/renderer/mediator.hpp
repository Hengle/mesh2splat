#pragma once
#include <string>
#include <utility>
#include <any>
#include "event.hpp"

class Mediator {
public:
    virtual void notify(EventType event) = 0;
    virtual ~Mediator() = default;
};
