
#if LWIP_FEATURES && !LWIP_IPV6

// Substitution list:
//   {t} - target name
//   {1} - The target to redirect to, in absolute URL form.
#ifdef DEBUG_VIEW
static const char portalRedirectHTML[] PROGMEM = R"EOF(
<!DOCTYPE html>
<html lang='en'>
<head>
<meta name='viewport' content='width=device-width'>
<meta http-equiv='Refresh' content='5; url={1}'>
<title>Redirecting</title>
</head>
<body>
<h1>Captive Portal Redirect</h1>
<p>Redirecting to <a href='{1}'>{t}</a></p>
<p>If you do not see the menu in 5 seconds, please click on the above link!</p>
</body>
</html>
)EOF";

#else
static const char portalRedirectHTML[] PROGMEM = R"EOF(
<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width'><meta http-equiv='Refresh' content='5; url={1}'><title>Redirecting</title></head><body><h1>Captive Portal Redirect</h1><p>Redirecting to <a href='{1}'>{t}</a></p><p>If you do not see the menu in 5 seconds, please click on the above link!</p></body></html>
)EOF";
#endif

void sendPortalRedirect(String path, String targetName) {
  CONSOLE_PRINTLN2("Request redirected to captive portal, original request was for ", server.hostHeader());
  /* There are 3 points of redirection here:
       1) "Location" element in the header
       2) HTML meta element to redirect
       3) Click on link to redirect
    If the "Location" header element works the HTML stuff is never seen.
  */
  // https://tools.ietf.org/html/rfc7231#section-6.4.3
  server.sendHeader("Location", path, true);
  addNoCacheHeader();
  String reply = FPSTR(portalRedirectHTML);
  reply.reserve(reply.length() + 2 * path.length() + 80);
  reply.replace("{t}", targetName);
  reply.replace("{1}", path);
  server.send(302, "text/html", reply);
}

#endif // LWIP_FEATURES && !LWIP_IPV6
