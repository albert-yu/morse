# Morse
Command line utility to send emails

## Building (macOS)

### Dependencies

* [`libcurl` 7.56.0 or above](https://curl.haxx.se/libcurl/c/libcurl.html) for HTTP requests
  * The default version of `curl` on macOS may be too old--install the newer one with Homebrew: 
  ```bash
  brew install curl
  ```

* [`libsodium`](https://github.com/jedisct1/libsodium) for cryptography

Drop these in the `lib/` folder or update their locations in the Makefile.

```Makefile
LDFLAGS += -L/usr/local/opt/curl/lib
LDFLAGS += -L/usr/local/Cellar/libsodium/1.0.16/lib
```

### Compile

In order to authenticate to Google, you need a file called `secrets.h` modelled after `secrets_temp.h`. It should contain the following information:

```C
#define GOOGLE_CLIENT_ID "<your client ID>"
#define GOOGLE_CLIENT_SECRET "<your client secret>"

// crypto
#define SECRET_KEY "<a hexadecimal string that is used to encrypt and decrypt the credentials>"
```

With [`GNU Make`](https://www.gnu.org/software/make/manual/html_node/Overview.html#Overview), just run

```bash
make
```


## Usage

TODO

## To-do list
- [ ] Implement caching
- [ ] Replace strarray usages with curl linked list
- [x] Get user email from token
- [ ] Receive emails
- [x] Support multiple recipients
- [x] Compile to static library
- [ ] Compile to debug
- [x] Add separate `morse.h` header for API
- [ ] Cross-platform compile
  * [ ] macOS
  * [ ] Linux
  * [ ] Windows
- [ ] Do not depend on environment for availability of dependencies
