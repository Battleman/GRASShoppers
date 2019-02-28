# GRASS

GRASS (GRep AS a Service) is an online search and file management service that implements clear-text remote command execution, file transfer, and search.

## Team members

The team tag is GRASShoppers:

 * Cattin Johan
 * Cloux Olivier
 * Genet Aymeric

## Structure

| Folder     | Description  |
|------------|--------------|
| `src\`     | Source files |
| `tst\`     | Test suite |
| 'bin\'     | Binary folder |
| `include\` | Common includes |
| `report\`  | Project report |
| `exploit\` | Exploit scripts |

Compile each separately with `make <client|server|test|report>`. Use `make clean` to refresh all files.

## Commands supported

All commands are sent by an `\n`.

| Command  |     Parameters     | Logged | Description |
|----------|--------------------|--------|-------------|
| `login`  | `<username>`       | No  | Starts authentication. The username must be one of the allowed usernames in the configuration file. |
| `pass`   | `<password>`       | No  | Confirms authentication. The password must match the one associated with the username from the previous `login` command in the configuration file. |
| `ping`   | -                  | No  | Responds with the output of the Unix command `ping <host> -c 1`. |
| `ls`     | -                  | Yes | Lists the available files in the working directory in the format as reported by `ls -l`. |
| `cd`     | `<directory>`      | Yes | Changes current working directory to the specified one. |
| `mkdir`  | `<directory>`      | Yes | Creates a new directory with the specified name in the working directory. |
| `rm`     | `<file>`           | Yes | Removes a specified file in the working directory. |
| `get`    | `<file>`           | Yes | Requests the specified file in the working directory. The server responds with: `get port: <port> size: <filesize>` where the client can connect to download the file. The server may only leave one port open per client. The client and server must handle failure conditions. |
| `put`    | `<file>`, `<size>` | Yes | Sends the specified file from the client's local working directory to the server's working directory. The server responds with: `put port: <port>` where the client can connect to upload the file. The server may only leave one port open per client. In this instance, the server will spawn a thread to receive the file from the clients sending thread. The client and server must handle failure conditions. |
| `grep`   | `<pattern>`        | Yes | Searches every file in the current directory and its subdirectory for the requested pattern. The server responds with: `<fileaddress> <endline>`, a line-separated list of addresses  |
| `date`   | -                  | Yes | Responds with the output of the Unix command `date`. |
| `whoami` | -                  | Yes | Responds with the name of the currently logged user. |
| `w`      | -                  | Yes | Responds with a line-separated list of currently logged users. |
| `logout` | -                  | Yes | Logs the user out of their session. |
| `exit`   | -                  | No  | Signals the end of the session. |

The `grep`should follow [the Extended Regular Expressions rules](https://www.gnu.org/software/grep/manual/html_node/Regular-Expressions.html).

### Bonus

Bonus points are earned if multiple parallel uploads and downloads can be done at the same time.

## Configuration file

The `grass.conf` must be in the same directory of the server and must support:

 * `base <path>` : base directory.
 * `port <number>` : port connection
 * `user <username> <password>` : registered logins

## Code vulnerabilities

Deliberately inject:

|      Vulnerability    | Amount |
|-----------------------|--------|
| Stack buffer overflow |   2    |
| Format string         |   1    |
| Command injection     |   1    |
| Of our choice         |   1    |

**NO** code obfuscation.

### Proofs of concept

Provide short executable script for each vulnerability + README.txt which details how to run the exploit scripts.

### Extra backdoors

Add up to 3 extra vulnerabilities. No proof of concept required, but full exploitation must be detailed in report.