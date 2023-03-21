
<h1 align="center" style="font-size: 150px">webserv</h1>

<p align="center">
<img src="./assets/How-Server-Work.png" alt="How a server works" width="1000"/>
</p>

<p align="center">
	<b><i>Project developed at <a href="https://www.1337.ma/">1337 Khouribga </a> coding school.</i></b><br>
    <b><i>Authors: <a href="">@JamiaFathiya</a> & <a href="">@achrafelkhnissi</a> & <a href="">@abellaismail7</a></i></b>
<!--
 <b><i>Grade: 125/100</i></b>
-->
</p>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/achrafelkhnissi/webserv?color=blueviolet" />
	<img alt="Number of lines of code" src="https://img.shields.io/tokei/lines/github/achrafelkhnissi/webserv?color=blueviolet" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/achrafelkhnissi/webserv?color=blue" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/achrafelkhnissi/wevserv?color=blue" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/achrafelkhnissi/webserv?color=brightgreen" />
</p>

---

## Table of Contents
- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)
- [Features](#features)
- [Resources](#resources)
- [Contact](#contact)


# HTTP

## What is HTTP?
HTTP stands for Hypertext Transfer Protocol, which is a protocol used to transfer hypertext. This raises two questions: what is a protocol, and what is hypertext? In this context, a protocol is a system of rules that enables the communication of information between different entities, such as computers. Hypertext is an outdated term for text displayed on a computer screen containing hyperlinks to other text, i.e., web documents. Therefore, the hypertext transfer protocol refers to the set of rules, servers, and browsers used to transfer web documents back and forth.

HTTP allows sessions, and stored states are shared between the browser and server. If a visitor is browsing a photo gallery, the browser and server can exchange information about the visitor's location in the sequence by passing information back and forth in the form of cookies. When the page is reloaded, the browser sends a cookie to the server indicating the last position visited, enabling the viewer to resume from that point. This means that while HTTP is stateless, it is not sessionless.

## Terminology

| Term | Definition |
| --- | --- |
| Browser | An application used to access and navigate between HTML documents. |
| User Agent | An application that acts on behalf of the user, typically a browser. |
| TCP | Short for Transmission Control Protocol, one of the main internet protocols used by the World Wide Web, email, FTP, and remote administration. |
| IP | Short for Internet Protocol. IP is used to transfer data between computers over a network. Every device connected to the internet has an IP address. |
| URL | Short for Uniform Resource Locator, an address pointing at a resource on the web. |
| DNS | Short for Domain Name Server. DNS catalogs all domain name URLs and points them to the IP addresses of servers. |
| Resource | The file or files available on a server when following a URL. |
| Server | A computer on the internet running some form of data storage and sharing application, most commonly a web server application. |
| Proxy | Software or hardware service acting as a middle person between clients and servers. |
| Request-Response Pairs | Clients and servers communicate over HTTP using request-response pairs. A request is sent, and a response is returned. |
| Header | Requests and responses use HTTP headers to pass information back and forth. |
| HTTP Request Method/Verb | Every HTTP request contains a method, also known as a verb, that explains what action the sender wants to perform on the resource (e.g., GET, PUT, DELETE). |
| Status Response Code | A numerical code in the 100 to 500 range describing what type of response the server sent back to the client. |
| Cache | A method for storing data on the client or the server to speed up performance. |
| Stateless | HTTP is stateless, meaning every request and response is unique, and no state is stored. |
| Cookie | A string of data passed back and forth between the client and server to create a stateful session. |
| Session | Clients and servers can share information about states by passing information back and forth, creating a session. |


## The HTTP Flow
1. First, the browser opens a TCP connection to the server. This ensures data can be sent back and forth over the network and that the data sent from one end is put together the same way at the other end. If the connection happens over HTTPS, TLS certificates are exchanged to ensure only the computer and the server can encrypt and decrypt the transmitted data. This prevents anyone from being able to eavesdrop on the conversation between the client and the server and steal the data they are transmitting.
2. Second, the browser sends an HTTP message. This message always contains an HTTP method, like GET, PUT, DELETE or something similar, and a URL pointing at the requested resource. It can also contain headers like cookies or authentication data and data if the browser is submitting data to the server using the post, put, or path methods.

<img src="./http_flow_1.png">

3. Third, the server performs the requested actions and sends a response back to the browser. This response will contain an HTTP status message indicating what happened, headers with information about the response, and whatever data was requested. This data could be an HTML document or a style sheet or a JavaScript file or image or any other type of content used in a standard website.

<img src="./http_flow_1.png">

4. Finally, once the response is fully received, the TCP connection is closed. Since HTTP is stateless, we are now back to a clean slate.


## HTTP Method table

| Method | Description | Success | Failure |
| --- | --- | --- | --- |
| GET | Get the specified resource, if available | 200 OK | 404 Not Found |
| POST | Create a new resource and add it to a collection | 201 Created | 401 Unauthorized, 409 Conflict, 404 Not Found |
| PUT | Update an existing singleton resource-based or id | 200 OK | 401 Unauthorized, 404 Not Found, 405 Method not Allowed |
| PATCH | Modify an existing singleton resource-based or ID | 200 OK | 401 Unauthorized, 404 Not Found, 405 Method not Allowed |
| DELETE | Delete singleton resource-based or ID (you can’t delete a collection of resources) | 200 OK | 401 Unauthorized, 404 Not Found |
| OPTION | Get the options available from this resource | 200 OK | - |
| HEAD | Get just the response headers from the resource | 200 OK | 404 Not Found |
