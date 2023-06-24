#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

class Request {
    public:
        std::string method;
        std::string body;
        std::string route;

        void setHeader(std::string key, std::string value);
        std::string getHeader(std::string key);
        void parse(std::string data);

    private:
        bool parsedTopLine = false;
        bool parsedHeaders = false;
        std::string temporaryParsingBuffer;
        std::map<std::string, std::string> headers;
};

#endif