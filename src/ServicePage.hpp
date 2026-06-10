#ifndef SERVICE_PAGE
#define SERVICE_PAGE

namespace bsk
{

const char HEADER[] = R"rawliteral(
HTTP/1.1 200 OK
Content-Type: text/html
Connection: close
)rawliteral";

const char PAGE[] = R"rawliteral(
<html>
<body>
<form action="/save">
<label>SSID</label>
<input name="ssid">
<label>PASSWORD</label>
<input name="password">
<button>Save</button>
</form>
</body>
</html>
)rawliteral";

const char PAGE_OK[] = R"rawliteral(
<html>
<body>
<p>Saved...<p>
</body>
</html>
)rawliteral";

const char PAGE_NOK[] = R"rawliteral(
<html>
<body>
<p>Failure...<p>
</body>
</html>
)rawliteral";

} // bsk namespace

#endif