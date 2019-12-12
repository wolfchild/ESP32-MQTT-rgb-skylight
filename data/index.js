window.onload = function() {
  var autoRefresh = true;

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
      '">' +
      n.ssid +
      " (" +
      securityToString(n.security) +
      ")" +
      "</option>"
    );
  }

  function getWiFiNetworks() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (xhr.readyState === 4) {
        var networkList = JSON.parse(xhr.responseText);
        var networkListHtml = "";
        for (var i = 0; i < networkList.length; i++) {
          networkListHtml += networkDetailsToHtml(networkList[i]);
        }
        document.getElementById("networkSelector").innerHTML = networkListHtml;
      }
    };
    xhr.open("GET", "/api/scan");
    xhr.send();
  }

  // ----
  var networkSelector = this.document.getElementById("networkSelector");

  networkSelector.onchange = function(e) {
    if (e.srcElement.selectedIndex != undefined) {
      document.getElementById("ssid").value = e.target.value;
    }
  };

  // ----
  this.setTimeout(function() {
    if (autoRefresh) {
      getWiFiNetworks();
    }
  }, 10000);

  getWiFiNetworks();
};
