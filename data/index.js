window.onload = function() {
  // Source: https://www.metageek.com/training/resources/understanding-rssi.html
  function rssiToCssClass(rssi) {
    var signalStrength = 0;
    if (rssi >= -30) {
      signalStrength = 4;
    } else if (rssi >= -67) {
      signalStrength = 3;
    } else if (rssi >= -70) {
      signalStrength = 2;
    } else if (rssi >= -80) {
      signalStrength = 1;
    }
    return "wifi-" + signalStrength;
  }

  function securityToString(security) {
    switch (security) {
      case 0: //    WIFI_AUTH_OPEN
        return "Open";
      case 1: //    WIFI_AUTH_WEP
        return "WEP";
      case 2: //    WIFI_AUTH_WPA_PSK
        return "WPA+PSK";
      case 3: //    WIFI_AUTH_WPA2_PSK
        return "WPA2+PSK";
      case 4: //    WIFI_AUTH_WPA_WPA2_PSK
        return "WPA+WPA2+PSK";
      case 5: //    WIFI_AUTH_WPA2_ENTERPRISE
        return "WPA2 Enterprise";
      case 6: //    WIFI_AUTH_MAX
        return "MAX";
      default:
        return "Unknown";
    }
  }

  function networkDetailsToHtml(n) {
    return (
      '<option value="' +
      n.ssid +
      '" class="imagebacked ' +
      rssiToCssClass(n.rssi) +
      '" data-security="' +
      n.security +
      '">' +
      n.ssid +
      " (" +
      securityToString(n.security) +
      ")" +
      "</option>"
    );
  }

  function doApiGet(url, successCallback, errorCallback) {
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
        if ((xhr.status == 200) && (successCallback instanceof Function) {
          successCallback(xhr.responseText);
        } 
        if ((xhr.status == 500) && (errorCallback instanceof Function){
          errorCallback(xhr.responseText);
        }
      }
    };
    xhr.open("POST", url);
    xhr.send();
  }

  function getWiFiNetworks() {
    doApiGet("/api/scan", function(responseText) {
      var networkList = JSON.parse(responseText);
      var networkListHtml = "";
      for (var i = 0; i < networkList.length; i++) {
        networkListHtml += networkDetailsToHtml(networkList[i]);
      }
      document.getElementById("networkSelector").innerHTML = networkListHtml;
    });
  }

  function getWiFiConfig() {
    doApiGet("/api/wificonfig", function(responseText) {
      var configuration = JSON.parse(responseText);

      document.getElementById("ssid").value = configuration.ssid;
      document.getElementById("key").value = configuration.key;
    });
  }

  // ---- attach ui event handlers
  this.document.getElementById("networkSelector").onchange = function(e) {
    if (e.srcElement.selectedIndex != undefined) {
      document.getElementById("ssid").value = e.target.value;
      var selectedOption = e.target.options[e.target.selectedIndex];

      var keyInputField = document.getElementById("key");
      keyInputField.value = "";
      keyInputField.disabled =
        selectedOption.getAttribute("data-security") == "0";
    }

    this.document.getElementById("save-button").onclick = function(e) {};
  };

  getWiFiConfig();
  getWiFiNetworks();
};
