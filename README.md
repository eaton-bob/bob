# bob

PROBLEM: The following protocol is not implemented

== PROTOCOL rfc-hello-world

Connects USER peer to PROVIDER peer.

USER sends the following message using MAILBOX SEND to PROVIDER:

* HELLO

PROVIDER peer MUST reply with one of the following messages

* HI/<name of requester>
* ERROR

where '/' indicates a multipart string message, <name of requester> is the name
of agent sending the request.

