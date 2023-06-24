#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

class Response {
    private:
        int status;
        std::string statusText;
        std::string body;
        std::map<std::string, std::string> headers;

    public:
        int getStatus()
        {
            return this->status;
        }
        void setStatus(int status)
        {
            this->status = status;
        }
        std::string getStatusText()
        {
            return this->statusText;
        }
        void setStatusText(std::string statusText)
        {
            this->statusText = statusText;
        }
        std::string getBody()
        {
            return this->body;
        }
        void setBody(std::string body);
        std::map<std::string, std::string> getHeaders()
        {
            return this->headers;
        }
        std::string getHeader(std::string name)
        {
            return this->headers[name];
        }
        void setHeader(std::string name, std::string value)
        {
            this->headers[name] = value;
        }
        std::string generateString();
        void sendResponse(int fd);
};

#endif