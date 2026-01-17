function parseWeather(json) {
  var timeseries = json.properties.timeseries;
  
  // Limit to the next 6 hours (or fewer if data is missing)
  var limit = Math.min(timeseries.length, 6);
  
  var maxUV = 0;
  
  // Start with 1.0 (100%) probability of it NOT happening
  var noRainProb = 1.0; 
  var noThunderProb = 1.0; 

  for (var i = 0; i < limit; i++) {
    var data = timeseries[i].data;
    
    // 1. Find highest UV index
    var uv = data.instant.details.ultraviolet_index_clear_sky || 0;
    if (uv > maxUV) {
      maxUV = uv;
    }

    // 2. Calculate total probability
    if (data.next_1_hours && data.next_1_hours.details) {
      var rain = data.next_1_hours.details.probability_of_precipitation || 0;
      noRainProb = noRainProb * (1 - (rain / 100));

      var thunder = data.next_1_hours.details.probability_of_thunder || 0;
      noThunderProb = noThunderProb * (1 - (thunder / 100));
    }
  }

  return {
    'UV': Math.round(maxUV),
    'Rain': Math.round((1 - noRainProb) * 100),
    'Thunder': Math.round((1 - noThunderProb) * 100)
  };
}

function locationSuccess(position) {
  var req = new XMLHttpRequest();
  req.open(
    "GET",
    "https://api.met.no/weatherapi/locationforecast/2.0/complete?" +
      "lat=" + position.coords.latitude +
      "&lon=" + position.coords.longitude,
    true
  );

  req.setRequestHeader("User-Agent", "PebbleWatchface/1.0 (github.com/alexanderteinum/pebble-out)");

  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        var json = JSON.parse(req.responseText);
        var toSend = parseWeather(json);
        console.log("Sending to Pebble:", JSON.stringify(toSend));
        Pebble.sendAppMessage(toSend);
      } else {
        console.log("HTTP error:", req.status);
      }
    }
  };
  req.send(null);
}

function locationError(err) {
  console.log("Location error!");

  Pebble.sendAppMessage({
    'UV': -1,
    'Rain': -1,
    'Thunder': -1
  });
}

function getWeather() {
  window.navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {
      timeout: 15000,
      maximumAge: 60000
    }
  );
}

Pebble.addEventListener("ready", function (e) {
  console.log("Pebble JS ready!");
  getWeather();
});

Pebble.addEventListener("appmessage", function (e) {
  console.log("AppMessage received from Pebble, fetching weather...");
  getWeather();
});
