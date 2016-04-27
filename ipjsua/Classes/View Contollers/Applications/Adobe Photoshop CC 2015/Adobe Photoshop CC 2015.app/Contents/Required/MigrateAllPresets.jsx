////////////////////////////////////////////////////////////////////////////
// ADOBE SYSTEMS INCORPORATED
// Copyright 2010 Adobe Systems Incorporated
// All Rights Reserved

// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the
// terms of the Adobe license agreement accompanying it.  If you have received this file from a
// source other than Adobe, then your use, modification, or distribution of it requires the prior
// written permission of Adobe.

// Adobe Photoshop QE / DI Pro Tools Team
// Email: DL-PS-External-Bugs-Tools@adobe.com
// Script File: MigrateAllPresets.jsx
// Script Dev: Kaori Mikawa, Barkin Aygun
// Script QE: Kyoko Itoda, Irina Satanovskaya

// This script carries over your presets and workspaces files from CC, CS6, CS5.1, CS5, CS4 and CS3 in order,
// not picking up already existing ones. 
// It does it quietly, and within Photoshop, is designed to run in first launch
/////////////////////////////////////////////////////////////////////////////
/*
// BEGIN__HARVEST_EXCEPTION_ZSTRING
<javascriptresource>
<name>$$$/JavaScripts/MigratePresets/Menu=Migrate Presets...</name>
<menu>help</menu>
</javascriptresource>
// END__HARVEST_EXCEPTION_ZSTRING
*/
#target photoshop;
$.localize = true;

try{ 
    app.bringToFront();
    app.displayDialogs = DialogModes.NO;
    
    var migrationComplete = false;
    var errorToQuit;
    
    // try to use variables to rev version.  Still need to search for these CC%20 or the CC YYYY below in zstring format 
    var versioncurrentsuite = "CC 2015";
    var versioncurrentps = 16;
    var versionstrs = new Array("CC 2014", "CC", "CS6", "CS5.1", "CS5", "CS4", "CS3");
    var versionflags = new Array();
    for (i = 0; i < versionstrs.length; i++) {
            versionflags.push(false);
    }
    
    const kUserCancelError = "cancel";
    
    var activesetfiles = new Array("Brushes.psp","Swatches.psp","Gradients.psp",
    							"Styles.psp","Patterns.psp","Contours.psp",
    							"CustomShapes.psp","Actions Palette.psp",
    							"ToolPresets.psp","Default Type Styles.psp");
    
    //ZSTRING
    var titleFileMigration = localize("$$$/MigratePresets/DialogTitle=Migrate Presets");
    var msgCompleteError = localize("$$$/MigratePresets/MessageError=An error occurred when migrating the presets. Preset migration failed.");
    var msgAdmin = localize("$$$/MigratePresets/MessageAdmin=Administrative privileges are required to migrate these presets.");
    var msgWrongVersion = localize("$$$/MigratePresets/MessageWrongVersion=This script is intended for Photoshop CC 2015. Launch Photoshop CC 2015 and try again.");
    var noFileSelected = localize("$$$/MigratePresets/MessageNoFile=No file is selected to migrate.");
    var msgMigrate =localize("$$$/MigratePresets/MessageConfirm=Would you like to migrate presets from the following versions?^n");
    var titleConfirmDialog = localize("$$$/MigratePresets/ConfirmTitle=Migrate Presets From Previous Versions of Adobe Photoshop");
    var determiningProgressText = localize("$$$/MigratePresets/MessageDetermining=Determining which presets to migrate");
    var migratingProgressText = localize("$$$/MigratePresets/MessageMigrating=Migrating presets");
    var searchingProgressText = localize("$$$/MigratePresets/MessageSearching=Searching for presets to migrate");
    
    var curOS = getCurOS($.os);
    var appVer = getAppVer();
    var dirCommonFiles = getDirCommonFiles(); 
    var dirUserData = getDirUserData();
    var dirUserPreferencesMac = getDirUserPreferencesMac();
    
    var dirUserPresets = new Folder(dirUserData + "/Adobe/Adobe Photoshop " + versioncurrentsuite + "/Presets");
	var dirUserSettings = new Folder();
    var dirUserWorkspaces = new Folder();
    var dirUserWorkspacesModified = new Folder();
    if(curOS.match("mac")){
    	dirUserSettings = new Folder(dirUserPreferencesMac + "/Adobe Photoshop " + versioncurrentsuite + " Settings");
        dirUserWorkspaces = new Folder(dirUserPreferencesMac + "/Adobe Photoshop " + versioncurrentsuite + " Settings/WorkSpaces");
        dirUserWorkspacesModified = new Folder(dirUserPreferencesMac + "/Adobe Photoshop " + versioncurrentsuite + " Settings/WorkSpaces (Modified)");
    }else{
    	dirUserSettings = new Folder(dirUserData + "/Adobe/Adobe Photoshop " + versioncurrentsuite + "/Adobe Photoshop " + versioncurrentsuite + " Settings");
        dirUserWorkspaces = new Folder(dirUserData + "/Adobe/Adobe Photoshop " + versioncurrentsuite + "/Adobe Photoshop " + versioncurrentsuite + " Settings/WorkSpaces");
        dirUserWorkspacesModified = new Folder(dirUserData + "/Adobe/Adobe Photoshop " + versioncurrentsuite + "/Adobe Photoshop " + versioncurrentsuite + " Settings/WorkSpaces (Modified)");
    }
   
    var fileItems = new Array();
    var fileToMigrate = new Array();
    var objFolderName = "";
        
    // -----------------------------------------
    // User Presets
    // -----------------------------------------
    var arrayUserFolderPresetsTo = new Array();
    var arrayUserFolderPresetsFrom = new Array();
    var arrayUserFolderPresetsDiff = new Array();
    var arrayUserFolderPresetsDiffDont = new Array();
    var userFolderPresetsDiffTotal = 0;
    // -----------------------------------------
    // User Workspaces
    // -----------------------------------------
    var arrayUserFolderWorkspacesTo = new Array();
    var arrayUserFolderWorkspacesFrom = new Array();
    var arrayUserFolderWorkspacesDiff = new Array();
    var arrayUserFolderWorkspacesDiffDont = new Array();
    var userFolderWorkspacesDiffTotal = 0;
    // -----------------------------------------
    // User Workspaces
    // -----------------------------------------
    var arrayUserFolderWorkspacesModifiedTo = new Array();
    var arrayUserFolderWorkspacesModifiedFrom = new Array();
    var arrayUserFolderWorkspacesModifiedDiff = new Array();
    var arrayUserFolderWorkspacesModifiedDiffDont = new Array();
    var userFolderWorkspacesModifiedDiffTotal = 0;
    // -----------------------------------------
    // Settings
    // -----------------------------------------
	var arraySettingsTo = new Array();
	var arraySettingsFrom = new Array();
	var arraySettingsDiff = new Array();
	var arraySettingsDiffDont = new Array();
	var userFolderSettingsDiffTotal = 0;
	
    var result = "success";
    
    // Eliminate versions w/out settings folder from our list so we don't allot part of the progress bar to them.
    quickCheckSettingsAvailable();
    
    var migrateResult = new Object();
    app.doProgress(searchingProgressText, "checkMigrateIsNecessary(migrateResult)");
    
    if (migrateResult.migrateNecessary) {
        if(appVer == versioncurrentps){
            for (i = 0; i < versionstrs.length; i++){
                if (versionflags[i] == true) {
                    msgMigrate = msgMigrate + "Adobe Photoshop " + versionstrs[i] +"\n";
                } 
            }
            if (confirm(msgMigrate, false, titleConfirmDialog)) {
            	// Force the progress to show immediately because we may be copying very
            	// large files, during which PS does not get a chance to pop the dialog
            	// after the normal delay.
            	app.doForcedProgress(determiningProgressText, "migrateAll(versioncurrentsuite)");
        	} else {
        		result = "failure";
        	}
        }else{
            alert(msgWrongVersion, titleFileMigration);
        }
    } else {
    	// Exit quietly. PS will inform the user that no presets were migrated based on
    	// this return value.
        result = "nothing";
    }
    result;
        
}catch(e){
	if (e != kUserCancelError)
		alertScriptError("Line: " + e.line +" - "+ e);
}

/****************************************
 * dirUserSettingFromForVersion
 ****************************************/
function dirUserSettingFromForVersion(versionstr)
{
	return new Folder(dirUserSettings.toString().replace(/CC%202015/g,versionstr));
}

/****************************************
 * quickCheckSettingsAvailable
 * Does a quick check to determine which versions have settings folders available. We will skip
 * all other versions.
 ****************************************/
function quickCheckSettingsAvailable()
{
    for (i = 0; i < versionstrs.length; i++){
    	if (dirUserSettingFromForVersion(versionstrs[i]).exists)
            versionflags[i] = true;
    }
}

/****************************************
 * countTrueVersionFlags
 ****************************************/
function countTrueVersionFlags()
{
	var count = 0;
    for (i = 0; i < versionstrs.length; i++){
		if (versionflags[i] == true)
			count++;
	}
	return count;
}

/****************************************
 * checkMigrateIsNecessary
 ****************************************/
function checkMigrateIsNecessary(outResult)
{
	var currentTask = 0;
	var totalTasks = countTrueVersionFlags();
	
    outResult.migrateNecessary= false;
    for (i = 0; i < versionstrs.length; i++){
    	// Some versions may have already be eliminated by quickCheckSettingsAvailable()
		if (versionflags[i] == false)
			continue;
			
        if (!app.doProgressSubTask(currentTask++, totalTasks, "refreshDiffData(versionstrs[i], true)"))
        	throw kUserCancelError;
        
        if (arrayUserFolderPresetsDiff.length == 0 && 
        	arrayUserFolderWorkspacesDiff.length == 0 &&
        	arrayUserFolderWorkspacesModifiedDiff.length == 0 &&
        	arraySettingsDiff.length == 0) {
            versionflags[i] = false;
        } else {
            versionflags[i] = true;
            outResult.migrateNecessary = true;
            
			// quickCheckSettingsAvailable() may have set multiple version flags to true, but we
			// really only want to offer migration from the most last installed version before
			// this version, so clear all the remaining version flags
            for (j = (i + 1); j < versionstrs.length; j++)
            	versionflags[j] = false;
            
            break; // this should break at first possible sync
        }
    }
}

/****************************************
 * migrateAll
 ****************************************/
function migrateAll (versionstrcur){
	var currentTask = 0;
	var totalTasks = countTrueVersionFlags();
	for (i = 0; i < versionstrs.length; i++) {
		if (versionflags[i] == true) {
			app.doProgressSubTask(currentTask++, totalTasks, "migrateAllForVersion(versionstrs[i], versionstrcur)");
		}
	}
}
	
/****************************************
 * migrateAllForVersion
 ****************************************/
function migrateAllForVersion(versionstr, versionstrcur){
   	// We will allot task segment length unevenly, giving the majority to getting the diff data
   	// and copying the user presets because those are the ones that are most like to be big tasks.
   	var doneTasks = 0;
   	var refreshTaskLen = 4, copyPresetsLen = 3, copyWSLen = 1, copyWSMLen = 1, copySettingsLen = 1;
   	var totalTaskLengths = refreshTaskLen + copyPresetsLen + copyWSLen + copyWSMLen + copySettingsLen;
    if (!app.doProgressSegmentTask(refreshTaskLen, doneTasks, totalTaskLengths, "refreshDiffData(versionstr, false)"))
    	throw kUserCancelError;
	
	doneTasks += refreshTaskLen;
	
	if (arrayUserFolderPresetsDiff.length == 0 && 
    	arrayUserFolderWorkspacesDiff.length == 0 &&
        arrayUserFolderWorkspacesModifiedDiff.length == 0 &&
        arraySettingsDiff.length == 0) {
        return;
    }
    
	app.changeProgressText(migratingProgressText);
	
    if (!app.doProgressSegmentTask(copyPresetsLen, doneTasks, totalTaskLengths, "migrateFiles(arrayUserFolderPresetsDiff, versionstr, versionstrcur)"))
    	throw kUserCancelError;
    
    doneTasks += copyPresetsLen;
    
    if (!app.doProgressSegmentTask(copyWSLen, doneTasks, totalTaskLengths, "migrateFiles(arrayUserFolderWorkspacesDiff, versionstr, versionstrcur)"))
    	throw kUserCancelError;
    
    doneTasks += copyWSLen;
    
    if (!app.doProgressSegmentTask(copyWSMLen, doneTasks, totalTaskLengths, "migrateFiles(arrayUserFolderWorkspacesModifiedDiff, versionstr, versionstrcur)"))
    	throw kUserCancelError;
    
    doneTasks += copyWSMLen;
    
    if (!app.doProgressSegmentTask(copySettingsLen, doneTasks, totalTaskLengths, "migrateFiles(arraySettingsDiff, versionstr, versionstrcur)"))
    	throw kUserCancelError;
    
    doneTasks += copySettingsLen;
}


/****************************************
 * refreshListBoxes
 ****************************************/
function refreshDiffData(versionstr, justCheck){
   	// We will allot task segment length unevenly, giving the majority to checking user presets
   	// because it is the one that is most like to be a big task.
   	var doneTasks = 0;
   	var totalTaskLengths = 10;
   	
	var resultObj = new Object();
	
	if (!app.doProgressSegmentTask(1, doneTasks++, totalTaskLengths, "refreshDiffDataSettingsFiles(versionstr, justCheck, resultObj)"))
		throw kUserCancelError;
	
	// if settings folder for an app does not exist, we shouldn't check the other folders as that app isn't "installed"
   	if (!resultObj.folderExists)
		return;
		
	if (arraySettingsDiff.length > 0 && justCheck)
		return;
		
	if (!app.doProgressSegmentTask(1, doneTasks++, totalTaskLengths, "refreshDiffDataUserWorkspaces(versionstr,justCheck)"))
		throw kUserCancelError;
	
	if (arrayUserFolderWorkspacesDiff.length > 0 && justCheck)
		return;
	
	if (!app.doProgressSegmentTask(1, doneTasks++, totalTaskLengths, "refreshDiffDataUserWorkspacesModified(versionstr,justCheck)"))
		throw kUserCancelError;
	
	if (arrayUserFolderWorkspacesModifiedDiff.length > 0 && justCheck)
		return;
		
	if (!app.doProgressSegmentTask(7, doneTasks, totalTaskLengths, "refreshDiffDataUserPresets(versionstr,justCheck)"))
		throw kUserCancelError;
	
	if (arrayUserFolderPresetsDiff.length > 0 && justCheck)
		return;
}


function refreshDiffDataSettingsFiles(versionstr, justCheck, outResult) {

     dirUserSettingsFrom = dirUserSettingFromForVersion(versionstr);
     
     outResult.folderExists = dirUserSettingsFrom.exists;
     
     if (dirUserSettingsFrom.exists)
   		{
		arraySettingsTo = getSettingsContents(dirUserSettings);
		arraySettingsFrom = getSettingsContents(dirUserSettingsFrom)
		arraySettingsDiff = getMissingFiles(arraySettingsFrom, arraySettingsTo, justCheck);
		arraySettingsDiffDont = new Array();
		}
}

function refreshDiffDataUserWorkspaces(versionstr, justCheck) {

    dirUserWorkspacesFrom = new Folder(dirUserWorkspaces.toString().replace(/CC%202015/g,versionstr));

	if (dirUserWorkspacesFrom.exists)
		{
		arrayUserFolderWorkspacesTo = getDirContents(dirUserWorkspaces.getFiles());
		arrayUserFolderWorkspacesFrom = getDirContents(dirUserWorkspacesFrom.getFiles());
		
		// Extensions were added to workspace files in 15.0, so ensure that we are looking for files
		// with extensions when looking for conflicts in the folders. We don't have to worry about adding
		// extensions when we copy the files because Photoshop will add them.
		var forcedFromExt = ".psw";
		
		arrayUserFolderWorkspacesDiff = getMissingFiles(arrayUserFolderWorkspacesFrom,arrayUserFolderWorkspacesTo,justCheck,forcedFromExt);
		arrayUserFolderWorkspacesDiffDont = new Array();
    	}
}

function refreshDiffDataUserWorkspacesModified(versionstr,justCheck) {

    dirUserWorkspacesModifiedFrom = new Folder(dirUserWorkspacesModified.toString().replace(/CC%202015/g,versionstr));
    
	if (dirUserWorkspacesModifiedFrom.exists)
		{
		arrayUserFolderWorkspacesModifiedTo = getDirContents(dirUserWorkspacesModified.getFiles());
		arrayUserFolderWorkspacesModifiedFrom = getDirContents(dirUserWorkspacesModifiedFrom.getFiles());
		
		// Extensions were added to workspace files in 15.0, so ensure that we are looking for files
		// with extensions when looking for conflicts in the folders. We don't have to worry about adding
		// extensions when we copy the files because Photoshop will add them.
		var forcedFromExt = ".psw";
		
		arrayUserFolderWorkspacesModifiedDiff = getMissingFiles(arrayUserFolderWorkspacesModifiedFrom,arrayUserFolderWorkspacesModifiedTo,justCheck,forcedFromExt);
		arrayUserFolderWorkspacesModifiedDiffDont = new Array();
    	}
}

function refreshDiffDataUserPresets(versionstr,justCheck) {

    dirUserPresetsFrom = new Folder(dirUserPresets.toString().replace(/CC%202015/g,versionstr));
    
    if (dirUserPresetsFrom.exists)
    	{
		arrayUserFolderPresetsTo = getDirContents(dirUserPresets.getFiles());  
		arrayUserFolderPresetsFrom = getDirContents(dirUserPresetsFrom.getFiles()); 
		arrayUserFolderPresetsDiff = getMissingFiles(arrayUserFolderPresetsFrom,arrayUserFolderPresetsTo, justCheck);
		arrayUserFolderPresetsDiffDont = new Array();
    	}
}

/****************************************
 * migrateFiles
 ****************************************/
function migrateFiles(filesToMigrate, versionstr, versionstrcurrent){
    //quietly return if there is nothing to migrate
    if(filesToMigrate.length == 0){
        return;
    }
    
    var migrationRes = doMigrateFiles(versionstrcurrent);
    if(!migrationRes){
        alert(msgCompleteError, titleFileMigration, true);
    }

    function doMigrateFiles(version_cur){
        try{
        	// the version "CC xxxx" is actually represented as "CC%20xxxx"
        	var old_ver = versionstr.replace( RegExp(" ", "gi"), "%20" );
        	var new_ver = version_cur.replace( RegExp(" ", "gi"), "%20" );
        	
            for(var i=0; i<filesToMigrate.length;i++){
                var targetFolder = Folder(filesToMigrate[i][1].parent.toString().replace(RegExp(old_ver, "gi"),new_ver));
                var targetFile = targetFolder+"/"+filesToMigrate[i][1].name;   
                if(!targetFolder.exists){
                    var createFolder = targetFolder.create();
                }
                if(targetFolder.exists){
                	// DO NOT clobber destination!!!
                	var fileCopy = false;
                	if (!File(targetFile).exists)
                	{
                    	fileCopy = filesToMigrate[i][1].copy(targetFile);
						if(filesToMigrate[i][0] == "Actions")
						{
							load(File(targetFile));
						}
					}
					else
					{
						// warn user
						filesToMigrate[i][1].error = "Skip copying because file exists";
					}
                }
                if(!fileCopy || !File(targetFile).exists){
                    alertScriptError(localize("$$$/MigratePresets/MessageCopyFail2=An error occurred while migrating the file: ") + decodeURI(filesToMigrate[i][1].name) + "(" + filesToMigrate[i][1].error + ")" );
                }
                
                // Update progress bar every so often
                if ((i % 100) == 0) {
					if (!app.updateProgress(i+1, filesToMigrate.length))
						throw kUserCancelError;
					}
             }
            return true;
        }catch(e){
        	if (e == kUserCancelError)
        		throw e;
        	
        	alertScriptError("Line: " + e.line +" - "+ e);
        }
    }
}

/****************************************
 * HasExtension
 ****************************************/
function HasExtension (filename, extension)
	{
	return (filename.indexOf(extension, filename.length - extension.length) !== -1);
	}

/****************************************
 * getMissingFiles
 ****************************************/
function getMissingFiles(fromArray,toArray,justCheck,forcedFromExtension) {
    var diffItems = new Array();
    try{
    	var totalChecks = fromArray.length * toArray.length;
    	var doneChecks = 0;
    	
        for(var x=0;x<fromArray.length;x++){
        
        	var fromFilename = fromArray[x][1].name;
        	
        	if (forcedFromExtension != null && !HasExtension (fromFilename, forcedFromExtension))
        		fromFilename += forcedFromExtension;
        	
        	var inToArray = false;
        	for (var i=0; i<toArray.length; i++) {
    			if (toArray[i][1].name.toLowerCase() == fromFilename.toLowerCase())
    				{
    				inToArray = true;
    				doneChecks = (x + 1) * toArray.length;
    				break;
    				}
    			else
    				doneChecks++;
        		
        		// Update progress bar every so often
        		if (doneChecks % 10000 == 0) {
	        		if (!app.updateProgress(doneChecks, totalChecks))
	        			throw kUserCancelError;
	        	}
        	}
        	
            if(!inToArray){
                diffItems.push(fromArray[x]);
                
                // If we are just checking, return once we know there is a difference
                if (justCheck)
                	return diffItems;
            }
        }
    }catch(e){
    	if (e == kUserCancelError)
    		throw e;
    	
    	alertScriptError("Line: " + e.line +" - "+ e);
    }
    return diffItems;
}

/****************************************
 * getSettingsContents
 ****************************************/
function getSettingsContents(settingFolder) {
	settingsItems = new Array();
	var objItem;
	var searchresult;
	for (var i = 0; i < activesetfiles.length; i++)
		{
		searchresult = settingFolder.getFiles(activesetfiles[i]);
		if (searchresult.length > 0) 
			{
			settingsItems.push(new Array(searchresult[0].parent.name, searchresult[0]));
			}
		}
	objItem = null;
	searchresult = null;
	return settingsItems;
}


/****************************************
 * getDirContents
 ****************************************/
function getDirContents(tmpFolderItems) {
    fileItems = new Array();
    getFiles(tmpFolderItems,fileItems);
    
    // Disabling sort because it is quite slow, we can't update progress during sorting,
    // and we do not take advantage of the fact that the array is sorted.
 //   outFileItems.sort();
 	return fileItems;
}
/****************************************
 * getFiles
 ****************************************/
function getFiles(tmpFolderItems,fileItems) {

    var objItem;
    
    for (var i=0;i<tmpFolderItems.length;i++){
        objItem = tmpFolderItems[i];
        if (objItem instanceof Folder){
            objFolderName = objItem.name;
            getFiles(objItem.getFiles(),fileItems);
        } else if ( -1 != objItem.fsName.indexOf(".DS_Store")){
            continue;	// Skip Mac's hidden file
        } else {
           fileItems.push(new Array(objItem.parent.name,objItem));
        }
    }
    objItem = null;
}

/****************************************
 * getDirApp
 ****************************************/
function getDirApp(){
    /*
    The full path of the location of the Adobe Photoshop application.
    */
    return app.path;
}

/****************************************
 * getDirCommonFiles
 ****************************************/
function getDirCommonFiles(){
    /*
    In Windows, the value of %CommonProgramFiles% (by default, C:\\Program Files\\Common Files)
    In Mac OS, /Library/Application Support
    */
    return Folder.commonFiles;
}

/****************************************
 * getDirUserData
 ****************************************/
function getDirUserData(){
    /*
    In Windows, the value of %USERDATA% (by default, C:\\Documents and Settings\\ username \\Application Data) 
    In Mac OS, ~/Library/Application Support.
    */
    return Folder.userData;
}
/****************************************
 * getDirUserPreferencesMac
 ****************************************/
function getDirUserPreferencesMac(){
    /*
    In Windows, the value of %USERDATA% (by default, C:\\Documents and Settings\\ username \\Application Data) 
    In Mac OS, ~/Library/Application Support.
    */
    var tempUserData = decodeURI(Folder.userData).toString().replace("Application Support", "Preferences");
    return Folder(tempUserData);
}

/****************************************
 * alertScriptError
 ****************************************/
function alertScriptError(msg){
	alert(msg,"File Migration Error",true);
	errorToQuit++;
}

/****************************************
 * getCurOS
 ****************************************/
function getCurOS(curOS){
	try{
		var myOS;
		if(curOS.match("Macintosh")){
			myOS = "mac";
		}else if(curOS.match("XP")){
			myOS = "winxp";
		}else if(curOS.match("Vista")){
			myOS = "winvista";
		}else{
			myOS = "win7";
		}
		return myOS;
    }catch(e){
        alertScriptError("Line: " + $.line +" - "+ e);
    }
}
/****************************************
 * getAppVer
 ****************************************/
function getAppVer(){
	try{
        var curAppVer = app.version;
        var arrayAppVer = curAppVer.split("."); 
        return parseInt(arrayAppVer[0]);
    }catch(e){
        alertScriptError("Line: " + $.line +" - "+ e);
    }
}
