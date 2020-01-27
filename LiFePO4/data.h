#ifndef DATA_H
#define DATA_H


// TODO: alle Bytes invertieren, damit das nicht mehr in setup() gemacht werden muss
char wifiXBM[] = {
  0xFF, 0x1F, 0xFF, 0x1F, 0x0F, 0x1E, 0xF3, 0x19, 0xFD, 0x17, 0x0E, 0x0E, 
  0xF7, 0x1D, 0xFB, 0x1B, 0x1F, 0x1F, 0xEF, 0x1E, 0xFF, 0x1F, 0xBF, 0x1F, 
  0xFF, 0x1F, };

static const char indexHTML[] = {"\
<!DOCTYPE html>\
<html>\
 <head>\
  <title>Campy</title>\
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\
 </head>\
 <body>\
 <script type=\"text/javascript\">\
 function reloadImage(pThis) {\
  pThis.onerror=null;\
  pThis.src=pThis.src;\
 }\
 </script>\
  <h1 style=\"text-align: center;\">Campy</h1>\
  <TABLE BORDER=1 CELLPADDING=3 CELLSPACING=0>\
   <TR>\
    <TD NOWRAP><FONT COLOR=green>Akku:</FONT> %d %%</TD>\
    <TD NOWRAP><FONT COLOR=red>Temperatur:</FONT> %d Grad</TD>\
    <TD NOWRAP><FONT COLOR=blue>Feuchtigkeit:</FONT> %d %%</TD>\
    <TD NOWRAP>Uptime: %d Stunden</TD>\
   </TR>\
   <TR>\
    <TD><A HREF=\"/woff.html\">WLAN aus</A></TD>\
    <TD><A HREF=\"/tdsp.html\">%s</A></TD>\
    <TD></TD>\
    <TD><A HREF=\"/askespoff.html\">ESP aus</A></TD>\
   </TR>\
  </TABLE><BR>\
  1/6 h<BR>\
  <IMG SRC=\"/graph.svg\" onerror=\"setTimeout(reloadImage(this),500);\"><BR>\
  1/7 d<BR>\
  <IMG SRC=\"/graphL.svg\" onerror=\"setTimeout(reloadImage(this),1300);\"><BR>\
  1/6 m<BR>\
  <IMG SRC=\"/graphXL.svg\" onerror=\"setTimeout(reloadImage(this),2700);\"><BR>\
 </body>\
</html>\0"};

static const char tdspHTML[] = {"\
<!DOCTYPE html>\
<html>\
 <head>\
  <title>Campy</title>\
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\
  <meta name=viewport content=\"width=device-width, initial-scale=1\">\
 </head>\
 <body><A HREF=\"/\">OK</A></body>\
</html>\0"};

static const char woffHTML[] = {"\
<!DOCTYPE html>\
<html>\
 <head>\
  <title>Campy</title>\
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\
  <meta name=viewport content=\"width=device-width, initial-scale=1\">\
 </head>\
 <body>WLAN ist jetzt aus</body>\
</html>\0"};

static const char askespoffHTML[] = {"\
<!DOCTYPE html>\
<html>\
 <head>\
  <title>Campy</title>\
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\
  <meta name=viewport content=\"width=device-width, initial-scale=1\">\
 </head>\
 <body>Wirklich komplett auschalten? <A HREF=\"/\">NEIN, nicht ausschalten!</A> - - - <A HREF=\"espoff.html\">Ja, aus</A></body>\
</html>\0"};


static const char espoffHTML[] = {"\
<!DOCTYPE html>\
<html>\
 <head>\
  <title>Campy</title>\
  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\
  <meta name=viewport content=\"width=device-width, initial-scale=1\">\
 </head>\
 <body>ESP ist jetzt aus</body>\
</html>\0"};

static const char graphSVG1[] ={"\
<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1000\" height=\"555\">\
<rect width=\"1000\" height=\"555\" fill=\"rgb(240, 240, 250)\" stroke-width=\"0\" />\
<g stroke=\"black\">\
<rect x=\"100\" y=\"9\" width=\"890\" height=\"516\" fill=\"none\"/>\
<text x=\"5\" y=\"20\">100%</text>\
<text x=\"5\" y=\"271\">10 G./50%</text>\
<text x=\"5\" y=\"529\">0%</text>\
</g>\
<g stroke=\"rgb(190,190,190)\">\
<polyline points=\"126,10, 126,524\"/>\
<polyline points=\"198,10, 198,524\"/>\
<polyline points=\"270,10, 270,524\"/>\
<polyline points=\"342,10, 342,524\"/>\
<polyline points=\"414,10, 414,524\"/>\
<polyline points=\"486,10, 486,524\"/>\
<polyline points=\"558,10, 558,524\"/>\
<polyline points=\"630,10, 630,524\"/>\
<polyline points=\"702,10, 702,524\"/>\
<polyline points=\"774,10, 774,524\"/>\
<polyline points=\"846,10, 846,524\"/>\
<polyline points=\"918,10, 918,524\"/>\
</g>\
<g stroke=\"rgb(250,190,190)\">\
<polyline points=\"101,66, 990,66\"/>\
<polyline points=\"101,146, 990,146\"/>\
<polyline points=\"101,186, 990,186\"/>\
<polyline points=\"101,226, 990,226\"/>\
<polyline points=\"101,266, 990,266\"/>\
<polyline points=\"101,306, 990,306\"/>\
<polyline points=\"101,346, 990,346\"/>\
<polyline points=\"101,386, 990,386\"/>\
<polyline points=\"101,466, 990,466\"/>\
</g>\
<g stroke=\"rgb(0,210,0)\">\
<text x=\"100\" y=\"545\">Akku</text>\
<polyline fill=\"none\" points=\"\0"};

static const char graphSVG1L[] ={"\
<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1000\" height=\"555\">\
<rect width=\"1000\" height=\"555\" fill=\"rgb(240, 240, 250)\" stroke-width=\"0\" />\
<g stroke=\"black\">\
<rect x=\"100\" y=\"9\" width=\"890\" height=\"516\" fill=\"none\"/>\
<text x=\"5\" y=\"20\">100%</text>\
<text x=\"5\" y=\"271\">10 Grad/50%</text>\
<text x=\"5\" y=\"529\">0%</text>\
</g>\
<g stroke=\"rgb(190,190,190)\">\
<polyline points=\"150,10, 150,524\"/>\
<polyline points=\"318,10, 318,524\"/>\
<polyline points=\"486,10, 486,524\"/>\
<polyline points=\"654,10, 654,524\"/>\
<polyline points=\"822,10, 822,524\"/>\
</g>\
<g stroke=\"rgb(250,190,190)\">\
<polyline points=\"101,66, 990,66\"/>\
<polyline points=\"101,146, 990,146\"/>\
<polyline points=\"101,186, 990,186\"/>\
<polyline points=\"101,226, 990,226\"/>\
<polyline points=\"101,266, 990,266\"/>\
<polyline points=\"101,306, 990,306\"/>\
<polyline points=\"101,346, 990,346\"/>\
<polyline points=\"101,386, 990,386\"/>\
<polyline points=\"101,466, 990,466\"/>\
</g>\
<g stroke=\"rgb(0,210,0)\">\
<text x=\"100\" y=\"545\">Akku</text>\
<polyline fill=\"none\" points=\"\0"};

static const char graphSVG1XL[] ={"\
<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1000\" height=\"555\">\
<rect width=\"1000\" height=\"555\" fill=\"rgb(240, 240, 250)\" stroke-width=\"0\" />\
<g stroke=\"black\">\
<rect x=\"100\" y=\"9\" width=\"890\" height=\"516\" fill=\"none\"/>\
<text x=\"5\" y=\"20\">100%</text>\
<text x=\"5\" y=\"271\">10 Grad/50%</text>\
<text x=\"5\" y=\"529\">0%</text>\
</g>\
<g stroke=\"rgb(190,190,190)\">\
<polyline points=\"262,10, 262,524\"/>\
<polyline points=\"444,10, 444,524\"/>\
<polyline points=\"626,10, 626,524\"/>\
<polyline points=\"808,10, 808,524\"/>\
</g>\
<g stroke=\"rgb(250,190,190)\">\
<polyline points=\"101,66, 990,66\"/>\
<polyline points=\"101,146, 990,146\"/>\
<polyline points=\"101,186, 990,186\"/>\
<polyline points=\"101,226, 990,226\"/>\
<polyline points=\"101,266, 990,266\"/>\
<polyline points=\"101,306, 990,306\"/>\
<polyline points=\"101,346, 990,346\"/>\
<polyline points=\"101,386, 990,386\"/>\
<polyline points=\"101,466, 990,466\"/>\
</g>\
<g stroke=\"rgb(0,210,0)\">\
<text x=\"100\" y=\"545\">Akku</text>\
<polyline fill=\"none\" points=\"\0"};

static const char graphSVG2[] ={"\
\"/>\
</g>\
<g stroke=\"rgb(210,0,0)\">\
<text x=\"5\" y=\"71\">60 Grad</text>\
<text x=\"5\" y=\"151\">40 Grad</text>\
<text x=\"5\" y=\"191\">30 Grad</text>\
<text x=\"5\" y=\"231\">20 Grad</text>\
<text x=\"5\" y=\"311\">0 Grad</text>\
<text x=\"5\" y=\"351\">-10 Grad</text>\
<text x=\"5\" y=\"391\">-20 Grad</text>\
<text x=\"5\" y=\"471\">-40 Grad</text>\
<text x=\"160\" y=\"545\">Temperatur</text>\
<polyline fill=\"none\" points=\"\0"};

static const char graphSVG3[] ={"\
\"/>\
</g>\
<g stroke=\"rgb(0,0,210)\">\
<text x=\"260\" y=\"545\">Feuchtigkeit</text>\
<polyline fill=\"none\" points=\"\0"};

static const char graphSVG4[] ={"\
\"/>\
</g></svg>\0"};

// https://github.com/zhouhan0126/WebServer-esp32/blob/master/examples/AdvancedWebServer/AdvancedWebServer.ino


#endif //DATA_H
