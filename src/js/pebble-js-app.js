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

function replaceAll(find, replace, str) {
  return str.replace(new RegExp(find, 'g'), replace);
}

function subscribe_to_show(show, channel){
    var topic = replaceAll(" ", "", show);
    console.log("Subscribing to " + topic + "__" + channel);
    Pebble.timelineSubscribe(topic + "__" + channel,
      function () {
        console.log('Subscribed to ' + replaceAll(" ", "", show) + '.');
        MessageQueue.sendAppMessage({
            "ACTION_STATUS": 1
        });
      },
      function (errorString) {
        console.log('Error subscribing to topic: ' + errorString);
        MessageQueue.sendAppMessage({
            "ACTION_STATUS": 0,
            "ACTION_ERROR": errorString
        });
      }
    );
}

function unsubscribe_from_show(show){
    var topic = replaceAll(" ", "", show);
    console.log("Unsubscribing from " + topic + "__" + channel);
    Pebble.timelineUnsubscribe(topic + "__" + channel,
      function () {
        console.log('Unsubscribed from ' + topic + "__" + channel + '.');
        MessageQueue.sendAppMessage({
            "ACTION_STATUS": 1
        });
      },
      function (errorString) {
        console.log('Error subscribing to topic: ' + errorString);
        MessageQueue.sendAppMessage({
            "ACTION_STATUS": 0,
            "ACTION_ERROR": errorString
        });
      }
    );
}

function getStartFromShow(show){
    var startString = show.attributes.start;
    var newStartString = startString.substring(0, 4) + "/" + startString.substring(4, 6) + "/" + startString.substring(6, 8) + " " + startString.substring(8, 10) + ":" + startString.substring(10, 12);
    var start = new Date(newStartString);
    return start;
}

function getEndFromShow(show){
    var endString = show.attributes.stop;
    var newEndString = endString.substring(0, 4) + "/" + endString.substring(4, 6) + "/" + endString.substring(6, 8) + " " + endString.substring(8, 10) + ":" + endString.substring(10, 12);
    var end = new Date(newEndString);
    return end;
}

function fetch_all_shows_from_channel(channel_id){
    var xhr = new XMLHttpRequest();
    var url = "http://tv.edwinfinch.com:52382";
    xhr.open('POST', url, true);
    xhr.onload = function() {
        if(xhr.readyState == 4 && xhr.status == 200){
            var object = JSON.parse(xhr.responseText);
            var shows_array = object.programme;

            for(var i = 0; i < shows_array.length; i++){
                var show = shows_array[i];
                var d = new Date()
                var n = d.getTimezoneOffset();
                var object = {
                    "show_name":show.title,
                    "show_channel_id":show.attributes.channel,
                    "show_new":show.new ? 1 : 0,
                    "show_start":getStartFromShow(show).getTime()/1000,
                    "show_end":getEndFromShow(show).getTime()/1000
                };
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
    var url = "http://tv.edwinfinch.com:52382";
    xhr.open('POST', url, true);
    xhr.onload = function() {
        if(xhr.readyState == 4 && xhr.status == 200){
            var object = JSON.parse(xhr.responseText);
            var channels_array = object.channel;

            for(var i = 0; i < channels_array.length; i++){
                var channel = channels_array[i];
                var object = {
                    "channel_name":channel.display_name[2],
                    "channel_id":channel.attributes.id,
                    "channel_number":parseInt(channel.display_name[1])
                };
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
    console.log("I am ready. Thanks for checking on me");
}

function config_closed_handler(e){
    if (e.response !== undefined && e.response !== '' && e.response !== 'CANCELLED') {
		values = JSON.parse(decodeURIComponent(e.response));

        Pebble.sendAppMessage(values);
	}
}

function app_message_handler(e){
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
            subscribe_to_show(e.payload.show_name, e.payload.channel_name);
            break;
        //Show unsubscribe
        case 1004:
            unsubscribe_from_show(e.payload.show_name, e.payload.channel_name);
            break;
    }
}

function open_config_handler(e){
    Pebble.openURL("http://tv.edwinfinch.com/v2/support.php");
}

Pebble.addEventListener("ready", ready_handler);
Pebble.addEventListener("webviewclosed", config_closed_handler);
Pebble.addEventListener("appmessage", app_message_handler);
Pebble.addEventListener("showConfiguration", open_config_handler);
