#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H


#include <iostream>

class HttpClient
{
public:
    std::string post(
        const std::string& endpoint,
        const std::string& body
    );
};

#endif