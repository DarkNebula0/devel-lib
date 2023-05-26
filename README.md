## Devel c++ lib

**This repository is now archived and will no longer be actively maintained.**

A new and improved version of this library is available at a different repository, specifically designed to support Linux. You can find the new repository at the following link:

[https://github.com/DarkNebula0/devel-library](https://github.com/DarkNebula0/devel-library)

Please note that the new repository is a rewrite of the original library and includes enhanced features and Linux compatibility. We encourage you to check it out and contribute to its development.

## Repository Status

This repository is now archived and no longer accepts new pull requests or bug reports. You can still fork the repository and make modifications for your own use.

For any issues related to the new repository, please refer to the README of the new repository at [https://github.com/DarkNebula0/devel-library](https://github.com/DarkNebula0/devel-library).

<br/>

    -- It was written only for Windows and x64 in mind --

    The library was mainly written by a buddy and me to provide utils fuction for developing private servers and other projects we did back then.

    Since it has been lonely in a private repo on bitbucket since 2018 and I could use some code snippets again for another project, I thought I'd upload it to a public repo on github.

    Maybe someone will find interesting functions that he can use. There is no warranty and I would not trust the SecureClient or Websocket protocol.

<br/>

Features:

- Network, everything you can need for a server, TCP, UDP, Socket and experimental web stuff.
- Serialize system, was used to quickly build packages for network protocols
- Thread stuff, Pool and Task systems
- MySQL stuff, queries, orm?
- Mutex utils
- File system stuff
- Json parser
- Ini file parser
- and many more utils

<br/>

## Build dependencies

---

#### 1. Install vcpkg and its Visual Studio integration

    > git clone https://github.com/microsoft/vcpkg
    > .\vcpkg\bootstrap-vcpkg.bat

#### 2. Install openssl

    > .\vcpkg\vcpkg install openssl-windows:x64-windows

#### 2. Install MySQL C Connector

- Download MySQL C Connector6.1.11 from `https://downloads.mysql.com/archives/c-c/`
- Copy lib and include folder to `external\mysql\`
