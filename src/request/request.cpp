#include "request.hpp"
#include <cstdio>

extern "C" {
#include <logger.h>
}

void Request::setHeader(std::string key, std::string value)
{
    this->headers[key] = value;
}

std::string Request::getHeader(std::string key)
{
    return this->headers[key];
}

void Request::parse(std::string data)
{
    if (this->temporaryParsingBuffer.length() > 0) {
        data = this->temporaryParsingBuffer + data;
        this->temporaryParsingBuffer = "";
    }

    if (!this->parsedTopLine) {
        int endOfLine = data.find("\r\n");

        if (endOfLine == std::string::npos) {
            this->temporaryParsingBuffer = data;
            return;
        }
        this->temporaryParsingBuffer = "";
        std::string line = data.substr(0, endOfLine);
        int space = line.find(" ");
        this->method = line.substr(0, space);
        int secondSpace = line.find(" ", space + 1);
        this->route = line.substr(space + 1, secondSpace - space - 1);
        this->parsedTopLine = true;
        this->parse(data.substr(endOfLine + 2));
    } else if (!this->parsedHeaders) {
        int headersEnd = data.find("\r\n\r\n");
        if (headersEnd == std::string::npos) {
            this->temporaryParsingBuffer = data;
            return;
        }
        this->temporaryParsingBuffer = "";
        std::string headers = data.substr(0, headersEnd);
        int headerEnd = headers.find("\r\n");
        while (headerEnd != std::string::npos) {
            std::string header = headers.substr(0, headerEnd);
            int colon = header.find(": ");
            std::string key = header.substr(0, colon);
            std::string value = header.substr(colon + 2);
            this->setHeader(key, value);
            headers = headers.substr(headerEnd + 2);
            headerEnd = headers.find("\r\n");
        }
        int colon = headers.find(": ");
        std::string key = headers.substr(0, colon);
        std::string value = headers.substr(colon + 2);
        this->setHeader(key, value);
        this->parsedHeaders = true;
        this->parse(data.substr(headersEnd + 4));
    } else {
        if (this->headers.find("Content-Length") == this->headers.end()) {
            // logWarn("No Content-Length header found, assuming no body");
            this->body = "";
            return;
        }
        int contentLength = std::stoi(this->headers["Content-Length"]);
        if (data.length() >= contentLength) {
            this->body = data.substr(0, contentLength);
        } else {
            this->temporaryParsingBuffer = data;
        }
    }
}