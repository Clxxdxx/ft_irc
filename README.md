*This project has been created as part of the 42 curriculum by clalopez, ancarret and jaboga-d.*

# ft_irc

## Description

**ft_irc** is a custom implementation of an **IRC (Internet Relay Chat) server** written in **C++**, following the specifications of the IRC protocol (RFC 1459 / RFC 2812).

The goal of this project is to build a functional IRC server capable of handling multiple clients simultaneously, managing channels, processing commands, and maintaining stable communication between connected users.

This project focuses on:

* Understanding **network programming**
* Implementing **TCP socket communication**
* Managing **multiple clients**
* Designing a **modular architecture**
* Parsing and executing **IRC commands**
* Handling **concurrent communication**

The server supports multiple users, channels, and core IRC commands, allowing clients such as **HexChat** to connect and interact.

---

## Features

The IRC server implements core IRC functionalities including:

* TCP server using **IPv4**
* Multiple client handling
* Nickname registration
* Username registration
* Channel creation and management
* User join and leave channels
* Message broadcasting
* Private messaging
* Operator privileges
* Channel modes
* Command parsing
* Client disconnection handling

### Implemented Commands

Typical IRC commands implemented:

* `PASS`
* `NICK`
* `USER`
* `JOIN`
* `PRIVMSG`
* `QUIT`
* `KICK`
* `INVITE`
* `TOPIC`
* `MODE`

---

## Project Structure

```
.
├── include
│   ├── Channel.hpp
│   ├── Client.hpp
│   ├── CommandDispatcher.hpp
│   ├── Server.hpp
│   ├── SimpleParser.hpp
│   └── Utils.hpp
├── src
│   ├── commands
│   │   └── CommandDispatcher.cpp
│   ├── model
│   │   ├── Channel.cpp
│   │   └── Client.cpp
│   ├── parser
│   │   └── SimpleParser.cpp
│   ├── server
│   │   └── Server.cpp
│   ├── main.cpp
│   └── utils.cpp
├── Makefile
└── README.md
```

### Architecture Overview

* **Server**

  * Handles socket creation and connections
  * Manages multiple clients
  * Coordinates communication

* **Client**

  * Represents a connected user
  * Stores nickname, username, and connection state

* **Channel**

  * Manages users inside a channel
  * Handles broadcasting and channel modes

* **CommandDispatcher**

  * Routes commands to their handlers

* **SimpleParser**

  * Parses raw IRC messages into structured commands

* **Utils**

  * Helper functions

---

## Compilation Instructions

To compile the project:

```bash
make
```

This will generate the executable:

```bash
./ircserv
```

To clean object files:

```bash
make clean
```

To remove everything:

```bash
make fclean
```

To rebuild:

```bash
make re
```

---

## Execution Instructions

Run the server using:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword
```

Where:

* `<port>` → TCP port number
* `<password>` → connection password required by clients

---

## Connecting to the Server

You can connect using IRC clients such as:

* **HexChat**

Example with **netcat**:

```bash
nc localhost 6667
```

Then:

```irc
PASS mypassword
NICK mynick
USER myuser 0 * :Real Name
```

---

## Example Usage

Create and join a channel:

```irc
JOIN #general
```

Send message:

```irc
PRIVMSG #general :Hello everyone!
```

Leave channel:

```irc
PART #general
```

Private message:

```irc
PRIVMSG nickname :Hello!
```

---

## Technical Choices

This project was developed using:

* **C++ (C++98 standard)**
* **TCP sockets**
* **poll()** for handling multiple clients
* Object-oriented design
* Modular architecture
* Custom IRC message parsing

Key design decisions:

* Separation between **parsing**, **networking**, and **command handling**
* Channel-based communication model
* Dispatcher-based command execution
* Efficient client management using polling

---

## Error Handling

The server includes handling for:

* Invalid commands
* Authentication failures
* Invalid nicknames
* Channel permission violations
* Client disconnections
* Socket errors

---

## Resources

### IRC Protocol Documentation

* RFC 1459 — Internet Relay Chat Protocol
* RFC 2812 — Internet Relay Chat: Client Protocol

Recommended reading:

* https://datatracker.ietf.org/doc/html/rfc1459
* https://datatracker.ietf.org/doc/html/rfc2812

### Networking Resources

* Beej's Guide to Network Programming
  https://beej.us/guide/bgnet/

* Linux poll() documentation
  https://man7.org/linux/man-pages/man2/poll.2.html

### C++ Resources

* cppreference
  https://en.cppreference.com/

---

## AI Usage Disclosure

Artificial Intelligence tools were used during the development of this project.

AI was used for:

* Understanding IRC protocol behaviors
* Debugging socket communication issues
* Reviewing architecture ideas
* Clarifying C++ syntax and standard behavior
* Generating documentation examples
* Improving README structure and clarity

AI was **not used** to generate complete project implementations. All core logic, architecture, and functionality were written and designed manually.

---

## Testing

The server was tested using:

* Multiple IRC clients
* Manual socket testing
* Simultaneous client connections
* Channel operations
* Command validation

Stress tests included:

* Multiple users joining channels
* Rapid message sending
* Unexpected disconnections

