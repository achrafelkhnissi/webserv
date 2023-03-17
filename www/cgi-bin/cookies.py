#!/usr/bin/env python3

import os
import http.cookies as cookies

# Set the content-type of the response to text/html
print("Content-type: text/html\n")

# Get the existing session cookie (if any)
cookie_string = os.environ.get('HTTP_COOKIE', '')
session_cookie = cookies.SimpleCookie(cookie_string).get('session')

# Generate a new session ID if one doesn't exist in the cookie
if session_cookie is None:
    import uuid
    session_id = str(uuid.uuid4())
    session_cookie = cookies.SimpleCookie()
    session_cookie['session'] = session_id

# Store the session ID in a session data store (e.g. a database or file)
# Here, we'll just print the session ID to the console
print(f"Session ID: {session_cookie.value}")

# Set the session cookie to expire in 1 hour
session_cookie['session']['expires'] = 60*60

# Send the session cookie in the response headers
print(session_cookie.output())
