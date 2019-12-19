window.onload = function() {
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

  function drawLightStatus(width, height) {
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

        var cellColour =
          y % 2
            ? "rgb(" +
              rDelta * x +
              "," +
              gDelta * y +
              "," +
              bDelta * (x + y) +
              ")"
            : "snow";

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

  //-----
  function CreateSVG() {
    var boxWidth = 300;
    var boxHeight = 300;

    var svgElem = document.createElementNS(xmlns, "svg");
    svgElem.setAttributeNS(
      null,
      "viewBox",
      "0 0 " + boxWidth + " " + boxHeight
    );
    svgElem.setAttributeNS(null, "width", boxWidth);
    svgElem.setAttributeNS(null, "height", boxHeight);
    svgElem.style.display = "block";

    var g = document.createElementNS(xmlns, "g");
    svgElem.appendChild(g);
    g.setAttributeNS(null, "transform", "matrix(1,0,0,-1,0,300)");

    // draw linear gradient
    var defs = document.createElementNS(xmlns, "defs");
    var grad = document.createElementNS(xmlns, "linearGradient");
    grad.setAttributeNS(null, "id", "gradient");
    grad.setAttributeNS(null, "x1", "0%");
    grad.setAttributeNS(null, "x2", "0%");
    grad.setAttributeNS(null, "y1", "100%");
    grad.setAttributeNS(null, "y2", "0%");
    var stopTop = document.createElementNS(xmlns, "stop");
    stopTop.setAttributeNS(null, "offset", "0%");
    stopTop.setAttributeNS(null, "stop-color", "#ff0000");
    grad.appendChild(stopTop);
    var stopBottom = document.createElementNS(xmlns, "stop");
    stopBottom.setAttributeNS(null, "offset", "100%");
    stopBottom.setAttributeNS(null, "stop-color", "#0000ff");
    grad.appendChild(stopBottom);
    defs.appendChild(grad);
    g.appendChild(defs);

    // draw borders
    var coords = "M 0, 0";
    coords += " l 0, 300";
    coords += " l 300, 0";
    coords += " l 0, -300";
    coords += " l -300, 0";

    var path = document.createElementNS(xmlns, "path");
    path.setAttributeNS(null, "stroke", "#000000");
    path.setAttributeNS(null, "stroke-width", 10);
    path.setAttributeNS(null, "stroke-linejoin", "round");
    path.setAttributeNS(null, "d", coords);
    path.setAttributeNS(null, "fill", "url(#gradient)");
    path.setAttributeNS(null, "opacity", 1.0);
    g.appendChild(path);

    var svgContainer = document.getElementById("svgContainer");
    svgContainer.appendChild(svgElem);
  }
  //-----

  drawLightStatus(190, 100);
};
