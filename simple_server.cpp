
#include <iostream>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include <utility>

using namespace std;
using boost::asio::ip::tcp;

pair<string,string> leftright_string (string str, string delimiter)
{
	int pos = str.find(delimiter);
	string left = str.substr(0,pos);
	string right = str.substr(pos+delimiter.size(),str.size());
	pair<string,string> retVal (left,right);
	return retVal;
}	

map<string,string> parse_request (string request)
{ 
	map<string,string> retVal;
	string token;

	while (( request.find("|")) != string::npos)
	{
		token = leftright_string(request,"|").first;
		retVal.insert(leftright_string(token,"="));
		request = leftright_string(request,"|").second;
	}
	
	return retVal;
}

string dispatch_request (map<string,string> request)
{
	string retVal="";
	string key = "TYPE";
	string reqType = request[key];
	//retVal="Request Type: " + request[key];
	if (reqType=="HEARTBEAT")
		retVal="beating";
	if (reqType=="UPDATE")
		retVal="updating";
	if (reqType=="TEST")
		retVal="testing";
	
	return retVal;
}

string process_request (string request)
{
	map<string,string> parsed = parse_request(request);
	return dispatch_request(parsed);
}

void start_server ()
{

	boost::system::error_code error;
	boost::asio::io_service io_service;
	
	tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(),4555));
	string received_request;
	for (;;)
	{
		char data[1024];
		boost::system::error_code error;
		boost::asio::ip::tcp::socket sock(io_service);
		a.accept(sock);
		size_t length = sock.read_some(boost::asio::buffer(data),error);
		if (error == boost::asio::error::eof)
			break;
		data[length]='\0';
		received_request=string(data);
		cout << "Received: " << received_request;
		string retVal = process_request(received_request);
		cout << "Result: " << retVal << "\n";
	}

}

int main(int argc, char* argv[])
{
//	string retVal = process_request("a=1|b=2|");
//	cout << retVal;
	start_server();
}


