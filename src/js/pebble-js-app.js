var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  var req = new XMLHttpRequest();
  var reqAPI = new XMLHttpRequest();
  var response;
  var responseAPI;
  var city;
  var country;
  console.log("lat:" + pos.coords.latitude + " long:" + pos.coords.longitude);
  // Construct URL
  var url = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude;
  // var geoAPI = "http://where.yahooapis.com/geocode?location="+pos.coords.latitude+","+pos.coords.longitude+"&flags=J&gflags=R&appid=4KEJ6Y4k";
  console.log("url: " + url);
  reqAPI.open('GET', url, false);
  reqAPI.send();
  if (reqAPI.readyState == 4) {
      if(reqAPI.status == 200) {
        console.log("REQAPI.RESPONSETEXT: " + reqAPI.responseText);
        responseAPI = JSON.parse(reqAPI.responseText);
        city = responseAPI.name;
        country = responseAPI.sys.country;

      } else {
        console.log("Error");
      }
  }
  console.log("city: " + city);
  console.log("country: " + country);
  
  // var yahoo_weather = "https://query.yahooapis.com/v1/public/yql?q=select%20item.condition%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.placefinder%20where%20text%3D%22%20" + pos.coords.latitude + "%2C%20" + pos.coords.longitude + "%22%20and%20gflags%3D%22R%22)%20and%20u%3D%22c%22&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
  // var yahoo_weather = "https://query.yahooapis.com/v1/public/yql?q=select%20item.condition%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22"+ city +"%2C%20"+ country + "%22)&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
  var yahoo_weather = "https://query.yahooapis.com/v1/public/yql?q=select%20item.condition%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22"+ city +"%2C%20"+ country + "%22)%20and%20u%3D%22c%22&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
  console.log("yahoo_weather: " + yahoo_weather);
  req.open('GET', yahoo_weather, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log("REQ.RESPONSETEXT: " + req.responseText);
        response = JSON.parse(req.responseText);
        var temp = response.query.results.channel.item.condition.temp;
        var cond = response.query.results.channel.item.condition.code;
        // console.log("response= " + response["query"]["results"]["channel"]["item"]["condition"]["temp"]);
        console.log("Temperature= " + temp);
        console.log("Conditions code= " + cond);
        var temperature = Math.round(temp);
        var conditions = Math.round(cond);
        var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": conditions
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
        
      } else {
        console.log("Error");
      }
    }
  };
  req.send(null);
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);
