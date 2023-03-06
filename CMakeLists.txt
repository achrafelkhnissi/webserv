cmake_minimum_required(VERSION 3.24)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

include_directories(inc)
include_directories(srcs/config)
include_directories(srcs/lib/toml)
include_directories(srcs/lib/toml/data)
include_directories(srcs/lib/toml/error)
include_directories(srcs/lib/toml/parser)
include_directories(srcs/lib/toml/result)
include_directories(srcs/lib/toml/tokenizer)
include_directories(srcs/lib/toml/tokenizer/token)
include_directories(srcs/request)
include_directories(srcs/request/parser)
include_directories(srcs/server)
include_directories(srcs/CGIHandler)
include_directories(srcs/request)
include_directories(srcs/request/parser)

add_executable(webserv
        inc/typedefs.hpp
        inc/utils.hpp
        srcs/config/Configuration.cpp
        srcs/config/Configuration.hpp
        srcs/lib/toml/data/table.cpp
        srcs/lib/toml/data/table.hpp
        srcs/lib/toml/error/ParseError.cpp
        srcs/lib/toml/error/ParseError.hpp
        srcs/lib/toml/parser/Parser.cpp
        srcs/lib/toml/parser/Parser.hpp
        srcs/lib/toml/result/Result.hpp
        srcs/lib/toml/syntax/checker.cpp
        srcs/lib/toml/tokenizer/token/Token.cpp
        srcs/lib/toml/tokenizer/token/Token.hpp
        srcs/lib/toml/tokenizer/Lexer.cpp
        srcs/lib/toml/tokenizer/Lexer.hpp
        srcs/lib/toml/toml.cpp
        srcs/lib/toml/toml.hpp
        srcs/request/Request.cpp
        srcs/request/Request.hpp
        srcs/CGIHandler/CGIHandler.cpp
        srcs/CGIHandler/CGIHandler.hpp
        srcs/server/Server.cpp
        srcs/server/Server.hpp
        srcs/server/SubServer.cpp
        srcs/server/SubServer.hpp
        srcs/server/VirtualServer.cpp
        srcs/server/VirtualServer.hpp
        srcs/utils/utils.cpp
        srcs/main.cpp
        srcs/request/parser/HttpParser.cpp
        srcs/request/parser/HttpParser.cpp
        srcs/request/Request.cpp
        srcs/request/Request.hpp
        )
