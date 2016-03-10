/*jslint browser: true*/
/*global jQuery, SmoothieChart, TimeSeries, manipulateDOM, WebSocket, JustGage, $*/

var WSN_CLIENT = (function() {
  'use strict';

  var activityAcc = 0;
  var accX = new TimeSeries();
  var accY = new TimeSeries();
  var accZ = new TimeSeries();
  var dataAccX = 0;
  var dataAccY = 0;
  var dataAccZ = 0;
  var websocket;
  var chartAcc;
  var sensorId = getSensorId();
  var pot;

  function showAcc() {
    $('#chartarea_acc').slideDown();
    document.getElementById('chart_acc').width = 300;
    document.getElementById('chart_acc').height = 200;
  }

  function showPotGauge() {
    var g1;
    g1 = new JustGage({
      id: 'potentiometer',
      value: 72,
      min: 0,
      max: 100,
      gaugeWidthScale: 0.6,
      counter: true,
      hideInnerShadow: true
    });
    return g1;
  }

  function showMain() {
    showAcc();
    showPotGauge();
  }

  function createTimeline() {
    chartAcc = new SmoothieChart({
      strokeStyle: 'rgb(125, 0, 0)',
      fillStyle: 'rgb(60, 0, 0)',
      lineWidth: 1,
      maxValue: 120,
      minValue: -120,
      millisPerLine: 250,
      verticalSections: 6,
    });
    chartAcc.addTimeSeries(accX, {
      strokeStyle: 'rgba(255, 0, 0, 1)',
      fillStyle: 'rgba(255, 0, 0, 0.1)',
      lineWidth: 3,
    });
    chartAcc.addTimeSeries(accY, {
      strokeStyle: 'rgba(0, 255, 0, 1)',
      fillStyle: 'rgba(0, 255, 0, 0.1)',
      lineWidth: 3,
    });
    chartAcc.addTimeSeries(accZ, {
      strokeStyle: 'rgba(0, 0, 255, 1)',
      fillStyle: 'rgba(0, 0, 255, 0.1)',
      lineWidth: 3,
    });
    chartAcc.streamTo(document.getElementById('chart_acc'), 200);
  }

  function isActiveAcc() {
    if (activityAcc === 0) {
      $('#chart_acc').fadeTo(10, 0.3);
    }

    activityAcc = 0;
    setTimeout(isActiveAcc, 2000);
  }

  function getSensorId() {
    var url = document.URL;
    var urlComponents = url.split('=');

    return urlComponents[1];
  }

  $(document).ready(function() {
    console.log('websocket.js running...');
    console.log('Sensor id : ' + sensorId + '\n');

    showAcc();
    createTimeline();

    pot = showPotGauge();

    websocket = new WebSocket('ws://localhost:9000/ws/sensors/rw');

    websocket.onopen = function() {
      var data = JSON.stringify({
        id: sensorId,
        type: 'SUBSCRIBE'
      });
      websocket.send(data);
    };

    websocket.onmessage = function(evt) {
      var jsonSensor = jQuery.parseJSON(evt.data.toString());
      var theDate = new Date().getTime();
      if (jsonSensor.id === sensorId) {
        activityAcc = 1;
        $('#chart_acc').fadeTo(100, 1);
        dataAccX = jsonSensor.ax;
        dataAccY = jsonSensor.ay;
        dataAccZ = jsonSensor.az;

        accX.append(theDate, dataAccX);
        accY.append(theDate, dataAccY);
        accZ.append(theDate, dataAccZ);

        pot.refresh(jsonSensor.pv);
      }
    };

    websocket.onclose = function() {
      var skip = 0;
      skip = skip + 1;
    };

    isActiveAcc();
  }
  );
  return {
    led1Command: function() {
      var data = JSON.stringify({
        id: sensorId,
        type: 'COMMAND',
        to: 'LED1_TOGGLE',
      });
      websocket.send(data);
    },

    led2Command: function() {
      var data = JSON.stringify({
        id: sensorId,
        type: 'COMMAND',
        to: 'LED2_TOGGLE',
      });
      websocket.send(data);
    },

    led3Command: function() {
      var data = JSON.stringify({
        id: sensorId,
        type: 'COMMAND',
        to: 'LED3_TOGGLE',
      });
      websocket.send(data);
    },

    led4Command: function() {
      var data = JSON.stringify({
        id: sensorId,
        type: 'COMMAND',
        to: 'LED4_TOGGLE',
      });
      websocket.send(data);
    },

    smoothCommand: function() {
      var data = JSON.stringify({
        id: sensorId,
        type: 'COMMAND',
        to: 'SMOOTH_TOGGLE',
      });
      websocket.send(data);
    },

    displayMessageCommand: function(msg) {
      var data = JSON.stringify({
        id: sensorId,
        type: 'COMMAND',
        to: 'TEST_MESSAGE',
        param: msg,
      });
      websocket.send(data);
    },

    mySensorId: function() {
      return sensorId;
    },
  };
}());
