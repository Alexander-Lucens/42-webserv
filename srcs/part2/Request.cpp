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

void Request::reset() {
    method.clear();
    uri.clear();
    path.clear();
    version.clear();
    headers.clear();
    body.clear();
    query_string.clear();

    state = RequestState::REQUEST_LINE;
}

void execute() {
    // whatever extra information you need
}
