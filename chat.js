var timer;
var username = null;

function format2Dig(value) {
    return ('0' + value).slice(-2);
}

function appendMessage(list, message) {
    message.text = message.text.replace('\\"', '"');

    var date = new Date(message.time * 1000);
    var format = format2Dig(date.getDate()) + '.' + format2Dig(date.getMonth() + 1) + '.' + date.getFullYear() + ' '
        + format2Dig(date.getHours()) + ':' + format2Dig(date.getMinutes()) + ':' + format2Dig(date.getSeconds());

    if (message.from == username) {
        list.append('<li><b>' + message.from + ' (' + format + '):</b> <pre>' + message.text + '</pre></li>');
    } else if (message.from == 'Admin') {
        list.append('<li><span class="admin-name">' + message.from + ' (' + format + '):</span> <pre>'
            + message.text + '</pre></li>');
    } else {
        list.append('<li>' + message.from + ' (' + format + '): <pre>' + message.text + '</pre></li>');
    }
}

function login(content) {
    $.ajax({
        url: '/login',
        method: 'POST',
        data: JSON.stringify({
            username: content
        }),
        contentType: 'application/json; charset=UTF-8',
        success: function () {
            username = content;

            var messageField = $('#message');
            $('#username').attr('disabled', true);
            $('#usernameEnter').attr('disabled', true);
            messageField.attr('disabled', false);
            $('#messageEnter').attr('disabled', false);
            messageField.focus();
            $('#indicator').attr('src', 'green_light.png');

            loadMessages(true)();
        }
    });
}

function loadMessages(all) {
    return function () {
        $.ajax({
            url: '/messages',
            method: 'GET',
            data: {
                username: username,
                all: all
            },
            dataType: 'json',
            success: function (data) {
                var messageList = $('#messages');
                for (var i = 0; i < data.messages.length; i++) {
                    appendMessage(messageList, data.messages[i]);
                }

                if (data.messages.length > 0) {
                    var messageBlock = $('#message-block');
                    var clientHeight = messageBlock.height();
                    var scrollHeight = messageBlock.get(0).scrollHeight;
                    if (clientHeight < scrollHeight) {
                        messageBlock.scrollTop(scrollHeight);
                    }
                }

                timer = setTimeout(loadMessages(false), 500);
            }
        });
    }
}

function sendMessage(message) {
    $.ajax({
        url: '/messages',
        method: 'POST',
        data: JSON.stringify({
            username: username,
            message: message
        }),
        contentType: 'application/json; charset=UTF-8',
        success: function () {
            clearTimeout(timer);
            loadMessages(false)();
        }
    });
}

function usernameEnter(event) {
    event.preventDefault();
    var usernameField = $('#username');
    var newUsername = usernameField.val();

    if (newUsername == 'Admin') {
        usernameField.val('');

        var unRed = function () {
            usernameField.removeClass('username-error');
        };
        usernameField.addClass('username-error');
        setTimeout(unRed, 200);
    } else if (newUsername != '') {
        login(newUsername);
    }
}

function messageEnter(event) {
    event.preventDefault();
    var messageField = $('#message');
    var message = messageField.val();
    if (message == '') {
        return;
    }

    messageField.val('');
    sendMessage(message);
}

$(document).ready(function () {
    $('#username').on('keydown', function (event) {
        if (event.keyCode == 13) {
            usernameEnter(event);
        }
    });

    $('#usernameEnter').on('click', function (event) {
        usernameEnter(event);
    });

    $('#message').on('keydown', function (event) {
        if (event.keyCode == 13 && !event.shiftKey) {
            messageEnter(event);
        }
    });

    $('#messageEnter').on('click', function (event) {
        messageEnter(event);
    });
});
