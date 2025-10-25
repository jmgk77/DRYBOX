#pragma once

#define FORM_SAVE_STRING(VAR)                                  \
  strncpy(VAR,                                                 \
          request->hasParam(#VAR, true)                        \
              ? request->getParam(#VAR, true)->value().c_str() \
              : "",                                            \
          sizeof(VAR));
#define FORM_SAVE_STRING2(VAR)                                                 \
  VAR = request->hasParam(#VAR, true) ? request->getParam(#VAR, true)->value() \
                                      : "";
#define FORM_SAVE_INT(VAR)                                   \
  VAR = request->hasParam(#VAR, true)                        \
            ? request->getParam(#VAR, true)->value().toInt() \
            : 0;
#define FORM_SAVE_BOOL(VAR)                                                   \
  VAR = request->hasParam(#VAR, true)                                         \
            ? (request->getParam(#VAR, true)->value() == "on" ? true : false) \
            : false;

#define FORM_START(URL) \
  s += "<form action='" + String(URL) + "' method='POST'>\n";
#define FORM_ASK_VALUE(VAR, TXT)                                           \
  s += "<label for='" + String(#VAR) + "' name='" + String(#VAR) + "'>" +  \
       String(TXT) + ":</label><input type='text' name='" + String(#VAR) + \
       "' value='" + VAR + "'><br>\n";
#define FORM_ASK_BOOL(VAR, TXT)                                                \
  s += "<label for='" + String(#VAR) + "' name='" + String(#VAR) + "'>" +      \
       String(TXT) + ":</label><input type='checkbox' name='" + String(#VAR) + \
       "' " + String(VAR ? "checked" : "") + "><br>\n";
#define FORM_ASK_BOOL_JS(VAR, TXT, JS)                                         \
  s += "<label for='" + String(#VAR) + "' name='" + String(#VAR) + "'>" +      \
       String(TXT) + ":</label><input type='checkbox' name='" + String(#VAR) + \
       "' " + String(VAR ? "checked " : "") + String(JS) + "><br>\n";
#define FORM_END(BTN)                                                          \
  s +=                                                                         \
      "<input type='hidden' name='s' value='1'><input type='submit' value='" + \
      String(BTN) + "'></form>\n";

const char html_header[] = R""""(
<!DOCTYPE html>
<html lang='pt-br'>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<meta http-equiv='cache-control' content='no-cache, no-store, must-revalidate'>
<meta http-equiv='refresh' content='600'/>
<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>
<title>DRYBOX</title>
</head>
<body>
)"""";

const char html_js[] = R""""(
var canvas = document.getElementById('c');
var ctx = canvas.getContext('2d');
var myChart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: l,
    datasets: [{
      label: 'Temperature',
      data: t,
      borderColor: 'rgb(255, 0, 0)',
      backgroundColor: 'rgb(255, 0, 0, 0.1)',
      tension: 0.1,
    }, {
      label: 'Humidity',
      data: h,
      borderColor: 'rgb(0, 0, 255)',
      backgroundColor: 'rgb(0, 0, 255, 0.1)',
      tension: 0.1,
    }]
  },
});
</script>
<div style="border: 1px solid black">
<canvas id="c" width="718" height="239" style="display: block; box-sizing: border-box; height: 239px; width: 718px;"></canvas>
</div>
)"""";

const char html_footer[] = R""""(
</body>
</html>)"""";

const char html_buttons[] = R""""(
<form action='/' method='POST'><input type='submit' value='MAIN'></form>
<form action='/info' method='POST'><input type='submit' value='INFO'></form>
<form action='/files' method='POST'><input type='submit' value='FILES'></form>
<form action='/config' method='POST'><input type='submit' value='CONFIG'></form>
<form action='/reboot' method='POST'><input type='submit' value='REBOOT'></form>
<form action='/reset' method='POST'><input type='submit' value='RESET'></form>)"""";

const char html_commands[] = R""""(
<form action='/command?status' method='POST'><input type='submit' value='STATUS'></form>
<form action='/command?fan_off' method='POST'><input type='submit' value='FAN OFF'></form>
<form action='/command?fan_on' method='POST'><input type='submit' value='FAN ON'></form>
<form action='/command?heater_off' method='POST'><input type='submit' value='HEATER OFF'></form>
<form action='/command?heater_on' method='POST'><input type='submit' value='HEATER ON'></form>
)"""";
