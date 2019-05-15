# Morse
Command line utility to send emails

## Building (macOS)

### Dependencies

* [`libcurl` 7.56.0 or above](https://curl.haxx.se/libcurl/c/libcurl.html) for HTTP requests

* [`libsodium`](https://github.com/jedisct1/libsodium) for cryptography

These are in the `lib/` folder.


### Compile


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
- [x] Do not depend on environment for availability of dependencies

