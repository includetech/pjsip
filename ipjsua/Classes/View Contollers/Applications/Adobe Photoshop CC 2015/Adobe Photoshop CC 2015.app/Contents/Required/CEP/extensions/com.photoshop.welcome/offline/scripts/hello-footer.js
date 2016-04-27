
var CHLFooterHelper = {

    // Read account information
    getAccountType: function () {
        var accountType = "paid";
        if (typeof (parent.personalizationProperties) !== "undefined") {
            accountType = parent.personalizationProperties["accountType"];
        }
        return accountType;
    },

    // Get the check box value
    getHideOnLaunch: function () {
        if (typeof (parent.personalizationProperties) !== "undefined") {
            var checkBoxState = parent.personalizationProperties["dontShowAgain"];
            // Wrapper DOM returns a string. Convert to Boolean.
            return (checkBoxState.toLowerCase() === "true");
        }
        return false;
    },

    // Pass the check box value back to wrapper
    setHideOnLaunch: function (hideOnLaunchState) {
        if (typeof (parent.projectHello) !== "undefined") {
            if (typeof (parent.projectHello.hostAppSetDoNotShowAgainPreference) !== "undefined") {
                parent.projectHello.hostAppSetDoNotShowAgainPreference(hideOnLaunchState);
            }
        }
        if (typeof (parent.personalizationProperties) !== "undefined") {
            var checkBoxState = $("#hidePanel").is(':checked') ? "true" : "false";
            parent.personalizationProperties["dontShowAgain"] = checkBoxState;
        }
    },

    toggleFooterMessage: function (checkBoxState) {
        $( "#cbtext2" ).stop(true, true);
        $( "#cbtext1" ).stop(true, true);
        if (checkBoxState) {
            $( "#cbtext1" ).fadeOut( 1, function() {
                $( "#cbtext2" ).fadeIn(1).delay(2000).fadeOut( 250, function() {
                    $( "#cbtext1" ).fadeIn(250)
                });
            });
        } else {
            $( "#cbtext2" ).stop(true, true).hide();
            $( "#cbtext1" ).stop(true, true).show();
        }
    },

    openBrowser: function (url) {
        if (typeof (parent.projectHello) !== "undefined") {
            if (typeof (parent.projectHello.openURLInDefaultBrowser) !== "undefined") {
                parent.projectHello.openURLInDefaultBrowser(url);
            }
        }
    }
};


// Hide panel checkbox in footer
function loadOffineFooter(PageName){	
    if (CHLFooterHelper.getAccountType() == "paid" && PageName.indexOf("create") == -1) {
        var checkBoxState = CHLFooterHelper.getHideOnLaunch();
        $("#hidePanel").prop("checked", checkBoxState);
        $("#hidePanelDiv1").show();
        $("#hidePanelDiv2").show();	
    } else {
		$("#hidePanelDiv1").hide();
        $("#hidePanelDiv2").hide();
    }

	$("#hidePanel").click(function () {
        var checkBoxState = $("#hidePanel").is(':checked') ? true : false;
        CHLFooterHelper.setHideOnLaunch(checkBoxState);
        CHLFooterHelper.toggleFooterMessage(checkBoxState);
    });

    $("#CCLogoDiv a").hover(function(){
        $(".cc-logo").attr("src","images/cc-logo-hover.png")
    },function(){
        $(".cc-logo").attr("src","images/cc-logo.png")
    })

    if (document.domain.indexOf("helpx.") !== -1) {
        document.body.oncontextmenu = function () {
            return false;
        }
    }
}
