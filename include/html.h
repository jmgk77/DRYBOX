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
<!DOCTYPE html><html lang='pt-br'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='cache-control' content='no-cache, no-store, must-revalidate'><meta http-equiv='refresh' content='30'/><script src='https://cdn.jsdelivr.net/npm/chart.js'></script><style>.button-link{display:inline-block;padding:6px 12px;margin-bottom:0;font-size:14px;font-weight:400;line-height:1.42857143;text-align:center;white-space:nowrap;vertical-align:middle;cursor:pointer;text-decoration:none}</style><title>DRYBOX</title></head><body>
)"""";

const char html_root[] = R""""(
<div style="border:1px solid black;padding:10px;text-align:center;margin-bottom:1em;">
  <span>Perfil: <b>%PROFILE_NAME%</b></span> | <span>Estado do Ciclo: <b>%STATE%</b></span> | <span>Tempo Restante: <b>%TIME%</b></span><br>
  <span>Aquecedor: <b>%HEATER_STATE%</b></span> | <span>Ventoinha: <b>%FAN_STATE%</b></span>
  | <span>Exaustor: <b>%VENT_STATE%</b></span>
</div>
<div style="border:1px solid black;width:98vw;height:50vh;"><canvas id="c" style="width:100%;height:100%;"></canvas></div>
<div style="text-align:center;margin-top:1em;"><button onclick="downloadCSV()">DOWNLOAD CSV</button></div>
)"""";

const char html_js[] = R""""(
function downloadCSV(){if("undefined"==typeof t||"undefined"==typeof h||t.length!==h.length)return void console.error("Data arrays are not defined or have different lengths.");let e="Temperature,Humidity\n";for(let n=0;n<t.length;n++)e+=`${t[n]},${h[n]}\n`;const o=new Blob([e],{type:"text/csv;charset=utf-8;"}),a=document.createElement("a"),l=URL.createObjectURL(o);a.href=l,a.setAttribute("download","drybox_data.csv"),document.body.appendChild(a),a.click(),document.body.removeChild(a),URL.revokeObjectURL(l)}const l=new Array(t.length).fill("");t.length>10&&(l[t.length-10]="10 min ago"),t.length>30&&(l[t.length-30]="30 min ago"),t.length>60&&(l[t.length-60]="1 hour ago"),t.length>120&&(l[t.length-120]="2 hours ago");const c=document.getElementById("c"),d=c.getContext("2d");new Chart(d,{type:"line",data:{labels:l,datasets:[{label:"Temperatura",data:t,borderColor:"rgb(255, 0, 0)",backgroundColor:"rgba(255, 0, 0, 0.1)",tension:.1},{label:"Umidade",data:h,borderColor:"rgb(0, 0, 255)",backgroundColor:"rgba(0, 0, 255, 0.1)",tension:.1}]}});</script>
)"""";

const char html_footer[] = R""""(
</body></html>
)"""";

const char html_commands[] = R""""(
<div style="border:1px solid black;padding:10px;text-align:center"><form action="/command" method="GET" style="display:inline-block;" autocomplete="off"><select name="profile" %DISABLED%>%PROFILE_OPTIONS%</select><input type="hidden" name="start_cycle" value="1"><button type="submit" %START_DISABLED%>INICIAR CICLO</button></form><a href="/command?stop_cycle" class="button-link"><button %STOP_DISABLED%>PARAR CICLO</button></a><a href="/command?fan_off" class="button-link"><button>FAN OFF</button></a><a href="/command?fan_on" class="button-link"><button>FAN ON</button></a><a href="/command?heater_off" class="button-link"><button>HEATER OFF</button></a><a href="/command?heater_on" class="button-link"><button>HEATER ON</button></a><a href="/command?vent_open" class="button-link"><button>VENT OPEN</button></a><a href="/command?vent_close" class="button-link"><button>VENT CLOSE</button></a></div>
)"""";

const char html_buttons[] = R""""(
<div style="border:1px solid black;padding:10px;text-align:center"><a href="/" class="button-link"><button>MAIN</button></a><a href="/info" class="button-link"><button>INFO</button></a><a href="/files" class="button-link"><button>FILES</button></a><a href="/config" class="button-link"><button>CONFIG</button></a><a href="/reboot" class="button-link"><button>REBOOT</button></a><a href="/reset" class="button-link"><button>RESET</button></a></div>
)"""";
