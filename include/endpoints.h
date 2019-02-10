#ifndef ENDPOINTS_H
#define ENDPOINTS_H


#ifdef __cplusplus
extern "C" {
#endif

// <GOOGLE> ---------------------------------------------------------------------------------
#define GOOGLE_SMTPS "smtps://smtp.gmail.com:465"
#define GOOGLE_SMTP "smtp://smtp.gmail.com:587"
#define GOOGLE_IMAPS "imaps://imap.gmail.com"

// https://developers.google.com/identity/protocols/googlescopes#gmailv1
#define GOOGLE_AUTH_URL "https://accounts.google.com/o/oauth2/v2/auth"
#define GOOGLE_EXCHANGE_URL "https://www.googleapis.com/oauth2/v4/token"
#define GOOGLE_MAIL_SCOPE "https://mail.google.com%20email"
#define GOOGLE_TOKEN_CHECK_URL "https://www.googleapis.com/oauth2/v1/tokeninfo?access_token="


// </GOOGLE> --------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif