#!/usr/bin/python

# Import modules for CGI handling
import cgi, cgitb

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')

# get cookies
import os
import Cookie
cookie = Cookie.SimpleCookie()
cookie_string = os.environ.get('HTTP_COOKIE')
cookie.load(cookie_string)


# Check if the cookie exists
body = "aa"

method = os.environ.get('REQUEST_METHOD', None)
if method == "POST" and  not ' ' in first_name:
    cookie['user'] = first_name
    print "Set-Cookie: user=%s\r" % (first_name)
if cookie.has_key('user'):
    first_name = cookie['user'].value



if not cookie.has_key('user'):
    body="""
    <!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>webserv index</title>
</head>

<form action = "/cgi-bin/session_management.py" method = "POST">
First Name: <input type = "text" name = "first_name">  <br />
<input type = "submit" value = "Submit" />
</form>

</html>
    """

else:
    body = "<html>"
    body += "<head>"
    body += "<title>Hello - Second CGI Program</title>"
    body += "</head>"
    body += "<body>"
    body += "<h2>Hello %s</h2>" % (first_name)
    body += "</body>"
    body += "</html>"

print "Content-type:text/html\r"
print "Content-Length: %d\r" % len(body)
print "\r"
print(body)
