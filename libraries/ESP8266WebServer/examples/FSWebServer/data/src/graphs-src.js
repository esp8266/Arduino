/*
  FSWebServer - Sample Graphing Library

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WebServer library for Arduino environment.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
function ce(t){ return document.createElement(t); }
function ac(a,b){ a.appendChild(b); }

function createGraph(el, t, w, h, l, m, ib, tc, cb){
  if(typeof el === "undefined" || !el) return null;
  if(typeof ib === "undefined" || !ib) ib = false;
  w = (w>360)?w:360;
  h = (h<100)?100:h;
  var vs = new Array();
  var bgc = "black";
  var lc = (typeof tc === "undefined" && tc)?"yellow":tc;
  var bc = "#444";
  var vcb = (typeof cb === "undefined" && cb)?null:cb;
  var minV = m;
  var maxV = 0;
  var sv = 1;
  var ov = l;
  var gh = h+20;
  
  el.style.display = "block";
  el.style.backgroundColor = "#eee";
  el.style.border = "1px solid "+bc;
  el.style.padding = "5px";
  el.style.margin = "5px";
  el.style.width = (w+2)+"px";
  
  var g = ce("canvas");
  g.style.border = "1px solid "+bc;
  g.style.marginTop = "5px";
  g.width = w;
  g.height = gh;
  
  var c = g.getContext("2d");
  
  var sl = ce("span");
  sl.innerText = "Scale: ";
  sl.style.paddingLeft = "5px";
  
  var st = ce("input");
  st.type = "text";
  st.size = 3;
  st.value = sv;
  st.onchange = function(e){ sv = st.value; };
  
  var sm = ce("button");
  sm.innerText = "-";
  sm.onclick = function(e){ if(sv > 1) sv--; st.value = sv; };
  
  var sp = ce("button");
  sp.innerText = "+";
  sp.onclick = function(e){ sv++; st.value = parseInt(sv); };
  
  var ol = ce("span");
  ol.innerText = "Offset: ";
  ol.style.paddingLeft = "5px";
  
  var ot = ce("input");
  ot.type = "text";
  ot.size = 5;
  ot.value = ov;
  ot.onchange = function(e){ ov = parseInt(ot.value); };
  
  var om = ce("button");
  om.innerText = "-";
  om.onclick = function(e){ if(ov > l) ov--; ot.value = ov; };
  
  var op = ce("button");
  op.innerText = "+";
  op.onclick = function(e){ if(ov < m) ov++; ot.value = ov; };
  
  ac(el,sl);
  ac(el,sm);
  ac(el,st);
  ac(el,sp);
  ac(el,ol);
  ac(el,om);
  ac(el,ot);
  ac(el,op);
  ac(el,ce("br"));
  ac(el,g);
  
  function scv(v){
    if(v <= ov) return 0;
    if(v > (((m - l) / sv) + ov)) return h;
    return Math.round((v - ov) * (h / ((m - l) / sv)));
  }
  
  g.add = function(v){
    if(vcb) v = vcb(v);
    vs.push(v);
    if(v < minV) minV = v;
    if(v > maxV) maxV = v;
    if(vs.length > w) vs.shift();
    c.canvas.width = w;
    c.lineWidth = 1;
    c.shadowBlur=2;
    c.shadowColor="black";
    
    c.fillStyle = bc;
    c.fillRect(0, 0, w, 20);
    c.fillStyle = lc;
    c.font = "14px Verdana";
    var header = t+": "+(vs[vs.length - 1])+" | Min: "+minV+" | Max: "+maxV;
    c.fillText(header,5,15);
    
    c.fillStyle = bgc;
    c.fillRect(0, 20, w, gh);
    c.strokeStyle = lc;
    
    for(var i = 0; i < vs.length; i++){
      if(ib){
        c.moveTo(i, gh);
        c.lineTo(i, gh - scv(vs[i]));
        c.stroke();
        continue;
      }
      if(i === 0){
        c.moveTo(0, gh - scv(vs[0]));
        c.lineTo(0, gh - scv(vs[0]));
        c.stroke();
      } else {
        c.moveTo((i - 1), gh - scv(vs[i - 1]));
        c.lineTo(i, gh - scv(vs[i]));
        c.stroke();
      }
    }
    c.fillStyle = lc;
    c.font = "10px Verdana";
    c.fillText(ov, 2, gh - 5);
    c.fillText(Math.round((((m - l) / sv) + ov) * 10) / 10, 2, 30);
  };
  return g;
}

function createDigiGraph(el, t, w, h, m, tc){
  if(typeof el === "undefined" || !el) return null;
  w = (w>360)?w:360;
  h = (h<20)?20:h;
  var vs = new Array();
  var bgc = "black";
  var lc = (typeof tc === "undefined" && tc)?"pink":tc;
  var bc = "#444";
  var sv = 1;
  var gh = h+20;
  
  el.style.display = "block";
  el.style.backgroundColor = "#eee";
  el.style.border = "1px solid "+bc;
  el.style.padding = "5px";
  el.style.margin = "5px";
  el.style.width = (w+2)+"px";
  
  var g = ce("canvas");
  g.style.border = "1px solid "+bc;
  g.style.marginTop = "5px";
  g.width = w;
  g.height = gh;
  
  var c = g.getContext("2d");
  
  var sl = ce("span");
  sl.innerText = "Scale: ";
  sl.style.paddingLeft = "5px";
  
  var st = ce("input");
  st.type = "text";
  st.size = 3;
  st.value = sv;
  st.onchange = function(e){ sv = st.value; };
  
  var sm = ce("button");
  sm.innerText = "-";
  sm.onclick = function(e){ if(sv > 1) sv--; st.value = sv; };
  
  var sp = ce("button");
  sp.innerText = "+";
  sp.onclick = function(e){ sv++; st.value = parseInt(sv); };
  
  ac(el,sl);
  ac(el,sm);
  ac(el,st);
  ac(el,sp);
  ac(el,ce("br"));
  ac(el,g);
  
  function gpos(p){
    var each = Math.floor(((h - ((m.length - 1)*2))/m.length));
    var s = (p * 2) + (p * each);
    return [s, s + each];
  }
  
  function gact(i,p){
    return ((vs[i] & (1 << m[p])) !== 0);;
  }
  
  var vc = ["aqua","yellow","#FF4500","#00FF7F","orange","cyan","magenta","#7FFF00","#FAF0E6","#00CED1","#FFD700","#EE82EE","#00FF00","#00BFFF","#FF4500","#EEE8AA","#FF1493"];
      
  g.add = function(v){
    vs.push(v);
    if(vs.length > w) vs.shift();
    c.canvas.width = w;
    c.lineWidth=1;
    c.shadowBlur=2;
    c.shadowColor="black";
    
    c.fillStyle = bc;
    c.fillRect(0, 0, w, 20);
    c.fillStyle = lc;
    c.font = "14px Verdana";
    c.fillText(t,5,15);
    
    c.fillStyle = bgc;
    c.fillRect(0, 20, w, gh);
    
    c.strokeStyle = bc;
    c.lineWidth = 1;
    c.shadowBlur=0;
    c.shadowColor="";
    for(var p=0;p<m.length;p++){
      var pos = gpos(p);
      c.moveTo(0, 20 + pos[1] + 1);
      c.lineTo(w, 20 + pos[1] + 1);
      c.stroke();
    }
    
    c.shadowBlur=2;
    c.font = "14px Verdana";
    var pn = Math.round(w / sv);
    var stv = w - pn - (w - vs.length);
    if(stv < 0) stv = 0;
    for(var i = stv; i < vs.length; i++){
      for(var p=0;p<m.length;p++){
        var pos = gpos(p);
        var act = gact(i, p); 
        c.fillStyle = vc[p];
        c.shadowColor="black";
        c.font = "12px Verdana";
        c.fillText(""+m[p], 2, pos[0] + 32);
        c.shadowColor=vc[p];
        var x1 = ((i-stv)*sv) - (sv / 2);
        var wa = gact(i - 1, p);
        if(i && act != wa){
          if(!wa){
            c.fillRect(x1, 20 + (pos[1] - 2), sv/2, 2);
            c.fillRect(x1+(sv/2), 20 + pos[0], 2, pos[1] - pos[0]);
            c.fillRect(x1+(sv/2), 20 + pos[0], sv/2, 2);
          } else {
            c.fillRect(x1, 20 + pos[0], sv/2, 2);
            c.fillRect(x1+(sv/2), 20 + pos[0], 2, pos[1] - pos[0]);
            c.fillRect(x1+(sv/2), 20 + (pos[1] - 2), sv/2, 2);
          }
        } else if(act){
          c.fillRect(x1, 20 + pos[0], sv, 2);
        } else {
          c.fillRect(x1, 20 + (pos[1] - 2), sv, 2);
        }
      }
    }
  };
  return g;
}
