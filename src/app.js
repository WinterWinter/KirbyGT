var city = "";


var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url;
  
  if (pos !== undefined) {

  url = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude + '&appid=' + '4f4f0b5ecd03fb8e857be86378159a38';
  }
  else{
    
  url = 'http://api.openweathermap.org/data/2.5/weather?&q=' + city + '&appid=4f4f0b5ecd03fb8e857be86378159a38';
  }
  
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(text) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(text);
      
      var city_called = json.name;
			console.log("City in response is " + city_called);
      

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp);
      var icon = iconFromWeatherId(json.weather[0].id);
      
      console.log("Temperature is " + temperature);
      console.log(icon);

      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_ICON":icon
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );}      
  );
}

function iconFromWeatherId(weatherId) {
  if (weatherId < 300 || weatherId < 600 || weatherId < 700 || weatherId > 800) {
    return 1;
  } else {
    return 0;
  }
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  if (city === "") {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
  }else{
    locationSuccess();
  }
}

Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS Ready!');
  city = localStorage.city;

  Pebble.sendAppMessage({'0': 0
	}, function(e) {
      console.log('Sent ready message!');
  }, function(e) {
      console.log('Failed to send ready message');
			console.log(e);
  });
});
  

  // Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

  // Get the initial weather
    getWeather();
  });

  // Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  });

  //Show Configuration

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://winterwinter.github.io/KirbyGT/';

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  city = configData.city;
	console.log("Entered city is " + city);
  localStorage.city = city;
  

                        
  // Send to the watchapp via AppMessage
var dict = {
  "KEY_SCALE" : configData.scale,
  "KEY_STEPSGOAL" : configData.stepsgoal
};

  // Send to the watchapp
Pebble.sendAppMessage(dict, function() {
  console.log('Config data sent successfully!');
}, function(e) {
  console.log('Error sending config data!');

});
});



