var width_screen = 0;
var height_screen = 0;

var mean_acc_x_circle = [0, 0];
var mean_acc_y_circle = [0, 0];
var mean_acc_x_circle_wii = [0, 0];
var mean_acc_y_circle_wii = [0, 0];


/**** This function is responsible for drawing the circle which moves on according to accelerometers ****/
function refreshBall(id)
{
  var canvas = (id == 0) ? document.getElementById("circle_acc") : document.getElementById("circle_wii");
  var ctx = canvas.getContext("2d");
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.strokeStyle = '#888';
  ctx.lineWidth   = 1;
  ctx.globalAlpha = 1;

  ctx.beginPath();
  for (var x = 0; x <= canvas.width; x += 10) {
    ctx.moveTo(x, 0);
    ctx.lineTo(x, canvas.height);
  }

  for (var x = 0; x <= canvas.height; x += 10) {
    ctx.moveTo(0, x);
    ctx.lineTo(canvas.width, x);
  }
  ctx.stroke();
  ctx.closePath();

  ctx.strokeStyle = '#000';
  ctx.lineWidth   = 2;

  ctx.beginPath();

  ctx.beginPath();
  ctx.moveTo(0, parseInt(canvas.height)/2);
  ctx.lineTo(4*parseInt(canvas.width)/5 - 10, parseInt(canvas.height)/2);
  ctx.moveTo(4*parseInt(canvas.width)/5 + 10, parseInt(canvas.height)/2);
  ctx.lineTo(parseInt(canvas.width), parseInt(canvas.height)/2);

  ctx.moveTo(parseInt(canvas.width) - 5, parseInt(canvas.height)/2 + 5);
  ctx.lineTo(parseInt(canvas.width), parseInt(canvas.height)/2);
  ctx.lineTo(parseInt(canvas.width) - 5, parseInt(canvas.height)/2 - 5);


  ctx.moveTo(parseInt(canvas.width)/2, 0);
  ctx.lineTo(parseInt(canvas.width)/2, 4*parseInt(canvas.height)/5 - 10);
  ctx.moveTo(parseInt(canvas.width)/2, 4*parseInt(canvas.height)/5 + 10);
  ctx.lineTo(parseInt(canvas.width)/2, parseInt(canvas.height));

  ctx.moveTo(parseInt(canvas.width)/2 - 5, parseInt(canvas.height) - 5);
  ctx.lineTo(parseInt(canvas.width)/2, parseInt(canvas.height));
  ctx.lineTo(parseInt(canvas.width)/2 + 5, parseInt(canvas.height) - 5);

  ctx.stroke();
  ctx.closePath();

  ctx.font = "bold 12px sans-serif";
  ctx.fillStyle = "#000"; // text color
  ctx.fillText("x", 4*parseInt(canvas.width)/5 - 3, parseInt(canvas.height)/2 + 3);
  ctx.fillText("y", parseInt(canvas.width)/2 - 2, 4*parseInt(canvas.height)/5 + 2);

  ctx.beginPath();


  //we obtain x and y between 0 and canvas_width or canvas_height
  var data_x = (id == 0) ? data_acc_x : data_acc_x_wii;
  var data_y = (id == 0) ? data_acc_y : data_acc_y_wii;
  var nx = parseInt(data_x) - ((900 + 300) / 2)
  var ny = parseInt(data_y) - ((900 + 300) / 2)
  var x = (parseInt(canvas.width)/2) * parseInt(data_x) / 300 + parseInt(canvas.width)/2;
  var y = (parseInt(canvas.height)/2) * parseInt(data_y) / 300 + parseInt(canvas.height)/2;
  
  if(id == 0)
  {
    for(var i = 0; i < mean_acc_x_circle.length - 1; i++)
    {
      mean_acc_x_circle[i] = mean_acc_x_circle[i + 1];
      mean_acc_y_circle[i] = mean_acc_y_circle[i + 1];
    }
    mean_acc_x_circle[mean_acc_x_circle.length - 1] = x;
    mean_acc_y_circle[mean_acc_y_circle.length - 1] = y;

    x = 0;
    y = 0;

    for(var i = 0; i < mean_acc_x_circle.length; i++)
    {
      x += mean_acc_x_circle[i];
      y += mean_acc_y_circle[i];
    }

    x /= mean_acc_x_circle.length;
    y /= mean_acc_y_circle.length;
  }
  else if(id == 1)
  {
    for(var i = 0; i < mean_acc_x_circle_wii.length - 1; i++)
    {
      mean_acc_x_circle_wii[i] = mean_acc_x_circle_wii[i + 1];
      mean_acc_y_circle_wii[i] = mean_acc_y_circle_wii[i + 1];
    }
    mean_acc_x_circle_wii[mean_acc_x_circle_wii.length - 1] = x;
    mean_acc_y_circle_wii[mean_acc_y_circle_wii.length - 1] = y;

    x = 0;
    y = 0;

    for(var i = 0; i < mean_acc_x_circle_wii.length; i++)
    {
      x += mean_acc_x_circle_wii[i];
      y += mean_acc_y_circle_wii[i];
    }

    x /= mean_acc_x_circle_wii.length;
    y /= mean_acc_y_circle_wii.length;

  }

  $("#debug").html("x: " + x + " y " + y + " data_x: " + data_acc_x + " data_y: " + data_acc_y);

  ctx.strokeStyle = '#000';
  ctx.lineWidth   = 2;
  ctx.arc(x, y, 10, 0, Math.PI*2, true);
  ctx.stroke();

  ctx.fillStyle = "rgb(255,10,10)";
  ctx.globalAlpha = 0.7;
  ctx.fill();
  ctx.closePath();

}


/**** This function is responsible updating the bar which indicates the level of light ****/
function refreshLight(id)
{
  var canvas = (id == 0) ? document.getElementById("animation_light") : document.getElementById("animation_light1");

  var rectangle_width = 5;
  var rectangle_height = 50;
  var space = 5;
  var number_rect = canvas.width / (space + rectangle_width);

  
  var light_scaled = (id == 0) ? number_rect * data_light / 140 : number_rect * data_light1 / 140;

  var ctx = canvas.getContext("2d");

  ctx.strokeStyle = '#000';
  ctx.lineWidth   = 1;
  ctx.globalAlpha = 1;

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // we print temp and press
  ctx.closePath();

  if(id == 0)
  {
    $("#press").html(data_press + " Pa");
    $("#temp").html(data_temp + " &ordm;C");
  }
  else if(id==1)
  {
    $("#press1").html(data_press1 + " Pa");
    $("#temp1").html(data_temp1 + " &ordm;C");

  }

  ctx.beginPath();

  for(var i = 0; i < number_rect; i++)
  {
    ctx.beginPath();
    ctx.fillStyle = 'rgb(255,' + Math.floor(255 - (255/number_rect)*i) + ', 0)';

    ctx.moveTo( (space + rectangle_width)*i + 1, canvas.height - rectangle_height - space);
    ctx.lineTo( (space + rectangle_width)*i + rectangle_width, canvas.height - rectangle_height - space);
    ctx.lineTo( (space + rectangle_width)*i + rectangle_width, canvas.height - space);
    ctx.lineTo( (space + rectangle_width)*i + 1 , canvas.height - space);
    ctx.lineTo( (space + rectangle_width)*i + 1 , canvas.height - rectangle_height - space);
    ctx.closePath();
    ctx.stroke();
    if(i <= light_scaled)
      ctx.fill();
  }
}




/**** This function is responsible updating the bar which indicates the level of light ****/
function refreshLight_mic(id)
{
  var canvas = (id == 0) ? document.getElementById("animation_light") : document.getElementById("animation_light1");

  //first, we draw light bar
  var rectangle_width = 3;
  var rectangle_height = 50;
  var space = 2;
  var number_rect = (3*canvas.width/5 - 20) / (space + rectangle_width);

  
  var light_scaled = (id == 0) ? number_rect * data_light / 140 : number_rect * data_light1 / 140;

  var ctx = canvas.getContext("2d");

  ctx.strokeStyle = '#000';
  ctx.lineWidth   = 1;
  ctx.globalAlpha = 1;

  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // we print temp and press
  ctx.closePath();

  if(id == 0)
  {
    $("#press").html(data_press + " Pa");
    $("#temp").html(data_temp + " &ordm;C");
  }
  else if(id == 1)
  {
    $("#press1").html(data_press1 + " Pa");
    $("#temp1").html(data_temp1 + " &ordm;C");
  }


  for(var i = 0; i < number_rect; i++)
  {
    ctx.beginPath();
    ctx.fillStyle = 'rgb(255,' + Math.floor(255 - (255/number_rect)*i) + ', 0)';

    ctx.moveTo( (space + rectangle_width)*i + 1, canvas.height - rectangle_height - space);
    ctx.lineTo( (space + rectangle_width)*i + rectangle_width, canvas.height - rectangle_height - space);
    ctx.lineTo( (space + rectangle_width)*i + rectangle_width, canvas.height - space);
    ctx.lineTo( (space + rectangle_width)*i + 1 , canvas.height - space);
    ctx.lineTo( (space + rectangle_width)*i + 1 , canvas.height - rectangle_height - space);
    ctx.closePath();
    ctx.stroke();
    if(i <= light_scaled)
      ctx.fill();
  }



  rectangle_width = 40;
  rectangle_height = 3;
  space = 2;
  number_rect = canvas.height / (space + rectangle_height);

  var mic_scaled = (id == 0) ? number_rect * data_mic / 7000 : number_rect * data_mic1 / 7000;


  ctx.strokeStyle = '#000';
  ctx.lineWidth   = 1;
  ctx.globalAlpha = 1;

  //$("#debug").html("data_mic: " + data_mic + "mic_scaled: " + mic_scaled);
  //var x_begin = canvas.width/2 - rectangle_width/2 - rectangle_width/1.5;
  var x_begin = 3*canvas.width/5 + 10;

  for(var i = 0; i < number_rect; i++)
  {
    if( canvas.height - (rectangle_height + space)*(i+1) > 0)
    {
      ctx.beginPath();
      ctx.fillStyle = 'rgb(150,' + Math.floor(255 - (255/number_rect)*i) + ', 0)';

      ctx.moveTo( x_begin, canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.closePath();
      ctx.stroke();
      if(i <= mic_scaled)
        ctx.fill();
    }
  }
  x_begin = x_begin + rectangle_width + 5;

  for(var i = 0; i < number_rect; i++)
  {
    if( canvas.height - (rectangle_height + space)*(i+1) > 0)
    {
      ctx.beginPath();
      ctx.fillStyle = 'rgb(150,' + Math.floor(255 - (255/number_rect)*i) + ', 0)';

      ctx.moveTo( x_begin, canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.closePath();
      ctx.stroke();
      if(i <= mic_scaled)
        ctx.fill();
    }
  }
}






/**** This function is responsible updating the bar which indicates the level of sound ****/
/*function refreshMic()
{
  var canvas = document.getElementById("animation_mic");

  var rectangle_width = 70;
  var rectangle_height = 5;
  var space = 3;
  var number_rect = canvas.height / (space + rectangle_height);

  var mic_scaled = number_rect * data_mic / 7000;

  var ctx = canvas.getContext("2d");

  ctx.strokeStyle = '#000';
  ctx.lineWidth   = 1;
  ctx.globalAlpha = 1;

  ctx.clearRect(0, 0, canvas.width, canvas.height);
  //$("#debug").html("data_mic: " + data_mic + "mic_scaled: " + mic_scaled);
  var x_begin = canvas.width/2 - rectangle_width/2 - rectangle_width/1.5;

  for(var i = 0; i < number_rect; i++)
  {
    if( canvas.height - (rectangle_height + space)*(i+1) > 0)
    {
      ctx.beginPath();
      ctx.fillStyle = 'rgb(150,' + Math.floor(255 - (255/number_rect)*i) + ', 0)';

      ctx.moveTo( x_begin, canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.closePath();
      ctx.stroke();
      if(i <= mic_scaled)
        ctx.fill();
    }
  }
  x_begin = canvas.width/2 - rectangle_width/2 + rectangle_width/1.5;

  for(var i = 0; i < number_rect; i++)
  {
    if( canvas.height - (rectangle_height + space)*(i+1) > 0)
    {
      ctx.beginPath();
      ctx.fillStyle = 'rgb(150,' + Math.floor(255 - (255/number_rect)*i) + ', 0)';

      ctx.moveTo( x_begin, canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.lineTo( x_begin + rectangle_width,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1) + rectangle_height);
      ctx.lineTo( x_begin,  canvas.height - (rectangle_height + space)*(i+1));
      ctx.closePath();
      ctx.stroke();
      if(i <= mic_scaled)
        ctx.fill();
    }
  }

}*/


