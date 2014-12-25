var timer;
var myId;

function scrollMessages() {
	var wd = $('#messages')[0].scrollHeight;
	$('#messages').parent().scrollTop(wd);
}

function wrap(val, nums) {
	var ret = "" + val;
	while (ret.length < nums) ret = "0" + ret;
	console.log(ret);
	return ret;
}

function appendMessage(messages, message) {
	message.text = decodeURIComponent(message.text);
	while (message.text.indexOf("+") != -1)
		message.text = message.text.replace("+", " ");

	var date = new Date(message.timestamp * 1000);
	var format =  wrap(date.getDay(), 2) + "-" + wrap(date.getMonth(), 2) + "-" + wrap(date.getYear() % 100, 2) + 
	" " + wrap(date.getHours(), 2) + ":" + wrap(date.getMinutes(), 2) + ":" + wrap(date.getSeconds(), 2);
	if (message.from != 0) {
		if (message.from == myId) 
			messages.append('<li><b>user'  + message.from + '</b>(' + format  + '): ' + message.text + '</li>' );
		else
			messages.append('<li>user'  + message.from + '(' + format  + '): ' + message.text + '</li>' );
	} else
		messages.append('<li><font color="gray"><i>System(' + format  + '): ' + message.text + '</i></font></li>' );	
}

function loadMessages(all) {
	return function() {
		$.ajax({
			//headers: {'Content-Type' : 'application/json; charset=utf-8'},
			url: '/messages', 
			method: 'GET',
			dataType: 'json',
			data: {
				all: all
			},
			success: function(data) {
				for (var i = 0; i < data.messages.length; ++i)
					appendMessage($('#messages'), data.messages[i]);
				//$('#messages').append('<li>' + data.response + (new Date()).getTime() +'</li>');
				if (data.messages.length != 0) 
					scrollMessages();		
				timer = setTimeout(loadMessages(false), 500);
			}
		});
	}
}

function sendMessage(message) {
	$.ajax({
		url: '/messages', 
		method: 'POST',
		headers: {'Content-Type' : 'text/html; charset=utf-8'},
		data: {
			message: message
		},
		dataType: 'json',
		success: function(response) {
		}
	});
	clearTimeout(timer);
	loadMessages(false)();
}

$(document).ready(function () {
	myId = parseInt(document.cookie.substring(5, document.cookie.indexOf(";")));
	$('#myForm').on('submit', function(event) {
		event.preventDefault();
		
		var message = $(this).find('input').val();
		if (message === '')
			return;
		$(this).find('input').val('');
		sendMessage(message);
		console.log(message);
	});
	loadMessages(true)();
});