1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully received by using markers or protocols that indicate the end of a message. In this case, the server sends an EOF (End of File) character or uses a fixed-length message, such as appending a special byte sequence or a delimiter at the end of the message. Techniques are like buffering and recv()

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol should define a clear start and end for each command, despite TCP's stream nature. This can be done by EOF marker or fixed-length messages. Doing this wrong will result in data loss, buffer overflows, etc.

3. Describe the general differences between stateful and stateless protocols.

Stateful Protocols: These protocols maintain information about the current state of the interaction. Stateless Protocols: These protocols do not maintain any state between requests. Each communication is independent, and there is no retained memory of prior interactions.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is "unreliable" in that it does not guarantee delivery, ordering, or error correction. However, it is still useful in scenarios where speed and low latency are more important than reliability, or where higher-level protocols manage reliability.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides the socket API as the abstraction to enable network communication. The socket interface allows applications to communicate over the network using protocols like TCP and UDP.