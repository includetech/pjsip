// Tue May 12 2015 15:12:51 GMT-0700 (Pacific Daylight Time)

var docSetup = {};

function onLoaded() {
    window.projectHello = {
							hostAppGetPersonalizationInfo           : getUserJsonData,
							hostAppOpenDoc                          : openExistingDocument,
							hostAppCreateNewDoc                     : openDocumentOfType,
							hostAppSetDoNotShowAgainPreference      : setHelloDontShowAgainPreference,
							hostAppRetryLoadFromServer              : retryLoadFromServer,
							hostAppNotifyDoneWithUI                 : notifyDoneWithDialog,
							hostAppNotifyFailedToLoad               : notifyFailedToLoad,
							openURLInDefaultBrowser                 : openLinkInDefaultBrowser,
							hostAppNotifyInContentTutorial			: notifyInContentTutorial,
                            hostAppNotifyContentTabChanged          : notifyContentTabChanged
							};

	docSetup.default =     { width: 504,
                             height: 360,
                             name: 'Default Photoshop Size',
                             resolution: 300,
                             profile: undefined };
	docSetup.print =       { width: 612,
                             height: 792,
                             name: 'Print Document',
                             resolution: 300,
                             profile: undefined };
	docSetup.photo =       { width: 360,
                             height: 504,
                             name: 'Photo Document',
                             resolution: 300,
                             profile: undefined };
	docSetup.web =         { width: 691.2,
                             height: 388.8,
                             name: 'Web Document',
                             resolution: 200,
                             profile: undefined,
                             artboard: true };
	docSetup.mobile =      { width: 165.6442,
                             height: 294.6258,
                             name: 'Mobile App Design Document',
                             resolution: 326,
                             profile: 'sRGB IEC61966-2.1',
                             artboard: true };
	docSetup.iconography = { width: 26.503,
                             height: 26.503,
                             name: 'Iconography Document',
                             resolution: 326,
                             profile: 'sRGB IEC61966-2.1' };
	docSetup.film =        { width: 504,
                             height: 360,
                             name: 'Film & Video Document',
                             resolution: 300,
                             profile: 'sRGB IEC61966-2.1' };
	docSetup.preset =      { width: undefined,
                             height: undefined,
                             name: 'Document From Saved Preset',
                             resolution: undefined,
                             profile: undefined };
	docSetup.custom =      { width: undefined,
                             height: undefined,
                             name: 'Custom Document',
                             resolution: undefined,
                             profile: undefined };

}

function getUserJsonData(callback) {
	try {
		evalScript("GetJSONData()", callback);
	} catch(e) {
		alert('JSON request catch: ' + e.message);
	}
 }

function evalScript(script, callback) {
	if (callback === null || callback === undefined) {
        callback = function(result){};
    }
    if (window.__adobe_cep__) {
		window.__adobe_cep__.evalScript(script, callback);
    } else {
        callback(null);
    }
}

function closeHello() {
	if (window.__adobe_cep__) {
		window.__adobe_cep__.closeExtension();
	}
}

function sendaboutToCloseEvents() {
	sendEvent("com.adobe.photoshop.welcome.aboutToAutomaticallyCloseHello", "");
}

function openExistingDocument(fullPath) {
    if (fullPath === "open") {
		var result = window.cep.fs.showOpenDialog();
		if (result && result.data.length) {
			var temp = result.data[0];
			if (IsWindowsOS()) {
				fullPath = '/' + temp[0] + temp.substr(2, temp.length);
			} else {
				fullPath = temp;
			}
		} else {
			fullPath = undefined;
		}
	}
	if ( typeof fullPath !== "undefined" ) {
		evalScript('try { if (File("' + fullPath + '").exists) { var descOpen = new ActionDescriptor(); descOpen.putPath(stringIDToTypeID("null"), new File("' + fullPath + '")); descOpen.putBoolean(stringIDToTypeID("forceRecording"), true); executeAction(stringIDToTypeID("open"), descOpen, DialogModes.ERROR); } } catch(e) { alert(e + ":" + e.line); } ', localAppOpen);
		logPIPEvent("ProjectHello", "Interaction", "OpenExistingDoc");
	}
}

function localAppOpen (result) {
	sendaboutToCloseEvents();
	closeHello();
}

function openDocumentOfType (documentType, suppressNewDocUI) {
	script = generateScriptForNewDoc(docSetup, documentType);
	evalScript(script, newDocResult);
}

function newDocResult(inResult) {
	logPIPEvent("ProjectHello", "Interaction", "CreateNewDoc:" + inResult);
	sendaboutToCloseEvents();
    closeHello();
}

function generateScriptForNewDoc(inObj, inPreset) {
	var script = "try { returnResult = '" + inPreset + "';";
	if (inPreset === "preset" || inPreset === "custom") {
		script += "app.displayDialogs = DialogModes.ALL;";
	} else {
		script += "app.displayDialogs = DialogModes.NO;";
	}
	var obj = inObj[inPreset];
	script += 'idMk = charIDToTypeID( "Mk  " );';
    script += 'desc1 = new ActionDescriptor();';
    script += 'idNw = charIDToTypeID( "Nw  " );';
    script += 'desc2 = new ActionDescriptor();';
    script += 'idNm = charIDToTypeID( "Nm  " );';
    script += 'desc2.putString( idNm, "' + obj.name + '");';
    script += 'idMd = charIDToTypeID( "Md  " );';
    script += 'idRGBM = charIDToTypeID( "RGBM" );';
    script += 'desc2.putClass( idMd, idRGBM );';
    script += 'unitPixels = charIDToTypeID( "#Pxl" );';
	script += 'idWdth = charIDToTypeID( "Wdth" );';
    if (obj.width !== undefined) {
		script += 'desc2.putUnitDouble( idWdth, unitPixels, ' + obj.width + ');';
    }
	script += 'idHght = charIDToTypeID( "Hght" );';
    if (obj.height !== undefined) {
		script += 'desc2.putUnitDouble( idHght, unitPixels, ' + obj.height + ');';
    }
	script += 'idRslt = charIDToTypeID( "Rslt" );';
    if (obj.resolution !== undefined) {
		script += 'desc2.putUnitDouble( idRslt, unitPixels, ' + obj.resolution + ');';
    }
    script += 'idpixelScaleFactor = stringIDToTypeID( "pixelScaleFactor" );';
    script += 'desc2.putDouble( idpixelScaleFactor, 1.000000 );';
    script += 'idFl = charIDToTypeID( "Fl  " );';
	script += 'idWht = charIDToTypeID( "Wht " );';
	script += 'desc2.putEnumerated( idFl, idFl, idWht );';
	script += 'idDpth = charIDToTypeID( "Dpth" );';
    script += 'desc2.putInteger( idDpth, 8 );';
    if (obj.profile !== undefined) {
		script += 'idprofile = stringIDToTypeID( "profile" );';
		script += 'desc2.putString( idprofile, "' + obj.profile + '");';
    }
    if (obj.artboard !== undefined && obj.artboard === true) {
        script += 'idfill = stringIDToTypeID( "fill" );';
        script += 'idtransparency = stringIDToTypeID( "transparency" );';
        script += 'desc2.putEnumerated( idfill, idfill, idtransparency );';
        script += 'kartboardListStr = stringIDToTypeID( "artboardList" );';
        script += 'idartboardRect = stringIDToTypeID( "artboardRect" );';
        script += 'desc39 = new ActionDescriptor();';
        script += 'idtop = stringIDToTypeID( "top" );';
        script += 'desc39.putDouble( idtop, 0.000000 );';
        script += 'idleft = stringIDToTypeID( "left" );';
        script += 'desc39.putDouble( idleft, 0.000000 );';
        script += 'idbottom = stringIDToTypeID( "bottom" );';
        script += 'desc39.putDouble( idbottom, "' + ( obj.height * obj.resolution / 72.0 ) + '");';
        script += 'idright = stringIDToTypeID( "right" );';
        script += 'desc39.putDouble( idright, "' + ( obj.width * obj.resolution / 72.0 ) + '");';
        script += 'idclassFloatRect = stringIDToTypeID( "classFloatRect" );';
        script += 'desc38 = new ActionDescriptor();';
        script += 'desc38.putObject( idartboardRect, idclassFloatRect, desc39 );';
        script += 'artboardList = new ActionList();';
        script += 'artboardList.putObject( kartboardListStr, desc38 );';
        script += 'desc2.putList( kartboardListStr, artboardList );';
    }
    script += 'idDcmn = charIDToTypeID( "Dcmn" );';
    script += 'desc1.putObject( idNw, idDcmn, desc2 );';
	script += 'resultDesc = executeAction( idMk, desc1, app.displayDialogs );';
	script += 'if (app.displayDialogs == DialogModes.ALL && resultDesc.count && resultDesc.hasKey( idNw )) {';
	script += '     objDesc = resultDesc.getObjectValue( idNw );';
	script += '     idpreset = stringIDToTypeID( "preset" );';
	script += '     if (objDesc.count && objDesc.hasKey( idpreset )) {';
	script += '          returnResult = objDesc.getString( idpreset );';
	script += '     }';
	script += '     if (objDesc.count && objDesc.hasKey( idWdth ) && objDesc.hasKey( idHght ) && objDesc.hasKey( idRslt )) {';
	script += '          returnResult = objDesc.getUnitDoubleValue( idWdth );';
	script += '          returnResult += "x" + objDesc.getUnitDoubleValue( idHght );';
	script += '          returnResult += "x" + objDesc.getUnitDoubleValue( idRslt );';
	script += '     }';
	script += '}';
	script += "returnResult; } catch(e) { 'ERROR'; }";
	return script;
}

function setHelloDontShowAgainPreference (dontShowAgain) {
	if (dontShowAgain) {
		logPIPEvent("ProjectHello", "Interaction", "SetDoNotShowAgain");
	} else {
		logPIPEvent("ProjectHello", "Interaction", "ClearDoNotShowAgain");
	}
	var trueOrFalse = dontShowAgain ? "true" : "false";
	var script = "SetDontShowAgain(" + trueOrFalse + ");";
	evalScript(script);
}

function retryLoadFromServer() {
	logPIPEvent("ProjectHello", "Interaction", "RetryPageLoad");
	if (window.navigator.onLine === true) {
		window.location = "https://helpx.adobe.com/photoshop/hello/v1_6/index.html";
	}
}

function notifyDoneWithDialog() {
    if (window.__adobe_cep__) {
		window.__adobe_cep__.closeExtension();
    }
}

function notifyFailedToLoad(errorString) {
	if (errorString) {
		logPIPEvent("ProjectHello", "Failure", errorString);
	} else {
		logPIPEvent("ProjectHello", "Failure", "No error string");
	}
}

function logPIPEvent(category, subCategory, eventName) {
	sendEvent("com.adobe.PhotoshopLOGPIP", category + "," + subCategory + "," + eventName);
}

function sendRecentFileOpenEvent(fileName) {
	sendEvent("com.adobe.photoshop.welcome.HandleRecentFileOpen", fileName);
}

function openLinkInDefaultBrowser(url) {
	if (window.cep.util.openURLInDefaultBrowser) {
		window.cep.util.openURLInDefaultBrowser(url);
		logPIPEvent("ProjectHello", "Interaction", "OpenExtLink:" + url);
	}
}

function sendEvent(eventType, eventData) {
	var csInterface = new CSInterface();                   
	var event = new CSEvent();
	event.type = eventType;
	event.scope = "APPLICATION";
	event.data = eventData;
	event.appId = "PHXS";
	event.extensionId = "com.photoshop.Welcome.extension1";
	csInterface.dispatchEvent(event);  
}

function notifyInContentTutorial(isInContentTutorial) {
	sendEvent("com.adobe.photoshop.welcome.isInContentTutorialNotification", (isInContentTutorial ? "Yes" : "No"));
}

function notifyContentTabChanged(currentTab) {
	sendEvent("com.adobe.photoshop.welcome.contentTabChangedNotification", "");
}

function IsWindowsOS() {
    var isWindows = true;
    if (navigator.userAgent.indexOf('Mac OS X') != -1) {
        isWindows = false;
    }
    return isWindows;
}

// end photoshop.jsx
