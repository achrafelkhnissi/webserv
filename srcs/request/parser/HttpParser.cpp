#include "HttpParser.hpp"
#include <_ctype.h>
#include <iostream>
#include <string>
#include <utility>

HttpParser::HttpParser() {
	state = HttpParser::p_status_line;
	sl_state = HttpParser::sl_start;
	encoding = HttpParser::none;
	error = err_none;
	h_state = HttpParser::h_start;
	field.reserve(4100); // 4kb
}

HttpParser::~HttpParser() { }


void HttpParser::into_request() {
	requests.push(Request());
	Request &req = requests.back();
	req.setError(error);
	if (error != err_none)
	{
		reset();
		return;
	}
	req.setMethod(method);
	req.setUri(uri);
	req.setHeaders(headers);
	req.setBody(chunk);
	req.setQuery(query);
    reset();
}

bool HttpParser::has_request() {
//	cerr << "has_request: " << requests.size() << endl;
	return !requests.empty();
}

Request HttpParser::consume_request() {
	Request req = requests.front();
	requests.pop();
	return req;
}

HttpParser::e_encoding HttpParser::get_encoding() {
	multimap<string, string>::iterator it;
	if (encoding == HttpParser::none) {
		if ((it = headers.find("Transfer-Encoding")) != headers.end()) {
			if (it->second == "chunked")
				encoding = HttpParser::chunked;
			else
				encoding = HttpParser::unspecified;
		} else if ((it = headers.find("Content-Length")) != headers.end()) {
			encoding = HttpParser::identity;
			chunk_size = atoi(it->second.c_str());
		} else
			encoding = HttpParser::unspecified;
	}
	return encoding;
}

HttpParser::e_status HttpParser::append(char c) {
	switch (state) {
	case HttpParser::p_status_line:
		return status_line_parser(c);
		break;
	case HttpParser::p_headers: {
		return headers_parser(c);
		break;
	}
	case HttpParser::p_body: {
//		print();
		switch (get_encoding()) {
		case identity:
			return identity_body_parser(c);
		case chunked:
			return chunked_body_parser(c);
		case unspecified:
			return HttpParser::DONE;
		case invalid:
			break;
		case none:
			abort();
			break;
		}
		break;
	}
	}
	return HttpParser::FAILED;
}

HttpParser::e_status HttpParser::identity_body_parser(char c) {
//	if (chunk_size == 0)
//		return HttpParser::DONE;
//	chunk_size--;
//	chunk.push_back(c);
//	return HttpParser::CONTINUE;

    if (chunk_size == 0)
        return HttpParser::DONE;
    chunk_size--;
    chunk.push_back(c);
    if (chunk_size == 0)
        return HttpParser::DONE;
    return HttpParser::CONTINUE;

}

HttpParser::e_status HttpParser::push(std::string& chunk) {
	for (size_t i = 0; i < chunk.size(); i++) {
		switch (append(chunk[i])) {
		case HttpParser::DONE: {
			switch (state) {
			case HttpParser::p_status_line:
				sl_state = HttpParser::sl_start;
				break;
			case HttpParser::p_headers:
				get_encoding();
                if (method == "GET" || method == "DELETE")
					return (into_request(), HttpParser::DONE);
				break;
			case HttpParser::p_body: {
				return (into_request(), HttpParser::DONE);
				break;
			}
			}
			next(state);
			break;
		}
		case HttpParser::FAILED: {
			err_position = i;
			into_request();
			return HttpParser::FAILED;
		}
		case HttpParser::CONTINUE:
			break;
		}
	}
	return HttpParser::CONTINUE;
}

HttpParser::e_state& next(HttpParser::e_state& s) {
	switch (s) {
	case HttpParser::p_status_line:
		s = HttpParser::p_headers;
		break;
	case HttpParser::p_headers:
		s = HttpParser::p_body;
		break;
	case HttpParser::p_body:
		s = HttpParser::p_status_line;
		break;
	}
	return s;
}

static const string methods[] = {
	"GET",
	"POST",
	"PUT",
	"DELETE",
	"HEAD",
	"CONNECT",
	"OPTIONS",
	"TRACE",
	"PATCH",
};

size_t same(string s1, string s2) {
	if (s1.size() > s2.size() || s1.back() != s2[s1.size() - 1]) {
		return -1;
	}
	size_t i = 0;
	while (1) {
		if (i >= s1.size())
			return s1.size() != s2.size();
		if (s1[i] != s2[i])
			return -1;
		i++;
	}
	return 1;
}

HttpParser::e_status HttpParser::is_method() {
	for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); i++) {
		size_t diff = same(method, methods[i]);
		if (diff == 0)
			return HttpParser::DONE;
		if (diff == 1)
			return HttpParser::CONTINUE;
	}
	return HttpParser::FAILED;
}

HttpParser::e_status HttpParser::chunked_body_parser(char c) {
	switch (ch_state) {
	case bd_start: {
		if (c == '\r')
			ch_state = HttpParser::bd_chunk_data_crlf;
		else if (isxdigit(c)) {
			ch_state = HttpParser::bd_chunk_size;
		} else
		{
			error = err_invalid_chunk_body;
			return HttpParser::FAILED;
		}
	}
	case bd_chunk_size: {
		if (isxdigit(c)) {
			chunk_size = (chunk_size * 0xF + c - '0');
			break;
		}
	}
	case bd_chunk_sp_after_size: {
		if (c == ' ' || c == '\t')
			break;
		if (c == '\r') {
			ch_state = HttpParser::bd_chunk_size_cr;
			break;
		} else if (c == ';')
		{
			error = err_invalid_chunk_body;
			return HttpParser::FAILED; // specific error should be returned
		}
		else
			return HttpParser::FAILED;
	}
	case bd_chunk_size_cr: {
		if (c == '\n') {
			ch_state = HttpParser::bd_chunk_data;
			break;
		} else
		{
			error = err_invalid_chunk_body;
			return HttpParser::FAILED;
		}
	}
	case bd_chunk_data: {
		if (chunk_size == 0) {
			return HttpParser::DONE;
		}
		chunk_size--;
		chunk.push_back(c);
		if (chunk_size == 0) {
			ch_state = HttpParser::bd_chunk_data_cr;
		}
		break;
	}
	case bd_chunk_data_cr: {
		if (c != '\r')
		{
			error = err_invalid_chunk_body;
			return HttpParser::FAILED;
		}
		ch_state = HttpParser::bd_chunk_data_crlf;
		break;
	}
	case bd_chunk_data_crlf: {
		if (c != '\n')
		{
			error = err_invalid_chunk_body;
			return HttpParser::FAILED;
		}
		ch_state = HttpParser::bd_start;
		break;
	}
	}
	return HttpParser::CONTINUE;
}

HttpParser::e_status HttpParser::headers_parser(char c) {
	switch (h_state) {
	case h_start: {
		if (c == '\r') {
			h_state = HttpParser::h_crlfcr;
			return HttpParser::CONTINUE;
		} else {
			if (strchr("!#$%&'*+-.^_`|~", c) == NULL && !isalnum(c))
			{
				error = err_invalid_header;
				return HttpParser::FAILED;
			}
			h_state = HttpParser::h_key;
		}
	}
	case h_key: {
		if (c == ':') {
			h_state = HttpParser::h_spaces_after_colon;
			return HttpParser::CONTINUE;
		} else if (strchr("!#$%&'*+-.^_`|~", c) != NULL || isalnum(c))
			field.push_back(c);
		else
		{
			error = err_invalid_header;
			return HttpParser::FAILED;
		}
		break;
	}
	case h_spaces_after_colon: {
		last_map = headers.insert(make_pair(field, ""));
		field.clear();
		h_state = HttpParser::h_value;
		if (c == ' ')
			break;
	}
	case h_value: {

		if (c == '\n' || c == '\r') {
			h_state = HttpParser::h_cr;
			if (c == '\r')
				break;
		} else {
			field.push_back(c);
			break;
		}
	}
	case h_cr: {
		if (c == '\n') {
			last_map->second = field;
			field.clear();
			h_state = HttpParser::h_start;
			break;
		} else
		{
			error = err_invalid_header;
			return HttpParser::FAILED;
		}
	}
	case h_crlf: {
		if (c == '\r') {
			h_state = HttpParser::h_crlfcr;
			break;
		}
	}
	case h_crlfcr: {
		if (c == '\n')
			return HttpParser::DONE;
		error = err_invalid_header;
		return HttpParser::FAILED;
		break;
	}
	}
	return HttpParser::CONTINUE;
}

HttpParser::e_status HttpParser::status_line_parser(char c) {
	switch (sl_state) {
	case sl_start: {
		if (c < 'A' || c > 'Z')
		{
			error = err_invalid_method;
			return HttpParser::FAILED;
		}
		sl_state = sl_method_start;
	}
	case sl_method_start: {
		if (c >= 'A' && c <= 'Z') {
			method.push_back(c);
			if (is_method() == HttpParser::FAILED)
			{
				error = err_invalid_method;
				return HttpParser::FAILED;
			}
			return HttpParser::CONTINUE;
		}
		if (is_method() != HttpParser::DONE)
		{
			error = err_invalid_method;
			return HttpParser::FAILED;
		}
		sl_state = sl_sp_after_method;
	}
	case sl_sp_after_method: {
		if (c != ' ')
		{
			error = err_invalid_method;
			return HttpParser::FAILED;
		}
		sl_state = sl_uri_start;
		return HttpParser::CONTINUE;
	}
	case sl_uri_start: {
		if (c != '/')
		{
			error = err_invalid_uri;
			return HttpParser::FAILED;
		}
		uri.push_back(c);
		sl_state = sl_uri;
		return HttpParser::CONTINUE;
	}
	case sl_uri: {
		switch (c) {
		case ' ':
			sl_state = sl_http_start;
			break;
		case '?':
			sl_state = sl_query;
			break;
		default:
			uri.push_back(c);
		}
		return HttpParser::CONTINUE;
	}
	case sl_query: {
		if (c == ' ') {
			sl_state = sl_http_start;
			return HttpParser::CONTINUE;
		}
		query.push_back(c);
		sl_state = sl_query;
		return HttpParser::CONTINUE;
	}
	case sl_http_start: {
		if (c != 'H')
		{
			error = ::err_invalid_version;
			return HttpParser::FAILED;
		}
		sl_state = sl_http_H;
		return HttpParser::CONTINUE;
	}
	case sl_http_H: {
		if (c != 'T')
		{
			error = err_invalid_version;
			return HttpParser::FAILED;
		}
		sl_state = sl_http_HT;
		return HttpParser::CONTINUE;
	}
	case sl_http_HT: {
		if (c != 'T')
		{
			error = err_invalid_version;
			return HttpParser::FAILED;
		}
		sl_state = sl_http_HTT;
		return HttpParser::CONTINUE;
	}
	case sl_http_HTT: {
		if (c != 'P')
		{
			error = err_invalid_version;
			return HttpParser::FAILED;
		}
		sl_state = sl_http_HTTP;
		return HttpParser::CONTINUE;
	}
	case sl_http_HTTP: {
		if (c != '/')
		{
			error = err_invalid_version;
			return HttpParser::FAILED;
		}
		sl_state = sl_http_slash;
		return HttpParser::CONTINUE;
	}
	case sl_http_slash: {
		if (c != '1')
		{
			error = err_not_implemented_version;
			return HttpParser::FAILED;
		}
		major_version = c - '0';
		sl_state = sl_http_major;
		return HttpParser::CONTINUE;
	}
	case sl_http_major: {
		if (c != '.')
		{
			error = err_invalid_version;
			return HttpParser::FAILED;
		}
		sl_state = sl_http_dot;
		return HttpParser::CONTINUE;
	}
	case sl_http_dot: {
		if (c != '1')
		{
			error = err_not_implemented_version;
			return HttpParser::FAILED;
		}
		minor_version = c - '0';
		sl_state = sl_http_minor;
		return HttpParser::CONTINUE;
	}
	case sl_http_minor: {
		sl_state = sl_almost_done;
		if (c == '\r')
			return HttpParser::CONTINUE;
		if (c == '\n') {
		} else
		{
			error = err_invalid_status_line;
			return HttpParser::FAILED;
		}
	}
	case sl_almost_done: {
		if (c != '\n')
		{
			error = err_invalid_status_line;
			return HttpParser::FAILED;
		}
		sl_state = sl_start;
		return HttpParser::DONE;
	}
	}
	return DONE;
}

HttpParser& HttpParser::operator=(const HttpParser& other) {
    if (this != &other) {
        this->chunk = other.chunk;
        this->field = other.field;
        this->last_map = other.last_map;
        this->body_size = other.body_size;
        this->chunk_size = other.chunk_size;
        this->method = other.method;
        this->uri = other.uri;
        this->body = other.body;
        this->query = other.query;
        this->major_version = other.major_version;
        this->minor_version = other.minor_version;
        this->state = other.state;
        this->sl_state = other.sl_state;
        this->h_state = other.h_state;
        this->ch_state = other.ch_state;
        this->encoding = other.encoding;
        this->state = other.state;
        this->headers = other.headers;
    }
    return *this;
}

void HttpParser::reset() {
	state = HttpParser::p_status_line;
    sl_state = HttpParser::sl_start;
    h_state = HttpParser::h_start;
    ch_state = HttpParser::bd_start;
    encoding = HttpParser::none;
    h_state = HttpParser::h_start;
	error = err_none;
    body_size = 0;
    chunk_size = 0;
    method.clear();
    uri.clear();
    query.clear();
    major_version = 0;
    minor_version = 0;
    field.clear();
    chunk.clear();
    headers.clear();
}

