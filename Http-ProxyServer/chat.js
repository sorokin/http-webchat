var timer;
var username = null;

function scrollMessages() {
    var wd = $('#messages')[0].scrollHeight;
    $('#messages').parent().scrollTop(wd);
}

function wrap(val, nums) {
    var ret = "" + val;
    while (ret.length < nums) ret = "0" + ret;
    return ret;
}

function appendMessage(messages, message) {
    message.text = decodeURIComponent(message.text);
    while (message.text.indexOf("+") != -1)
        message.text = message.text.replace("+", " ");

    var date = new Date(message.timestamp * 1000);
    var format = wrap(date.getDate(), 2) + "-" + wrap(date.getMonth() + 1, 2) + "-" + wrap(date.getYear() % 100, 2) +
        " " + wrap(date.getHours(), 2) + ":" + wrap(date.getMinutes(), 2) + ":" + wrap(date.getSeconds(), 2);
    if (message.from != 0) {
        if (message.from == username) {
            messages.append('<li><b>user' + message.from + '</b>(' + format + '): ' + message.text + '</li>');
        } else {
            messages.append('<li>user' + message.from + '(' + format + '): ' + message.text + '</li>');
        }
    } else {
        messages.append('<li><font color="gray"><i>System(' + format + '): ' + message.text + '</i></font></li>');
    }
}

function login(content) {
    $.ajax({
        url: '/login',
        method: 'POST',
        data: {
            username: content
        },
        success: function() {
            username = content;
            loadMessages(true)();
        }
    })
}

function loadMessages(all) {
    return function () {
        $.ajax({
            url: '/messages',
            method: 'GET',
            headers: {'Content-Type' : 'application/json; charset=utf-8'},
            data: {
                username: username,
                all: all
            },
            dataType: 'json',
            success: function(data) {
                for (var i = 0; i < data.messages.length; ++i)
                    appendMessage($('#messages'), data.messages[i]);
                if (data.messages.length != 0)
                    scrollMessages();
                timer = setTimeout(loadMessages(false), 500);
            }
        });
    }
}

function sendMessage(message) {
    $.ajax({
        url: '/messages?username=' + username,
        method: 'POST',
        headers: {'Content-Type': 'application/json; charset=utf-8'},
        data: {
            message: message
        },
        dataType: 'json',
        success: function (response) {
        }
    });
    clearTimeout(timer);
    loadMessages(false)();
}

$(document).ready(function () {
    $(document).on('keydown', function(event) {
        if (event.keyCode == 13) {
            event.preventDefault();
            var content = $(this).find('input').val();
            if (content === '')
                return;
            $(this).find('input').val('');

            if (username != null) {
                sendMessage(content);
            } else {
                login(content);
            }
        }
    });
});