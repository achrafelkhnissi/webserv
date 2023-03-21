#pragma once

#include <cstddef>
#include <iostream>
#include <queue>
#include <string>
#include <map>
#include <vector>
#include "Request.hpp"

using namespace std;

class HttpParser {

public:
	HttpParser();
	~HttpParser();
	HttpParser(const HttpParser& other);
	HttpParser& operator=(const HttpParser& other);

	enum e_encoding {
		none,
		unspecified,
		invalid,
		identity,
		chunked,
	};

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
	e_encoding get_encoding();
	void into_request();
	bool has_request();
	Request consume_request();
    void reset(); // reset parser
	

	void print() {
		cout << "Method: " << method << endl;
		cout << "URI: " << uri << endl;
		cout << "Version: " << major_version << "." << minor_version << endl;
		cout << "Query: " << query << endl;
		cout << "Body: " << chunk << endl;
		cout << "size: " << chunk_size << endl;
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

	enum e_chbody_state {
		bd_start,
		bd_chunk_size,
		bd_chunk_sp_after_size,
		bd_chunk_size_cr,
		bd_chunk_data,
		bd_chunk_data_cr,
		bd_chunk_data_crlf,
	};

private:
	string chunk;
	string field;
	multimap<string, string>::iterator last_map;
	size_t body_size;
	size_t chunk_size;

	string method;
	string uri;
	string body;
	string query;
	int major_version;
	int minor_version;
	
	/* error */
	e_error error;
	size_t err_num;
	size_t err_position;


	/* states */
	e_state state; // general state
	e_sl_state sl_state; // status line state
	e_header_state h_state; // header state
	e_chbody_state ch_state; // chunked body state
	e_encoding encoding; // body encoding

	/* utils */
	e_status status_line_parser(char c);
	e_status headers_parser(char c);
	e_status chunked_body_parser(char c);
	e_status identity_body_parser(char c);
	multimap<string, string> headers;
	queue<Request> requests;
};

HttpParser::e_state& next(HttpParser::e_state& s);

