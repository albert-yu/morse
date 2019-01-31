# Morse
Command line utility to send emails

## Building (macOS)

### Dependencies

* [`cmark-gfm`](https://github.com/github/cmark-gfm) for Markdown compilation

Set the location of `libcmark-gfm.a` in `Makefile`. In my environment, I installed cmark with [`homebrew`](https://brew.sh),

```bash
brew install cmark-gfm
```

so I set the `CMARK` path to:

```Makefile
CMARK=/usr/local/Cellar/cmark-gfm/0.28.3.gfm.16/lib/libcmark-gfm.a
```

Note: You may have to uninstall `cmark` in your environment first.

One can use `cmark` instead, but GitHub's implementation adds support for tables.

* [`libcurl`](https://curl.haxx.se/libcurl/c/libcurl.html) for HTTP requests

* [`libsodium`](https://github.com/jedisct1/libsodium) for cryptography

### Compile

In order to authenticate to Google, you need a file called `secrets.h` modelled after `secrets_temp.h`. It should contain the following information:

```C
#define GOOGLE_CLIENT_ID "your client ID"
#define GOOGLE_CLIENT_SECRET "your client secret"

// crypto
#define SECRET_KEY "a hexadecimal string that is used to encrypt and decrypt the credentials"
```

With [`GNU Make`](https://www.gnu.org/software/make/manual/html_node/Overview.html#Overview), just run

```bash
make
```


## Usage

TODO
