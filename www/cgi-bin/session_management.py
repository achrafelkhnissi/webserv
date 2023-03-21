#!/usr/bin/python

# Import modules for CGI handling
import cgi, cgitb

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')

# get cookies
import os
import Cookie
cookie = Cookie.SimpleCookie()
cookie_string = os.environ.get('HTTP_COOKIE')
cookie.load(cookie_string)


# Check if the cookie exists

if cookie.has_key('user') and cookie['user'].value == first_name + ' ' + last_name:
    print "Content-type:text/html\r\n\r\n"
    print "Cookie exists"
    print cookie['user'].value
    print "<h1>Welcome back %s</h1>" % cookie['user'].value
    exit()

## store cookies
cookie['user'] = first_name + ' ' + last_name

print "Set-Cookie: user=%s\r\n" % (first_name + ' ' + last_name)

print "Content-type:text/html\r\n\r\n"

print "\n<html>"
print "<head>"
print "<title>Hello - Second CGI Program</title>"
print "</head>"
print "<body>"
print "<h2>Hello %s %s</h2>" % (first_name, last_name)
print "</body>"
print "</html>"
