#ifndef WIFIHTTP_H_
#define WIFIHTTP_H_


// #define DEBUG_VIEW
// The idea here is to debug HTML with DEBUG_VIEW defined then, when finished,
// use one of the minify web applications to strip the comments and nice
// formating spaces. Then update the minified version below.
//
// Also there are comment sections at the top and bottom of each block of HTML
// code. The purpose is to move the lines of C code near by into the blocked
// comment sections. Then you have a large block of continguious HTML that can
// be copy/pasted into one of the online web HTML checkers. You can adjust the
// code there till it is correct then copy/paste back here. Then, you can move
// comment boarders around until the C code is back in place.

#ifdef DEBUG_VIEW
static const char configHead[] PROGMEM = R"EOF(
<!--

Captive Portal html for WiFi Config

-->

<!DOCTYPE html>
<html lang="en">
<head>
<meta name='viewport' content='width=device-width'>
<title>WiFi</title>
<style>

html, body {
  width: 100%;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
  font-size: 16px;
}

h1, h2, h3, h4 {
  vertical-align:middle;
  text-align:center;
}

input[type=submit] {
  width: 98%;
  font-family: 'Open Sans', Helvetica, Arial, sans-serif;
  height: 3em;
}

a {
  text-decoration: none;
  color: blue;
}

.mono, td, input {
  font-family: 'Source Code Pro', monospace;
  height: 1.5em;
  font-size: 1em;
  vertical-align: middle;
}

.lg {
  font-size: 1em;
  height: 1.8em;
}
/*
*/
</style>
)EOF";
#else
static const char configHead[] PROGMEM = R"EOF(<!DOCTYPE html><html lang="en"><head><meta name='viewport' content='width=device-width'><title>WiFi</title><style>html,body{width:100%;font-family:'Open Sans',Helvetica,Arial,sans-serif;font-size:16px}h1,h2,h3,h4{vertical-align:middle;text-align:center}input[type=submit]{width:98%;font-family:'Open Sans',Helvetica,Arial,sans-serif;height:3em}a{text-decoration:none;color:blue}.mono,td,input{font-family:'Source Code Pro',monospace;height:1.5em;font-size:1em;vertical-align:middle}.lg{font-size:1em;height:1.8em}</style>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configHead2[] PROGMEM = R"EOF(
<style>
/*

*/
.left {
  text-align: left;
}

.center {
  text-align: center;
}

.right {
  text-align: right;
}

@media screen and (orientation: portrait) {
  #inner {
    display: block;
    margin: 0 auto;
    max-width: 400px;
    min-width: 350px;
    text-align: left;
  }

  #outer {
    width:100%
  }
}

@media screen and (orientation: landscape) {
  #inner {
    display: block;
    margin: 0 auto;
    width: 400px;
    text-align: left;
  }

  #outer {
    width:100%
  }
}

</style>
</head>

<body onload='ol()'>
<script>

// short hand
function ge(i) {
  return document.getElementById(i);
}

// Toggle password view
function pv() {
  let e = ge('p');
  if (e.type === 'password') {
    e.type = 'text';
  } else {
    e.type = 'password';
  }
  e.focus();
}

// Copy SSID value to input element for SSID
// Move focus to password input element and clear
function cs(i){
  ge('s').value = i.innerText;
  let e = ge('p');
  e.type = 'password';
  e.value = '';
  e.focus();
  e.scrollIntoView();
}
</script>
<!--
-->
)EOF";
#else
static const char configHead2[] PROGMEM = R"EOF(<style>.left{text-align:left}.center{text-align:center}.right{text-align:right}@media screen and (orientation: portrait){#inner{display:block;margin:0 auto;max-width:400px;min-width:350px;text-align:left}#outer{width:100%}}@media screen and (orientation: landscape){#inner{display:block;margin:0 auto;width:400px;text-align:left}#outer{width:100%}}</style></head><body onload='ol()'> <script>function ge(i){return document.getElementById(i);} function pv(){let e=ge('p');if(e.type==='password'){e.type='text';}else{e.type='password';} e.focus();} function cs(i){ge('s').value=i.innerText;let e=ge('p');e.type='password';e.value='';e.focus();e.scrollIntoView();}</script>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configPresetInput[] PROGMEM = R"EOF(
<!--
Prefill input fields with the last credentials used.
{s} - Network Name/SSID
{p} - Password
-->
<script>
// After web page load init SSID/PSK field
// onload copy credentials
function ol() {
  ge('s').value='{s}';
  ge('p').value='{p}';
}
</script>
<!--
-->
)EOF";
#else
static const char configPresetInput[] PROGMEM = R"EOF(<script>function ol(){ge('s').value='{s}';ge('p').value='{p}';}</script>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configConnection[] PROGMEM = R"EOF(
<!--
{w} - SoftAP info "SoftAP: softAP_ssid"
or  - WiFi Network connection info "WiFi Network: ssid"
-->
<div id='outer'>
<div id='inner'>
<h1>WiFi Details and Config</h1>
<p>You are connected through the {w}</p>
<!--
-->
)EOF";
#else
static const char configConnection[] PROGMEM = R"EOF(<div id='outer'><div id='inner'><h1>WiFi Details and Config</h1><p>You are connected through the {w}</p>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configInfo[] PROGMEM = R"EOF(
<!--
{s} - SSDI of Network
{i} - IP address on that Network, WiFi.softAPIP or WiFi.localIP
{a} - Number of Stations connected to AP
-->
<br />
<h2>SoftAP Details</h2>
<table>
<tr><td>SSI</td><td>{s}</td></tr>
<tr><td>BSSID&nbsp;</td><td>{b}</td></tr>
<tr><td>IP</td><td>{i}</td></tr>
<tr><td>STA</td><td>Count: {a}</td></tr>
</table>
<!--
-->
)EOF";
#else
static const char configInfo[] PROGMEM = R"EOF(<br /><h2>SoftAP Details</h2><table><tr><td>SSI</td><td>{s}</td></tr><tr><td>BSSID&nbsp;</td><td>{b}</td></tr><tr><td>IP</td><td>{i}</td></tr><tr><td>STA</td><td>Count: {a}</td></tr></table>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configInfo2[] PROGMEM = R"EOF(
<!--
{s} - SSDI of Network
{b} - BSSID
{c} - Channel
{p} - PHY Mode
{r} - RSSI
{i} - IP address on that Network, WiFi.softAPIP or WiFi.localIP
{m} - Subnet Mask
{g} - Gateway
{1} - DNS1
{2} - DNS2
-->
<br />
<h2>WLAN Details</h2>
<table>
<tr><td>SSID</td><td>{s}</td></tr>
<tr><td>BSSID&nbsp;</td><td>{b}</td></tr>
<tr><td>CH</td><td>{c}</td></tr>
<tr><td>PHY</td><td>{p}</td></tr>
<tr><td>RSSI</td><td>{r}</td></tr>
<tr><td>IP</td><td>{i}</td></tr>
<tr><td>GW</td><td>{g}</td></tr>
<tr><td>Mask</td><td>{m}</td></tr>
<tr><td>DNS1</td><td>{1}</td></tr>
<tr><td>DNS2</td><td>{2}</td></tr>
</table>
<!--
-->
)EOF";
#else
static const char configInfo2[] PROGMEM = R"EOF(<br /><h2>WLAN Details</h2><table><tr><td>SSID</td><td>{s}</td></tr><tr><td>BSSID&nbsp;</td><td>{b}</td></tr><tr><td>CH</td><td>{c}</td></tr><tr><td>PHY</td><td>{p}</td></tr><tr><td>RSSI</td><td>{r}</td></tr><tr><td>IP</td><td>{i}</td></tr><tr><td>GW</td><td>{g}</td></tr><tr><td>Mask</td><td>{m}</td></tr><tr><td>DNS1</td><td>{1}</td></tr><tr><td>DNS2</td><td>{2}</td></tr></table>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configList[] PROGMEM = R"EOF(
<!--
-->
<br />
<h2>WLAN Network List</h2>
<h4>(refresh if any are missing)</h4>
<table>
<tr><th class='left'>Network Name/SSID</th>
    <th style='width: 2em;' class='center'>CH</th>
    <th style='width: 1em;' class='center'></th>
    <th class='right'>RSSI</th></tr>
<!--
-->
)EOF";
#else
static const char configList[] PROGMEM = R"EOF(<br /><h2>WLAN Network List</h2><h4>(refresh if any are missing)</h4><table><tr><th class='left'>Network Name/SSID</th><th style='width: 2em;' class='center'>CH</th><th style='width: 1em;' class='center'></th><th class='right'>RSSI</th></tr>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configItem[] PROGMEM = R"EOF(
<!--
use this with each AP when APs are found in a scan
{s} - WiFi.SSID
{c} - channel
{l} - For a secure Network replace with &#x1f512; (lock symbol)
{r} - WiFi.RSSI
-->
<tr><td><a class='mono' onclick='cs(this)' title='{t}'>{s}</a></td>
    <td class='center'>{c}</td>
    <td class='center'>{l}</td>
    <td class='right'>{r}</td></tr>
<!--
-->
)EOF";
#else
static const char configItem[] PROGMEM = R"EOF(<tr><td><a class='mono' onclick='cs(this)' title='{t}'>{s}</a></td><td class='center'>{c}</td><td class='center'>{l}</td><td class='right'>{r}</td></tr>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configNoAPs[] PROGMEM = R"EOF(
<!--
or this when no APs are found
-->
<tr><td>No WLAN found</td>
    <td></td>
    <td></td>
    <td></td></tr>
<!--
-->
)EOF";
#else
static const char configNoAPs[] PROGMEM = R"EOF(<tr><td>No WLAN found</td><td></td><td></td><td></td></tr>)EOF";
#endif

#ifdef DEBUG_VIEW
static const char configEnd[] PROGMEM = R"EOF(
<!--
-->
</table>
<br /><form method='POST' action='wifisave'><h4>Connect to Network:</h4>
<input id='s' class='lg' type='text' size=32 maxlength=32 placeholder='Network Name/SSID' name='n' spellcheck='false' data-gramm_editor='false'/>
<br /><br />
<input id='p' class='lg' type='password' size=32 maxlength=64 placeholder='password' name='p' spellcheck='false' data-gramm_editor='false'/>
&nbsp;&nbsp;<a class='lg' onclick='pv();'>&#x1f441;</a>
<br /><br /><input type='submit' value='Connect/Disconnect'/>
</form>
<br />
<p>You may want to <a href='/'>return to the home page</a>.</p>
<p></p>
</div>
</div>
</body>
</html>
<!--
-->
)EOF";
#else
static const char configEnd[] PROGMEM = R"EOF(</table> <br /><form method='POST' action='wifisave'><h4>Connect to Network:</h4> <input id='s' class='lg' type='text' size=32 maxlength=32 placeholder='Network Name/SSID' name='n' spellcheck='false' data-gramm_editor='false'/> <br /><br /> <input id='p' class='lg' type='password' size=32 maxlength=64 placeholder='password' name='p' spellcheck='false' data-gramm_editor='false'/> &nbsp;&nbsp;<a class='lg' onclick='pv();'>&#x1f441;</a> <br /><br /><input type='submit' value='Connect/Disconnect'/></form> <br /><p>You may want to <a href='/'>return to the home page</a>.</p><p></p></div></div></body></html>)EOF";
#endif

#endif
