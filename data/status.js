window.onload = function() {
  var lightStatus = {
    width: 19,
    height: 20,
    whiteColor: { r: 251, g: 241, b: 224 }, //http://planetpixelemporium.com/tutorialpages/light.html
    whiteLevel: 255,
    values: firePattern(19, 10)
  };

  function firePattern(width, height) {
    var values = [];

    for (var pixel = 0; pixel < width * height; pixel++) {
      values[pixel] = { r: pixel * 1.5, g: pixel * 1.2, b: pixel * 0.5 };
    }

    return values;
  }

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

    var cellWidth = width / lightStatus.width;
    var cellHeight = height / lightStatus.height;
    for (var y = 0; y < lightStatus.height; y++) {
      if (y % 2 == 0) {
        // white background illumination
        var cell = document.createElementNS(xmlns, "rect");
        var wc = lightStatus.whiteColor;
        var wl = lightStatus.whiteLevel / 256;
        var cellColour =
          "rgb(" + wc.r * wl + "," + wc.g * wl + "," + wc.b * wl + ")";

        cell.setAttributeNS(null, "width", width);
        cell.setAttributeNS(null, "height", cellHeight);
        cell.setAttributeNS(null, "x", 0);
        cell.setAttributeNS(null, "y", y * cellHeight);
        cell.setAttributeNS(null, "fill", cellColour);
        g.appendChild(cell);
      } else {
        // rgb mood illumination
        for (var x = 0; x < lightStatus.width; x++) {
          var cs = lightStatus.values[(x * y + y) % lightStatus.values.length];
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
    }

    var lightsStatusElement = document.getElementById("lights");
    lightsStatusElement.appendChild(image);
  }

  drawLightStatus(190, 100, lightStatus);
};
