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