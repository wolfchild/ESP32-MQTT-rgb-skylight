window.onload = function() {
  // attach click handlers

  this.document.getElementById("networkSelector").onchange = function(e) {
    if (e.srcElement.selectedIndex != undefined) {
      document.getElementById("ssid").value = e.target.value;
    }
  };
};
