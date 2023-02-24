#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <queue>
#include <string>

using namespace std;

class HttpParser {

public:
	HttpParser();
	~HttpParser();
	HttpParser(const HttpParser& other);
	HttpParser& operator=(const HttpParser& other);

	enum e_status {
		DONE,
		FAILED,
		CONTINUE,
	};

	enum e_state {
		p_status_line,
		p_headers,
		p_body,

	};

	enum e_sl_state {
		sl_start,
		sl_method_start,
		sl_sp_after_method,
		sl_uri_start,
		sl_uri,
		sl_query,
		sl_http_start,
		sl_http_H,
		sl_http_HT,
		sl_http_HTT,
		sl_http_HTTP,
		sl_http_slash,
		sl_http_major,
		sl_http_dot,
		sl_http_minor,
		sl_almost_done,
	};

	e_status push(string& chunk); // process a chunk
	e_status append(char c); // process a next char
	// Request into_request(); // convert to request

	void print() {
		cout << "Method: " << method << endl;
		cout << "URI: " << uri << endl;
		cout << "Version: " << major_version << "." << minor_version << endl;
		cout << "Query: " << query << endl;
	}

private:
	string chunk;

	string method;
	string uri;
	string body;
	string query;
	int major_version;
	int minor_version;
	/* states */
	e_state state; // general state
	e_sl_state sl_state; // status line state

	/* utils */
	e_status status_line_parser(char c);
};

HttpParser::e_state& operator++(HttpParser::e_state& s);

#endif
