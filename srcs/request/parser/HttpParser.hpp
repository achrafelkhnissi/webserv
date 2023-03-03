#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <queue>
#include <string>
#include <map>
#include <vector>

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
	e_status is_method();
	// Request into_request(); // convert to request

	void print() {
		cout << "Method: " << method << endl;
		cout << "URI: " << uri << endl;
		cout << "Version: " << major_version << "." << minor_version << endl;
		cout << "Query: " << query << endl;
	}

	enum e_header_state {
		h_start,
		h_key,
		h_spaces_after_colon,
		h_value,
		h_cr,
		h_crlf,
		h_crlfcr,
	};

private:
	string chunk;
	string field;
	multimap<string, string>::iterator last_map;

	string method;
	string uri;
	string body;
	string query;
	int major_version;
	int minor_version;
	/* states */
	e_state state; // general state
	e_sl_state sl_state; // status line state
	e_header_state h_state; // header state

	/* utils */
	e_status status_line_parser(char c);
	e_status headers_parser(char c);
	multimap<string, string> headers;
};

HttpParser::e_state& next(HttpParser::e_state& s);

#endif
