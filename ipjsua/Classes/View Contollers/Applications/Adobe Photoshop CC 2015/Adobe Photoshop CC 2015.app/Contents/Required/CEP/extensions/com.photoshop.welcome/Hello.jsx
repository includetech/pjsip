// Fri Nov 14 2014 13:58:00 GMT-0800 (Pacific Standard Time)

try {

function GetJSONData() {
    var welcomeInfo = GetWelcomeInfo();
    // Coerce other english varients to en_US
    var modLocale = app.locale;
    if (! (modLocale in {"en_US":1, "en_GB":1}))
    {
        if (modLocale.match(/en_??/))
            modLocale = "en_US";
    }
    var a = '{"appid":"1235",';
    a += '"appversion":"' + app.version + '",';
    a += '"platform":"' + (IsWindowsOS() ? "win" : "mac") + '",';
    a += '"language":"' + modLocale + '",';
    a += '"userTrackingEnabled":"' + welcomeInfo.userTrackingEnabled + '",';
    a += '"adobeid":"' + GetAdobeID() + '",';
    a += '"greeting":"' + welcomeInfo.greeting + '",';
    a += '"countryCode":"' + welcomeInfo.countryCode + '",';
    a += '"delegateGuid":"' + GetDelegateGUID() + '",';
    a += '"accountType":"' + welcomeInfo.accountType + '",';
    a += '"secondsLeftInTrial":"' + welcomeInfo.secondsLeftInTrial + '",';
    a += '"appStartClockTime":"' + welcomeInfo.appStartClockTime + '",';
    a += '"newDocTypes":[' + GetNewDocTypes() + '],';
    a += '"recentFiles":[' + GetRecentFiles() + '],';
    a += '"dontShowAgain":"' + GetDontShowAgain().toString() + '",';
    a += '"lastError":"' + welcomeInfo.lastError + '"}';
    return a;
}

function GetWelcomeInfo() {
    try {
        var w = {};
        w.userTrackingEnabled = true;
        w.greeting = "Hello!";
        w.accountType = "trial";
        w.secondsLeftInTrial = "1396292060";
        w.appStartClockTime =  "1396292061";
        w.lastError = "";
        w.countryCode = "US";
        var classApplication   = charIDToTypeID('capp');
        var typeOrdinal        = charIDToTypeID('Ordn');
        var enumTarget         = charIDToTypeID('Trgt');
        var welcome            = charIDToTypeID('wlcm');
        var classProperty      = charIDToTypeID("Prpr");
        var kentryStatusStr    = stringIDToTypeID("entryStatus");
        var kleftStr           = stringIDToTypeID("left");
        var kstartTimeStr      = stringIDToTypeID("startTime");
        var koptinStr          = stringIDToTypeID("optin");
        var kfirstStr          = stringIDToTypeID("first");
        var kisoCountryCodeStr = stringIDToTypeID("isoCountryCode");
        var ref = new ActionReference();
        ref.putProperty(classProperty, welcome);
        ref.putEnumerated(classApplication, typeOrdinal, enumTarget);
        var appDesc = app.executeActionGet(ref);
        if (appDesc.hasKey(welcome)) {
            welcomeDesc = appDesc.getObjectValue(welcome);
            if (welcomeDesc.count) {
                w.accountType = welcomeDesc.getInteger(kentryStatusStr) == 1 ? "trial" : "paid";
                w.secondsLeftInTrial = welcomeDesc.getLargeInteger(kleftStr);
                w.appStartClockTime = welcomeDesc.getLargeInteger(kstartTimeStr);
                w.userTrackingEnabled = welcomeDesc.getBoolean(koptinStr);
                w.greeting = "Hello, " + welcomeDesc.getString(kfirstStr) + "!";
                w.countryCode = welcomeDesc.getString(kisoCountryCodeStr);
            }
        }
    }
    catch(e) {
        ;
    }
    return w;
}

function IsWindowsOS() {
    if ( $.os.search(/windows/i) != -1 ) {
        return true;
    } else {
        return false;
    }
}

function GetDelegateGUID() {
    var s = "ERROR GUID";
    try {
        var classApplication = charIDToTypeID('capp');
        var typeOrdinal      = charIDToTypeID('Ordn');
        var enumTarget       = charIDToTypeID('Trgt');
        var behance          = charIDToTypeID('bhnc');
        var classProperty    = charIDToTypeID("Prpr");
        var ref = new ActionReference();
        ref.putProperty(classProperty, behance);
        ref.putEnumerated(classApplication, typeOrdinal, enumTarget);
        var appDesc = app.executeActionGet(ref);
        if (appDesc.hasKey(behance)) {
            s = appDesc.getString(behance).split("@")[0];
        }
    }
    catch(e) {
         s = "CATCH ERROR GUID:" + e.toString();
    }
    return s;

}

function GetAdobeID() {
    var s = "ERROR ID";
    try {
        var classApplication = charIDToTypeID('capp');
        var typeOrdinal      = charIDToTypeID('Ordn');
        var enumTarget       = charIDToTypeID('Trgt');
        var behance2         = charIDToTypeID('bhn2');
        var classProperty    = charIDToTypeID("Prpr");
        var ref = new ActionReference();
        ref.putProperty(classProperty, behance2);
        ref.putEnumerated(classApplication, typeOrdinal, enumTarget);
        var appDesc = app.executeActionGet(ref);
        if (appDesc.hasKey(behance2)) {
            s = appDesc.getString(behance2);
        }
    }
    catch(e) {
        s = "CATCH ERROR ID:" + e.toString();
    }
    return s;
}

// Populate the Create panel with some options
// NOTE: the docType must match the case statement in openDocumentOfType in photoshop.js
function GetNewDocTypes() {
    var s = '{"docType":"default","name":"Default Photoshop Size","icon":"psd"}';
    s += ', {"docType":"print","name":"Print Document","icon":"psd"}';
    s += ', {"docType":"photo","name":"Photo Document","icon":"psd"}';
    s += ', {"docType":"web","name":"Web Document","icon":"psd"}';
    s += ', {"docType":"mobile","name":"Mobile App Design Document","icon":"psd"}';
    s += ', {"docType":"iconography","name":"Iconography Document","icon":"psd"}';
    s += ', {"docType":"film","name":"Film & Video Document","icon":"psd"}';
    s += ', {"docType":"preset","name":"Document From Saved Preset","icon":"psd"}';
    s += ', {"docType":"custom","name":"Custom Document","icon":"psd"}';
    return s;
}

function GetRecentFiles() {
    var s = '';
    var ref = new ActionReference();
    var keyrecentFilesAsStrings = stringIDToTypeID("recentFilesAsStrings");
    var classProperty = charIDToTypeID( "Prpr" );
    ref.putProperty(classProperty, keyrecentFilesAsStrings);
    ref.putEnumerated(charIDToTypeID("capp"), charIDToTypeID("Ordn"), charIDToTypeID("Trgt"));
    var d = executeActionGet(ref);  
    if (d.count && d.hasKey(keyrecentFilesAsStrings)) {
        var l = d.getList(keyrecentFilesAsStrings);
        var lc = l.count < 10 ? l.count : 10;
        for (var i = 0; i < lc; i++) {
            var item = File(l.getString(i));
            s += '{"fullpath":"';
            s += item.fullName;
            s += '","name":"';
            s += unescape(item.name);
            s += '","icon":"psd"}'
            if (i < lc - 1)
                s += ',';
        }
        s += ',{"fullpath":"open","name":"Open...","icon":"openicon"}';
    } else {
        s += '{"fullpath":"open","name":"Open...","icon":"openicon"}';
    }
    return s;
}

function SetDontShowAgain(inValue) {
    try {
        var desc = new ActionDescriptor();
        desc.putBoolean( stringIDToTypeID( "show" ), ! inValue );
        app.putCustomOptions( "0685b2e9-19db-4679-90b9-34644fcc7884", desc, true );
    } 
    catch(e) { 
        ;
    }
}

function GetDontShowAgain() {
    var returnValue = false;
    try {
        var desc = app.getCustomOptions( "0685b2e9-19db-4679-90b9-34644fcc7884" );
        var kshowStr = stringIDToTypeID( "show" );
        if ( desc.count > 0 && desc.hasKey( kshowStr ) ) {
            returnValue = ! desc.getBoolean( kshowStr );
        }
    }
    catch(e) {
        returnValue = false;
    }
    return returnValue;
}

} catch(e) {
    ;
}
// end Hello.jsx
