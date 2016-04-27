	var args = []; 
	var personalizationProperties;
	var cUpdateIntervalSec = 10;
	var secondsPerDay = 86400;
	var secondsPerHour = 3600;
	var secondsPerMinute = 60;
	var lastTabClicked = "";
	
	function processAudienceManagerProfile(profile) { $(window).trigger('generateMBoxes', profile); }
		
	//$(document).bind( 'touchmove', function(e) { e.preventDefault(); } );
	
	function jsonDataReceiver(jsonData)
	{		
		if ( typeof jsonData !== "undefined" && jsonData !== "" && jsonData !== null)
		{
            // photoshop builds their JSON string by concatinating JavaScript strings which
            // may or may not be properly escaped for " characters.  So we need to do a quick
            // check and fix them up so we dont get a malformed JSON before sending it to parse
            var fixedString = jsonData.replace(/(:\s*"[^"]*)"([^"]*"\s*(,|\s*\}))/g, '$1\\"$2');
                                               
            jsonData = JSON.stringify(eval("(" + fixedString + ")"));

            personalizationProperties = JSON.parse(jsonData,function( key, val )
                                                            {
                                                                // the file display name and tooltip values
                                                                // coming from the host app side need to be
                                                                // properly encoded for display HTML - this
                                                                // is so that if XSS is tried to be passed
                                                                // through the strings, it gets encoded away
                                                                if ( key === "name" || key === "tip" )
                                                                {
                                                                    val = Encoder.htmlEncode( val );
                                                                }
                                                                // if this app uses the full path to the file
                                                                // for the open command, then encode it properly
                                                                // too.  More XSS protection.
                                                                else if ( key === "fullpath" )
                                                                {
                                                                    val = encodeURI(val);
                                                                }
                                                                return val;
                                                            });
            }
						
		loadDefaultLanguage();							
	}	
	
    $(document).ready(function () {					
		//document.oncontextmenu = function() {return false;}					
		
		onLoaded();				
		if (window.projectHello === undefined) window.projectHello = {};
		window.projectHello.hostAppGetPersonalizationInfo(jsonDataReceiver);											
										
	});	
	
	function loadDefaultLanguage()
	{		
		LangStringSet = personalizationProperties["language"];
		if(LangStringSet.length == 0) LangStringSet = "en_US";
		
		$.get("language/"+LangStringSet+".xml", {}, function(xml) {		
			var visitCounter = "";
			$(xml).find("string").each(function()
			  {									

				if($(this).attr("counter") == "true")
				{visitCounter += $(this).attr("name")+'Counter,'+$(this).attr("name")+'LUpdate,';}
				
				if($(this).attr("align")=="left")
				{
					if(personalizationProperties["accountType"]=="trial" && $(this).attr("name") == "specialoffer")
					{
						$(".shell_nav ul").append('<li name="'+$(this).attr("name")+'" data-url="'+$(this).attr("data-url")+'" counter="'+$(this).attr("counter")+'"><span id="tDaysLeft"></span>'+$(this).text()+'</li>');	
						loadTrialDaysRemaining();					
					}				
					else if($(this).attr("name") != "specialoffer") 
					{
						$(".shell_nav ul").append('<li name="'+$(this).attr("name")+'" data-url="'+$(this).attr("data-url")+'" counter="'+$(this).attr("counter")+'">'+$(this).text()+'</li>');
					}					
				}
				else if($(this).attr("align")=="right")
				{
					if(personalizationProperties["accountType"]=="trial" && $(this).attr("name") == "specialoffer")
					{
						$(".shell_nav ul").append('<li name="'+$(this).attr("name")+'" style="float:right;" data-url="'+$(this).attr("data-url")+'" counter="'+$(this).attr("counter")+'"><span id="tDaysLeft"></span> '+$(this).text()+'</li>');
						loadTrialDaysRemaining();
					}				
					else if($(this).attr("name") != "specialoffer") 
					{
						$(".shell_nav ul").append('<li name="'+$(this).attr("name")+'" style="float:right;" data-url="'+$(this).attr("data-url")+'" counter="'+$(this).attr("counter")+'">'+$(this).text()+'</li>');
					}
				}	
								
			  });
				
			$('.shell_nav ul li').on('click', function(){
				var liName	= $(this).attr('name');	
				$("body").removeAttr( "style" );							
				$(this).addClass('active').siblings().removeClass('active');		
				$('#shell_wrapper').stop().fadeOut(300, function() {
					loadContentPages(liName);				
				});
			});			

			if(checkCookie('lastTabClicked') != 1)
			{
				$(".shell_nav ul li[name=create]").trigger( "click" );
			}
			else
			{	
				lastTabClickedCookie = getCookie('lastTabClicked').split("=");
				lastTabClicked = lastTabClickedCookie[1];
				
				if(lastTabClicked == "" || lastTabClicked == undefined || lastTabClicked == "none")
				{
					$(".shell_nav ul li[name=create]").trigger( "click" );
				}
				else 
				{

					$(".shell_nav ul li[name="+lastTabClicked+"]").trigger( "click" );
				}	
			}
											
			
		}, "xml");
	}
	
	
	function loadContentPages(liName)
	{	
		$("#shell_content" ).load($(".shell_nav ul li[name="+liName+"]").attr('data-url'), function() {
			setCookie('lastTabClicked',liName,365);
			$('#shell_wrapper').fadeIn(300);	
			if(liName.indexOf("create")!=-1){LoadCreateContent();}	
			else{
				if(personalizationProperties["lastError"] != undefined && personalizationProperties["lastError"] != "" && personalizationProperties["lastError"] != null)
				{$(".offline_error_title").html(personalizationProperties["lastError"]);}
				else
				{$(".offline_error_title").html("Internet Connection Was Lost");}								
			}
		});
	}
			
	function LoadCreateContent(){
		
		var recentFiles = parent.personalizationProperties["recentFiles"];


		if(recentFiles.length == undefined || recentFiles.length == null || recentFiles.length == "" || recentFiles.length < 2)
		{
			$(".create_base_wrapper").css("display","none");
			$("body").attr("style", "background:url('images/PsCreateInitialExp.jpg') no-repeat;background-position:0px 45px;background-color:#F1D5B6;");			
			$(".create_base_wrapper_new").removeAttr( "style" );	
		}
		else
		{
			$(".create_base_wrapper_new").css("display","none");
			$(".create_base_wrapper").removeAttr( "style" );	
			$("body").removeAttr( "style" );	
	
			var newDocTypes = parent.personalizationProperties["newDocTypes"];		
							
			for ( i=0;i<newDocTypes.length;i++) {
				var docType = newDocTypes[i]["docType"];
				var docTypeName =  newDocTypes[i]["name"];
				$("#createNewContainer").append( '<a class="new_site_button" onClick="processNewDocClick(event,\'' + docType + '\')"><img src="images/' + newDocTypes[i]["icon"] + '.png"/><p> ' + docTypeName + '</p></a>');
				
			}
            
			for ( j=0;j<recentFiles.length;j++)
            {
				var fileName = recentFiles[j]["name"];
				var fileTip  = recentFiles[j]["tip"];
                
                if ( typeof fileTip === "undefined" )
                {
                    fileTip = "";
                }
                // because of possible XSS problems, we pass an index into the recent file array to the
                // click handler instead of the actual file, this way we avoid any problems with encoded
                // file paths
				$("#openRecentContainer").append( '<a class="new_site_button" title-"'+fileTip+'" onClick="processRecentDocClick(event,'+j+')"><img src="images/' + recentFiles[j]["icon"] + '.png" /><p>' + fileName + '</p></a>');
			}
		}
		
		setTimeout($('#shell_wrapper').fadeIn(300),9000);
		
		loadOffineFooter("create");
				
	}
		 
		
	function processNewDocClick( e, docType ) {		
		var doSuppressUI = e.altKey;
		window.projectHello.hostAppCreateNewDoc( docType, doSuppressUI );
	}
	
	
	function processRecentDocClick( e, recent )
    {
        var filePath = "";
        
        // to provide backwards support for older extensions, we need to check the
        // parameter type to make sure that we do the right thing.  If we have an
        // index parameter, we look up the file name (possibly encoded) in the recent
        // files array.  If we have a string then we assume it is a file path and
        // just pass it along - any other type is bad...so we ignore it
        if ( typeof recent === "number" )
        {
            filePath = parent.personalizationProperties.recentFiles[recent]["fullpath"];
        }
        else if ( typeof recent === "string" )
        {
            filePath = recent;
        }
        
        window.projectHello.hostAppOpenDoc( filePath );
	}

	function handleNewDocumentClick()
	{
		var newDocTypes = parent.personalizationProperties["newDocTypes"];
		var docType = newDocTypes[0]["docType"];
		window.projectHello.hostAppCreateNewDoc( docType, false);
	}	

	function loadTrialDaysRemaining() {
				
		var secondsPerDay = 86400;
		var appTrialTime = 0;
		var appStartTime = 0;
		var sysCurrentTime = parseInt((new Date()).getTime()/1000);
		var elapsedTime = 0;
		var remainingTime = 0;
		var remainingDays = 0;
		
		appTrialTime = parseInt(personalizationProperties["secondsLeftInTrial"]);		
		appStartTime = parseInt(personalizationProperties["appStartClockTime"]);
		elapsedTime = parseInt((sysCurrentTime - appStartTime));				
		remainingTime = parseInt(appTrialTime - elapsedTime);		
		
		if ( remainingTime < 0 ) remainingTime = 0;		
		remainingDays = Math.round(remainingTime / secondsPerDay);		
		$('#tDaysLeft').html(remainingDays)
	}
	
	function setCookie(cname,cvalue,exdays)
	{
		var d = new Date();
		d.setTime(d.getTime()+(exdays*24*60*60*1000));
		var expires = "expires="+d.toGMTString();
		document.cookie = cname + "=" + cvalue + "; " + expires;
	}
	
	function getCookie(cname)
	{
		var name = cname + "=";
		var ca = document.cookie.split(';');
		for(var i=0; i<ca.length; i++)
		{
			var c = ca[i].trim();
			if (c.indexOf(name)==0) return c;
		}
		return "";
	}
	
	function checkCookie(cname)
	{
		var coname = getCookie(cname);
		if (coname!="" && coname!=null && coname!=undefined)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}