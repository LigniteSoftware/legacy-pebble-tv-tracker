var API_ROOT = "http://192.73.235.248:5000";

function requestPin(userToken, minutesToAdd) {
  var xhr = new XMLHttpRequest();

  // construct the url for the api
  var url = API_ROOT + '/senduserpin/' + userToken + '/' + minutesToAdd;

  xhr.open('GET', url, true);
  xhr.onload = function() {
    console.log('requestPin server response: ' + xhr.responseText);

    // Update text on the watch to say we've sent the pin
    Pebble.sendAppMessage({text: 'Sent!\nCheck your timeline!'});

    // set a timer to quit the app in 2 seconds
    setTimeout(function() {
      Pebble.sendAppMessage({quit: true});
    }, 2000);
  };

  xhr.send();
}

Pebble.addEventListener("ready", function(e){
    return;
    Pebble.timelineSubscribe('chuck',
      function () {
        console.log('Subscribed to chuck!');
      },
      function (errorString) {
        console.log('Error subscribing to topic: ' + errorString);
      }
    );

    Pebble.getTimelineToken(
      function (token) {
        console.log('My timeline token is ' + token);
        requestPin(token, 0);
      },
      function (error) {
        console.log('Error getting timeline token: ' + error);
      }
    );
});
