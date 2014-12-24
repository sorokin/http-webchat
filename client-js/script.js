function scrollAss() {
	var wd = $('#messages')[0].scrollHeight;
	$('#messages').parent().scrollTop(wd);
}

function loadData() {
	$.ajax({
		url: '/data', 
		method: 'GET',
		dataType: 'json',
		success: function(data) {
			console.log(data);
			$('#messages').append('<li>' + data.response + (new Date()).getTime() +'</li>');
			scrollAss();		
			setTimeout(loadData, 100);
		}
	});
}

function sendMessage(message) {
	$('#messages').append('<li>'+(new Date()).getTime()+' - '+ message+'</li>');
	scrollAss();
	$.ajax({
		url: '/messages', 
		method: 'POST', //POST here
		data: {
			'message': message,
			'to': 1
		},
		dataType: 'json',
		success: function(response) {
			$('#messages').append('<li>' + data.response + (new Date()).getTime()+'</li>');
		}
	});
}

$(document).ready(function () {
	$('#myForm').on('submit', function(event) {
		event.preventDefault();
		
		var message = $(this).find('input').val();
		$(this).find('input').val('');
		sendMessage(message);
		console.log(message);
	});
	//loadData();
});