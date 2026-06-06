#ifndef WSCONNECT_H
#define WSCONNECT_H
#include <string>
#include <string_view>



class WSConnect
{

public:
	void connectWS(std::string_view coin);
};

#endif