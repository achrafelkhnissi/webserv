#include "HttpParser.hpp"

HttpParser::HttpParser() {
	state = HttpParser::p_status_line;
	sl_state = HttpParser::sl_start;
}

HttpParser::~HttpParser() {
}

HttpParser::e_status HttpParser::append(char c) {
	switch (state) {
	case HttpParser::p_status_line:
		return status_line_parser(c);
		break;
	case HttpParser::p_headers:
		break;
	case HttpParser::p_body:
		break;
	}
	return HttpParser::FAILED;
}

HttpParser::e_status HttpParser::push(std::string &chunk) {
	for (size_t i = 0; i < chunk.size(); i++) {
		switch (append(chunk[i])) {
		case HttpParser::DONE: {
			if (state == HttpParser::p_body)
				return HttpParser::DONE;
			break;
		}
		case HttpParser::FAILED:
			return HttpParser::FAILED;
		case HttpParser::CONTINUE:
			break;
		}
	}
	return HttpParser::CONTINUE;
}

HttpParser::e_state& operator++(HttpParser::e_state& s) {
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

HttpParser::e_status HttpParser::status_line_parser(char c) {
	switch (sl_state) {
	case sl_start: {
		if (c < 'A' || c > 'Z')
			return HttpParser::FAILED;
		sl_state = sl_method_start;
	}
	case sl_method_start: {
		if (c >= 'A' && c <= 'Z') {
			method.push_back(c);
			return HttpParser::CONTINUE;
		}
		sl_state = sl_sp_after_method;
	}
	case sl_sp_after_method: {
		if (c != ' ')
			return HttpParser::FAILED;
		sl_state = sl_uri_start;
		return HttpParser::CONTINUE;
	}
	case sl_uri_start: {
		if (c != '/')
			return HttpParser::FAILED;
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
			return HttpParser::FAILED;
		sl_state = sl_http_H;
		return HttpParser::CONTINUE;
	}
	case sl_http_H: {
		if (c != 'T')
			return HttpParser::FAILED;
		sl_state = sl_http_HT;
		return HttpParser::CONTINUE;
	}
	case sl_http_HT: {
		if (c != 'T')
			return HttpParser::FAILED;
		sl_state = sl_http_HTT;
		return HttpParser::CONTINUE;
	}
	case sl_http_HTT: {
		if (c != 'P')
			return HttpParser::FAILED;
		sl_state = sl_http_HTTP;
		return HttpParser::CONTINUE;
	}
	case sl_http_HTTP: {
		if (c != '/')
			return HttpParser::FAILED;
		sl_state = sl_http_slash;
		return HttpParser::CONTINUE;
	}
	case sl_http_slash: {
		if (c != '1')
			return HttpParser::FAILED;
		major_version = c - '0';
		sl_state = sl_http_major;
		return HttpParser::CONTINUE;
	}
	case sl_http_major: {
		if (c != '.')
			return HttpParser::FAILED;
		sl_state = sl_http_dot;
		return HttpParser::CONTINUE;
	}
	case sl_http_dot: {
		if (c != '1')
			return HttpParser::FAILED;
		minor_version = c - '0';
		sl_state = sl_http_minor;
		return HttpParser::CONTINUE;
	}
	case sl_http_minor: {
		if (c != '\r')
			return HttpParser::FAILED;
		sl_state = sl_almost_done;
		return HttpParser::CONTINUE;
	}
	case sl_almost_done: {
		if (c != '\n')
			return HttpParser::FAILED;
		return HttpParser::DONE;
	}
	}
	return DONE;
}
