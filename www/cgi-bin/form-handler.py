#!/usr/bin/env python

import cgi

# Set the content type to HTML
print("Content-Type: text/html\r\n\r\n")
# print()

# Get the form data from the request
form = cgi.FieldStorage()

# Extract the form field values
name = form.getvalue('name')
email = form.getvalue('email')
message = form.getvalue('message')

# Print out the form data
print("<html>")
print("<body>")
print("<h1>Form Data</h1>")
print("<p>Name: %s</p>" % name)
print("<p>Email: %s</p>" % email)
print("<p>Message: %s</p>" % message)
print("</body>")
print("</html>")
