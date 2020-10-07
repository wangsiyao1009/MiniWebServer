#include "TimeListNode.h"

TimeListNode::TimeListNode() : expire(0), httpConnection(nullptr),
                               next(nullptr), prev(nullptr) {
}

TimeListNode::~TimeListNode() = default;