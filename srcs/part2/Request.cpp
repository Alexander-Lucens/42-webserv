#pragma once

#include "Request.hpp"

Request::Request() {}
Request::~Request() {}

void Request::clear() {

}

std::string Request::getHeader(const std::string &key) const {
    return headers.at(key);
}

void Request::setHeader(const std::string &key, const std::string &value) {
    headers[key] = value;
}

std::string Request::toString() const {

}

