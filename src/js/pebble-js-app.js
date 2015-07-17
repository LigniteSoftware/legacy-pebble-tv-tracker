var MessageQueue = function() {
    var RETRY_MAX = 5;
    var queue = [];
    var sending = false;
    var timer = null;
    return {
        reset: reset,
        sendAppMessage: sendAppMessage,
        size: size
    };

    function reset() {
        queue = [];
        sending = false
    }

    function sendAppMessage(message, ack, nack) {
        if (!isValidMessage(message)) {
            return false
        }
        queue.push({
            message: message,
            ack: ack || null,
            nack: nack || null,
            attempts: 0
        });
        setTimeout(function() {
            sendNextMessage()
        }, 1);
        return true
    }

    function size() {
        return queue.length
    }

    function isValidMessage(message) {
        if (message !== Object(message)) {
            return false
        }
        var keys = Object.keys(message);
        if (!keys.length) {
            return false
        }
        for (var k = 0; k < keys.length; k += 1) {
            var validKey = /^[0-9a-zA-Z-_]*$/.test(keys[k]);
            if (!validKey) {
                console.log("(MessageQueue) Invalid key: '" + keys[k] + "'.");
                return false
            }
            var value = message[keys[k]];
            if (!validValue(value)) {
                return false
            }
        }
        return true;

        function validValue(value) {
            switch (typeof value) {
                case "string":
                case "number":
                case "boolean":
                    return true
                case "object":
                    if (toString.call(value) == "[object Array]") {
                        return true
                    }
            }
            console.log("(MessageQueue) Invalid value '" + value + "' which has a typeof: " + typeof value);
            return false
        }
    }

    function sendNextMessage() {
        if (sending) {
            return
        }
        var message = queue.shift();
        if (!message) {
            return
        }
        message.attempts += 1;
        sending = true;
        Pebble.sendAppMessage(message.message, ack, nack);
        timer = setTimeout(function() {
            timeout()
        }, 1e3);

        function ack() {
            clearTimeout(timer);
            setTimeout(function() {
                sending = false;
                sendNextMessage()
            }, 200);
            if (message.ack) {
                message.ack.apply(null, arguments)
            }
        }

        function nack() {
            clearTimeout(timer);
            if (message.attempts < RETRY_MAX) {
                queue.unshift(message);
                setTimeout(function() {
                    sending = false;
                    sendNextMessage()
                }, 200 * message.attempts)
            } else {
                if (message.nack) {
                    message.nack.apply(null, arguments)
                }
            }
        }

        function timeout() {
            setTimeout(function() {
                sending = false;
                sendNextMessage()
            }, 1e3);
            if (message.ack) {
                message.ack.apply(null, arguments)
            }
        }
    }
}();

var API_ROOT = "http://192.73.235.248:5000";
var CONFIG_URL = "http://tv.edwinfinch.com/index.php"

function subscribe_to_show(show){
    Pebble.timelineSubscribe(show,
      function () {
        console.log('Subscribed to ' + show + '.');
      },
      function (errorString) {
        console.log('Error subscribing to topic: ' + errorString);
      }
    );
    var xhr = new XMLHttpRequest();
    var url = "http://tv.edwinfinch.com:3000";
    xhr.open('POST', url, true);
    xhr.onload = function() {
        if(xhr.readyState == 4 && xhr.status == 200){
            console.log('Subscribe server response: ' + xhr.responseText);
        }
        else if(xhr.status != 200){
            console.log("Error: " + xhr.responseText + " (status code: " + xhr.status + ")");
        }
    };
    xhr.send("show=" + show + "&username=" + localStorage.getItem("username") + "&accessToken=" + localStorage.getItem("accessToken"));
}

function fetch_all_shows_from_channel(channel_id){
    var xhr = new XMLHttpRequest();
    var url = "http://tv.edwinfinch.com:1000";
    xhr.open('POST', url, true);
    xhr.onload = function() {
        if(xhr.readyState == 4 && xhr.status == 200){
            console.log('Fetch server response found: ' + xhr.responseText + ' Running through appmessagequene.');
            var object = JSON.parse(xhr.responseText);
            var shows_array = object.programme;

            for(var i = 0; i < shows_array.length; i++){
                var show = shows_array[i];
                var object = {
                    "show_name":show.title,
                    "show_channel_id":show.attributes.channel,
                    "show_new":show.new
                };
                console.log("Sending: " + JSON.stringify(object));
                MessageQueue.sendAppMessage(object);
                //Pebble.sendAppMessage(object);
            }
        }
        else if(xhr.status != 200){
            console.log("Error: " + xhr.responseText + " (status code: " + xhr.status + ")");
        }
    };
    xhr.send("request_name=" + channel_id.substring(0, 6) + "&request_type=channel");
}

function fetch_all_channels(){
    var xhr = new XMLHttpRequest();
    var url = "http://tv.edwinfinch.com:1000";
    xhr.open('POST', url, true);
    xhr.onload = function() {
        if(xhr.readyState == 4 && xhr.status == 200){
            console.log('Fetch server response found: ' + xhr.responseText + ' Running through appmessagequene.');
            var object = JSON.parse(xhr.responseText);
            var channels_array = object.channel;

            for(var i = 0; i < channels_array.length; i++){
                var channel = channels_array[i];
                var object = {
                    "channel_name":channel.display_name[2],
                    "channel_id":channel.attributes.id,
                    "channel_number":parseInt(channel.display_name[1])
                };
                console.log("Sending: " + JSON.stringify(object));
                MessageQueue.sendAppMessage(object);
                //Pebble.sendAppMessage(object);
            }
        }
        else if(xhr.status != 200){
            console.log("Error: " + xhr.responseText + " (status code: " + xhr.status + ")");
        }
    };
    xhr.send("request_type=all_channels");
}

function ready_handler(e){
    //Do something?
    console.log("Ready for a long beating.");
}

function config_closed_handler(e){
    if (e.response !== undefined && e.response !== '' && e.response !== 'CANCELLED') {
		values = JSON.parse(decodeURIComponent(e.response));

        Pebble.sendAppMessage(values);
	}
}

function app_message_handler(e){
    console.log("Got an appmessage");
    switch(e.payload.MESSAGE_TYPE){
        //Channel search
        case 1001:
            fetch_all_channels();
            break;
        //Show (all) search
        case 1002:
            fetch_all_shows_from_channel(e.payload.channel_id);
            break;
        //Show subscribe
        case 1003:
            break;
        //Show unsubscribe
        case 1004:
            break;
    }
}

function open_config_handler(e){
    Pebble.openURL(CONFIG_URL);
}

Pebble.addEventListener("ready", ready_handler);
Pebble.addEventListener("webviewclosed", config_closed_handler);
Pebble.addEventListener("appmessage", app_message_handler);
Pebble.addEventListener("showConfiguration", open_config_handler);
