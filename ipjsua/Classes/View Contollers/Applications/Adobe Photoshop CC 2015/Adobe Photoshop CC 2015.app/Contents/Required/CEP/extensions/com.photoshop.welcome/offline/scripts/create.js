//  JavaScript Document  



// Do several inialization steps immediately after the DOM is ready
$(document).ready( function () {
	
	// Set the greeting
	var greetingString = parent.helloCtrlAndLangStringSet.find("helloGreeting").text();
	greetingString = greetingString.replace( "^1", parent.personalizationProperties["username"]);
	$('#helloGreeting').html( greetingString );
	
	// Set up the "new document" buttons
	$('#createNewTitle').html( parent.helloCtrlAndLangStringSet.find("createNewTitle").text() );
	var newDocTypes = parent.personalizationProperties["newDocTypes"];
	for ( i=0;i<newDocTypes.length;i++) {
		var docType = newDocTypes[i]["docType"];
		var docTypeName =  newDocTypes[i]["name"];
		$("#createNewContainer").append( '<a class="new_site_button" href="javascript:window.projectHello.createNewDoc(\'' + docType + '\', true )" style="text-decoration:none; color: #000;"><p> <img src="images/' + newDocTypes[i]["icon"] + '.png" style="position: relative; top: 2px; height: 15px;" /> ' + docTypeName + '</p></a>');
	}
	
	// Set up the "recent documents" buttons
	$('#openRecentTitle').html( parent.helloCtrlAndLangStringSet.find("openRecentTitle").text() );
	var localfiles = parent.personalizationProperties["localfiles"];
	for ( j=0;j<localfiles.length;j++) {
		var fullPath = localfiles[j]["fullpath"];
		var fileName =  localfiles[j]["name"];
		$("#openRecentContainer").append( '<a class="recent-file open_site_button" href="#" x-doc-path="' + fullPath + '" style="text-decoration:none; color: #000;"><p> <img src="images/' + localfiles[j]["icon"] + '.png" style="position: relative; top: 2px; height: 15px;" /> ' + fileName + '</p></a>');
	}
});