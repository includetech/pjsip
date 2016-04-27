try {
	window.onerror = function(msg, url, line) {
		var isDebug = false;
		try {
			var params = window.top.document.location.search.replace(/^\?/i, '').split('&');
			for (var i = 0, paramsLength = params.length; i < paramsLength; i++)
            if ('debug=true' === params[i]) { isDebug = true; break; }
		} catch(e) {
			// do nothing here… we dont care about parse error
		}
		try {
			if (window.top.projectHello)
				window.top.projectHello.hostAppNotifyFailedToLoad('err=' + msg + '; url=' + url + '; line=' + line);
            
			if (isDebug)
				alert('Error: ' + msg + '\nURL: ' + url + '\nLine: ' + line);
		} catch (e) {
			if (isDebug)
				alert(e.message ? e.message : e);
		}
        
		return true; // suppress errors
	};
} catch(errCreateListener) {
	// do nothing
}
