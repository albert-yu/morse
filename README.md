# Morse

C library for sending emails.

[![CI Status](https://img.shields.io/travis/albert-yu/morse.svg?style=flat)](https://travis-ci.org/albert-yu/morse)
[![Version](https://img.shields.io/cocoapods/v/morse.svg?style=flat)](https://cocoapods.org/pods/morse)
[![License](https://img.shields.io/cocoapods/l/morse.svg?style=flat)](https://cocoapods.org/pods/morse)
[![Platform](https://img.shields.io/cocoapods/p/morse.svg?style=flat)](https://cocoapods.org/pods/morse)


## Installation

`morse` is available through [CocoaPods](https://cocoapods.org). To install
it, simply add the following line to your Podfile:

```ruby
pod 'morse'
```


## Building (macOS)

### Dependencies

* [`libcurl` 7.56.0 or above](https://curl.haxx.se/libcurl/c/libcurl.html) for HTTP requests

* [`libsodium`](https://github.com/jedisct1/libsodium) for cryptography

These are in the `lib/` folder.


### Compile

`libcurl` has headers included, but `libsodium`'s headers need to be made available through an install.

```bash
brew install libsodium
```

Then, with [`GNU Make`](https://www.gnu.org/software/make/manual/html_node/Overview.html#Overview), just run

```bash
make
```


## Usage

The main API functions are declared `send.h` and `receive.h`, but these are both referenced by `morse.h`. 

## To-do list
- [ ] Use configuration file
- [ ] Implement caching
- [ ] Clean up API design
- [ ] Replace strarray usages with curl linked list
- [x] Get user email from token
- [ ] Receive emails
- [x] Support multiple recipients
- [x] Compile to static library
- [ ] Compile to debug
- [x] Add separate `morse.h` header for API
- [ ] Cross-platform compile
  * [x] macOS
  * [ ] Linux
  * [ ] Windows
- [x] Do not depend on environment for availability of dependencies

## License

`morse` is available under the MIT license. See the LICENSE file for more info.
