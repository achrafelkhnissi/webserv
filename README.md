# webserv

## NOTES
### Requirements
- `poll()` (or equivalent) must check read and write at the same time.
- A request to your server should never hang forever.
- You can’t use fork for something else than CGI (like PHP, or Python, and so forth).
- Clients must be able to upload files.
- Stress tests your server. It must stay available at all cost*

### Configuration file
* Setup the server_name or not
* The first server for a host:port will be the default for thishost:port(that means it will answer to all the requests that don’t belong to another server).
* Limit client body size.
* Setup routes with one or multiple of the following rules/configuration:
    - Define a list of accepted HTTP methods for the route.
    - Define a HTTP redirection.
    - Define a directory or a file from where the file should be searched for example (for example,if url `/kapouet` is rooted to `/tmp/www`, url `/kapouet/pouic/toto/pouet` is `/tmp/www/pouic/toto/pouet`).
    - Turn on or off directory listing.
    - Set a default file to answer if the request is a directory.

### TODO:
- Test with a host other than `localhost`
- If the `server_name` is not set, the server must answer to any host.
- If `listener IP:PORT` is specified, the server must answer to this IP:PORT only.
- If `listener *:PORT` is specified, the server must answer to any IP on this PORT.

### Useful links
* [HTTP/1.1: Status Code Definitions](https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html)
* [HTTP/1.1: Message Syntax and Routing](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html)
* [HTTP/1.1: Semantics and Content](https://www.w3.org/Protocols/rfc2616/rfc2616-sec7.html)
* [HTTP/1.1: Header Field Definitions](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
* [HTTP/1.1: Caching](https://www.w3.org/Protocols/rfc2616/rfc2616-sec13.html)
* [HTTP/1.1: Range Requests](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
* [HTTP/1.1: Conditional Requests](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
* [HTTP/1.1: Authentication](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
* [HTTP/1.1: Content Negotiation](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
* [HTTP/1.1: Message Length](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html)
* [HTTP/1.1: Transfer Codings](https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html)
* [HTTP/1.1: Chunked Transfer Coding](https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html)
* [HTTP/1.1: Message Body Length](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html)
* [HTTP/1.1: Message Body](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html)
* [HTTP/1.1: Message Headers](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html)
* [HTTP/1.1: Message Syntax and Routing](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html)
* [HTTP/1.1: Message Headers](https://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html)
