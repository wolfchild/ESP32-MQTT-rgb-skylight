window.onload = function() {
  var lightStatus = [
    { r: 255, g: 0, b: 0 },
    { r: 0, g: 255, b: 0 },
    { r: 0, g: 0, b: 255 },
    { r: 128, g: 128, b: 128 }
  ];

  function doApiGet(url, successCallback) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (xhr.readyState === 4) {
        if (successCallback instanceof Function) {
          successCallback(xhr.responseText);
        }
      }
    };
    xhr.open("GET", url);
    xhr.send();
  }

  function doApiPost(url, successCallback, errorCallback) {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (xhr.readyState === XMLHttpRequest.DONE) {
        if (xhr.status == 200 && successCallback instanceof Function) {
          successCallback(xhr.responseText);
        }
        if (xhr.status == 500 && errorCallback instanceof Function) {
          errorCallback(xhr.responseText);
        }
      }
    };
    xhr.open("POST", url);
    xhr.send();
  }

  function drawLightStatus(width, height, lightStatus) {
    var xmlns = "http://www.w3.org/2000/svg";
    var image = document.createElementNS(xmlns, "svg");
    image.setAttributeNS(null, "viewBox", "0 0 " + width + " " + height);
    //image.setAttributeNS(null, "width", width);
    //image.setAttributeNS(null, "height", height);
    image.style.display = "block";

    var filter = document.createElementNS(xmlns, "filter");
    filter.setAttributeNS(null, "id", "blur");

    var blur = document.createElementNS(xmlns, "feGaussianBlur");
    blur.setAttributeNS(null, "in", "SourceGraphic");
    blur.setAttributeNS(null, "stdDeviation", 5);
    filter.appendChild(blur);

    var g = document.createElementNS(xmlns, "g");
    g.setAttributeNS(null, "filter", "url(#blur");

    image.appendChild(filter);
    image.appendChild(g);

    var cellWidth = width / 19;
    var cellHeight = height / 20;

    for (var x = 0; x < 19; x++) {
      for (var y = 0; y < 20; y++) {
        var rDelta = Math.floor(256 / 19);
        var gDelta = Math.floor(256 / 10);
        var bDelta = Math.floor(256 / 29);
        var cs = lightStatus[(x * y + y) % lightStatus.length];
        var cellColour = "rgb(" + cs.r + "," + cs.g + "," + cs.b + ")";

        var cell = document.createElementNS(xmlns, "rect");
        cell.setAttributeNS(null, "width", cellWidth);
        cell.setAttributeNS(null, "height", cellHeight);
        cell.setAttributeNS(null, "x", x * cellWidth);
        cell.setAttributeNS(null, "y", y * cellHeight);
        cell.setAttributeNS(null, "fill", cellColour);
        g.appendChild(cell);
      }
    }

    var lightsStatusElement = document.getElementById("lights");
    lightsStatusElement.appendChild(image);
  }

  drawLightStatus(190, 100, lightStatus);
};
