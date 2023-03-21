#!/usr/bin/python

body = """"

<!DOCTYPE html>
<html>
<head>
	<title>Hello CGI</title>
	<style>
		body {
			font-family: Arial, sans-serif;
			background-color: black;
		}

		h1 {
			color: white;
			font-size: 6rem;
			text-align: center;
			margin-top: 5rem;
			text-shadow: 1px 1px 0px #fff, -1px -1px 0px #fff, 1px -1px 0px #fff, -1px 1px 0px #fff;
		}
		
		p {
		    color: white;
		}
	footer {
	width: 100%;
	position: absolute;
	bottom: 0;
	text-align: center;
	 }	
	 </style>
</head>
<body>
	<h1>CGI</h1>
	<div align="center">
	<img src="https://www.memecreator.org/static/images/memes/4359802.jpg" alt="Hello CGI">
	</div>
	
	<footer>
    <p>webserv/1.0</p>
</footer>
</body>

</html>


"""

print "Content-type:text/html\r"
print "Content-length: %d\r" % len(body)
print "\r"
print body

