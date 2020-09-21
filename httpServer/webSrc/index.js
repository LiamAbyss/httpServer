$("document").ready(function(){

	console.log($someVariable);

	$("#farfetchd").mouseenter(function(){
		$("#farfetchd").css("backgroundColor", "red");
	});
	$("#farfetchd").mouseleave(function(){
		$("#farfetchd").css("backgroundColor", "");
	});
});