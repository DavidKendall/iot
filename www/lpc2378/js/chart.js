var acc_x = new TimeSeries();
var acc_y = new TimeSeries();
var acc_z = new TimeSeries();

var acc_x_wii = new TimeSeries();
var acc_y_wii = new TimeSeries();
var acc_z_wii = new TimeSeries();

var light = new TimeSeries();
var light1 = new TimeSeries();
var press = new TimeSeries();
var press1 = new TimeSeries();
var mic = new TimeSeries();
var mic1 = new TimeSeries();
var temp = new TimeSeries();
var temp1 = new TimeSeries();

var data_acc_x = 0;
var data_acc_y = 0;
var data_acc_z = 0;

var data_acc_x_wii = 0;
var data_acc_y_wii = 0;
var data_acc_z_wii = 0;

var data_light = 0;
var data_light1 = 0;
var data_press = 0;
var data_press1 = 0;
var data_temp = 0;
var data_temp1 = 0;
var data_mic = 0;
var data_mic1 = 0;

var chart_acc;
var chart_wii;
var chart_env;
var chart_env1;
var chart_mic;

function createTimeline() 
{
  // old min : 300, old max : 900
  chart_acc = new SmoothieChart({strokeStyle: 'rgb(125, 0, 0)', fillStyle: 'rgb(60, 0, 0)', lineWidth: 1, maxValue:120, minValue: -70, millisPerLine: 250, verticalSections: 6 });
  chart_wii = new SmoothieChart({strokeStyle: 'rgb(125, 0, 0)', fillStyle: 'rgb(60, 0, 0)', lineWidth: 1, maxValue:360, minValue:-360, millisPerLine: 250, verticalSections: 6 });
  chart_env = new SmoothieChart({strokeStyle: 'rgb(125, 0, 0)', fillStyle: 'rgb(60, 0, 0)', lineWidth: 1, maxValue:200, minValue:0, millisPerLine: 250, verticalSections: 6 });
  chart_env1 = new SmoothieChart({strokeStyle: 'rgb(125, 0, 0)', fillStyle: 'rgb(60, 0, 0)', lineWidth: 1, maxValue:200, minValue:0, millisPerLine: 250, verticalSections: 6 });
  chart_mic = new SmoothieChart({strokeStyle: 'rgb(125, 0, 0)', fillStyle: 'rgb(60, 0, 0)', lineWidth: 1, maxValue:6000, minValue:0, millisPerLine: 250, verticalSections: 6 });

  chart_acc.addTimeSeries(acc_x, { strokeStyle: 'rgba(255, 0, 0, 1)', fillStyle: 'rgba(255, 0, 0, 0.1)', lineWidth: 3 });
  chart_acc.addTimeSeries(acc_y, { strokeStyle: 'rgba(0, 255, 0, 1)', fillStyle: 'rgba(0, 255, 0, 0.1)', lineWidth: 3 });
  chart_acc.addTimeSeries(acc_z, { strokeStyle: 'rgba(0, 0, 255, 1)', fillStyle: 'rgba(0, 0, 255, 0.1)', lineWidth: 3 });

  chart_wii.addTimeSeries(acc_x_wii, { strokeStyle: 'rgba(255, 0, 0, 1)', fillStyle: 'rgba(255, 0, 0, 0.1)', lineWidth: 3 });
  chart_wii.addTimeSeries(acc_y_wii, { strokeStyle: 'rgba(0, 255, 0, 1)', fillStyle: 'rgba(0, 255, 0, 0.1)', lineWidth: 3 });
  chart_wii.addTimeSeries(acc_z_wii, { strokeStyle: 'rgba(0, 0, 255, 1)', fillStyle: 'rgba(0, 0, 255, 0.1)', lineWidth: 3 });

  chart_env.addTimeSeries(press, { strokeStyle: 'rgba(255, 0, 0, 1)', fillStyle: 'rgba(255, 0, 0, 0.1)', lineWidth: 3 });
  chart_env.addTimeSeries(temp, { strokeStyle: 'rgba(0, 255, 0, 1)', fillStyle: 'rgba(0, 255, 0, 0.1)', lineWidth: 3 });
  chart_env.addTimeSeries(light, { strokeStyle: 'rgba(255, 255, 255, 1)', fillStyle: 'rgba(255, 255, 255, 0.1)', lineWidth: 3 });
  //chart_env.addTimeSeries(mic, { strokeStyle: 'rgba(0, 0, 255, 1)', fillStyle: 'rgba(0, 0, 255, 0.1)', lineWidth: 3 });

  chart_env1.addTimeSeries(press1, { strokeStyle: 'rgba(255, 0, 0, 1)', fillStyle: 'rgba(255, 0, 0, 0.1)', lineWidth: 3 });
  chart_env1.addTimeSeries(mic1, { strokeStyle: 'rgba(0, 255, 0, 1)', fillStyle: 'rgba(0, 255, 0, 0.1)', lineWidth: 3 });
  chart_env1.addTimeSeries(temp1, { strokeStyle: 'rgba(0, 0, 255, 1)', fillStyle: 'rgba(0, 0, 255, 0.1)', lineWidth: 3 });
  chart_env1.addTimeSeries(light1, { strokeStyle: 'rgba(255, 255, 255, 1)', fillStyle: 'rgba(255, 255, 255, 0.1)', lineWidth: 3 });

  chart_mic.addTimeSeries(mic, { strokeStyle: 'rgba(255, 0, 0, 1)', fillStyle: 'rgba(255, 0, 0, 0.1)', lineWidth: 3 });

  chart_acc.streamTo(document.getElementById("chart_acc"), 200);
  chart_wii.streamTo(document.getElementById("chart_wii"), 200);
  chart_env.streamTo(document.getElementById("chart_env"), 200);
  chart_env1.streamTo(document.getElementById("chart_env1"), 200);
  //chart_mic.streamTo(document.getElementById("chart_mic"), 200);
}


