var timer;
var myId;

function scrollMessages() {
	var wd = $('#messages')[0].scrollHeight;
	$('#messages').parent().scrollTop(wd);
}

function appendMessage(messages, message) {
	if (message.from != 0) {
		console.log(message.from);
		console.log(myId);
		if (message.from == myId) 
			messages.append('<li><b>user'  + message.from + '</b>(' + message.timestamp  + '): ' + message.text + '</li>' );
		else
			messages.append('<li>user'  + message.from + '(' + message.timestamp  + '): ' + message.text + '</li>' );
	} else
		messages.append('<li><font color="gray"><i>System(' + message.timestamp  + '): ' + message.text + '</i></font></li>' );	
}

function loadMessages(all) {
	return function() {
		$.ajax({
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
		method: 'POST', //POST here
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