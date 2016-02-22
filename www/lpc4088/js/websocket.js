/*jslint browser: true */
/*global jQuery, SmoothieChart, TimeSeries, manipulateDOM, WebSocket, $*/

var WSN_CLIENT = (function () {
    'use strict';

    var activityAcc = 0,
        accX = new TimeSeries(),
        accY = new TimeSeries(),
        accZ = new TimeSeries(),
        dataAccX = 0,
        dataAccY = 0,
        dataAccZ = 0,
        websocket,
        chartAcc;

    function showGraph(chart) {
        if (chart === "acc") {
            $('#chartarea_acc').slideDown();
            document.getElementById("chart_acc").width = 300;
            document.getElementById("chart_acc").height = 200;
        }
    }

    function showMain() {
        showGraph("acc");
    }

    function createTimeline() {
        chartAcc = new SmoothieChart({strokeStyle: 'rgb(125, 0, 0)', fillStyle: 'rgb(60, 0, 0)', lineWidth: 1, maxValue: 120, minValue: -120, millisPerLine: 250, verticalSections: 6 });
        chartAcc.addTimeSeries(accX, { strokeStyle: 'rgba(255, 0, 0, 1)', fillStyle: 'rgba(255, 0, 0, 0.1)', lineWidth: 3 });
        chartAcc.addTimeSeries(accY, { strokeStyle: 'rgba(0, 255, 0, 1)', fillStyle: 'rgba(0, 255, 0, 0.1)', lineWidth: 3 });
        chartAcc.addTimeSeries(accZ, { strokeStyle: 'rgba(0, 0, 255, 1)', fillStyle: 'rgba(0, 0, 255, 0.1)', lineWidth: 3 });
        chartAcc.streamTo(document.getElementById("chart_acc"), 200);
    }

    function isActiveAcc() {
        if (activityAcc === 0) {
            $("#chart_acc").fadeTo(10, 0.3);
        }
        activityAcc = 0;
        setTimeout(isActiveAcc, 2000);
    }

    $(document).ready(function () {
        showMain();
        createTimeline();

        websocket = new WebSocket('ws://hesabu.net:9000/ws/sensors/rw');

        websocket.onopen = function () {
            var skip = 0;
            skip = skip + 1;
        };

        websocket.onmessage = function (evt) {
            var theDate,
                jsonSensor = jQuery.parseJSON(evt.data.toString());
            theDate = new Date().getTime();
            if (jsonSensor.id === "SN01") {
                activityAcc = 1;
                $("#chart_acc").fadeTo(100, 1);
                dataAccX = jsonSensor.ax;
                dataAccY = jsonSensor.ay;
                dataAccZ = jsonSensor.az;

                accX.append(theDate, dataAccX);
                accY.append(theDate, dataAccY);
                accZ.append(theDate, dataAccZ);
            } 
        };

        websocket.onclose = function () {
            var skip = 0;
            skip = skip + 1;
        };

        isActiveAcc();
    });
    return {
        led1Command : function () {
            var data = JSON.stringify({id : 'SN01', type : 'COMMAND', to : 'LED1_TOGGLE'});
            websocket.send(data);
        },
        led2Command : function () {
            var data = JSON.stringify({id : 'SN01', type : 'COMMAND', to : 'LED2_TOGGLE'});
            websocket.send(data);
        },
        led3Command : function () {
            var data = JSON.stringify({id : 'SN01', type : 'COMMAND', to : 'LED3_TOGGLE'});
            websocket.send(data);
        },
        led4Command : function () {
            var data = JSON.stringify({id : 'SN01', type : 'COMMAND', to : 'LED4_TOGGLE'});
            websocket.send(data);
        },
        smoothCommand : function () {
            var data = JSON.stringify({id : 'SN01', type : 'COMMAND', to : 'SMOOTH_TOGGLE'});
            websocket.send(data);
        },
        testMessageCommand : function () {
            var data = JSON.stringify({id : 'SN01', type : 'COMMAND', to : 'TEST_MESSAGE'});
            websocket.send(data);
        }
    };
}());
