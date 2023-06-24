#include "response.hpp"

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

std::string Response::generateString()
{
    std::string out = "HTTP/1.1 ";
    char temp[12];
    sprintf(temp, "%d", this->status);
    out += temp;
    out += " " + this->statusText + "\r\n";
    for (auto& header : this->headers) {
        out += header.first + ": " + header.second + "\r\n";
    }

    out += "\r\n";
    if (this->body.length())
        out += this->body;

    return std::string(out);
}

void Response::setBody(std::string body)
{
    this->body = body;
    this->setHeader("Content-Length", std::to_string(body.length()));
}

void Response::sendResponse(int fd)
{
    std::string packet = this->generateString();
    send(fd, packet.c_str(), packet.length(), 0);
}