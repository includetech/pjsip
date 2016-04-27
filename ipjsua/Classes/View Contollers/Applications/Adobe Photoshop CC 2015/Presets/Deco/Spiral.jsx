//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here are a few parameters that you can change to modify the behavior of the patterm
// Feel free to modify the values, don't change the variable names

modelParameters = {
    // scaling of the input pattern
    patternScale : 1,
    
    // Offset in pixels between two rings of the spiral
    // Value of 1 will create tightly packed spiral, higher value will create bigger gap between spiral rings
    ringOffset : 0,    // use a value between -50 and 100. The default is 0.

    // Controls how close the elements are along the spiral. 
    // Use value from 0.3 to 1.5, it is a multiplicative factor for the angle between subsequent segments.
    // The default is 1.
    patternOffset : 1, 

    // Add spacing in pixels around each patterns in pixels. Note that this value will increase the offset between spiral rings
    spacing : 0,   // use a value between -10 to 20. The default is 0.

    // Keep each pattern upright.
    keepPatternUpright : false,  // The default is false. 
    
    // Variation of color of the pattern. 
    // For example, value of 0.2 means that each of the red, green, and blue color components
    // will be multiplied by a DIFFERENT random value from interval 0.8 and 1.2. 
    // Set to 0 if you do not want to modify the pattern color.
    colorRandomness : 0.05,    // use a value between 0 and 1. The default is 0.05.

    // Variation of pattern brightness. 
    // For example, value of 0.6 means that each of the red, green, and blue color components
    // will be multiplied by THE SAME random value from interval 0.4 and 1.6. 
    // Set to 0 if you do not want to modify the pattern brightness.
    brightnessRandomness : 0.1,   // use a value between 0 and 1. The default is 0.1.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// You can modify the code below but keep in mind that as with any scripting
// you can break things. Keep a backup copy.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get pattern  and its size
var pattern = RenderAPI.getParameter(kpsPattern)
var patternSize = pattern.getParameter(kpsSize)
var patternSizeForDialog = Math.max (patternSize.x, patternSize.y) 

// For the script to work with a menu with preview, you need to define a run method with the following three parameters:
// 1. api - render api to be used. Do not use RenderAPI directly, since preview uses a different renderer
// 2. parameter - contains model parameters. Do not refer directly to modelParameters inside the run method or methods called from it.
// 3. scale - preview can be scaled up or down depending on the size of the input pattern. Accomodate scale inside the run method.
function run (api, parameters, scale)
{ 
    // get size of the output area
    var outputSize = api.getParameter(kpsSize)
    
    var patternSize = pattern.getParameter(kpsSize)
    patternSize *= parameters.patternScale

    //pattern.setParameter(kpsMaxPatternCacheSize, patternSize.x * patternSize.y * 7 * 8 * 180)
    
    // to finetune the progress bar, specify how many pixels are covered by patterns - the bigger the spacing the lower the value, for example
    api.setParameter(kpsPixelCoverage, 0.5 * patternSize.y / (patternSize.y + parameters.ringOffset) * 
                              patternSize.x / (patternSize.x + parameters.patternOffset))

    // possibly add spacing
    if (patternSize.x + parameters.spacing >= Math.min (5, patternSize.x) )
        patternSize.x += parameters.spacing
    else
        patternSize.x = Math.min (5, patternSize.x)
        
     if (patternSize.y + parameters.spacing >= Math.min (5, patternSize.y))
        patternSize.y += parameters.spacing
    else
        patternSize.y = Math.min (5, patternSize.y)

    // scale up if the patternsize is 1 to avoid long loop
    if (patternSize.x == 1 && patternSize.y == 1)
    {
        if (scale == 1)
            scale = 20
        keepPatternUpright = true    // 1x1 patterns that are scaled up do not rotate gracefully
    }

    patternSize *= scale


     var angle = -90
     var angleStep
     
     api.getParameter (kpsSelectedPaths)

    // if the visible area is smaller than the selection, center the spiral in the center of the visible area
    var visibleSize = api.getParameter(kpsVisibleSize)
    var visibleOrigin = api.getParameter(kpsVisibleOrigin)
    var outputOrigin = api.getParameter(kpsOrigin)
   
    var spiralCenter
     if (visibleSize.x < outputSize.x)
       // if the visible area is smaller than the selection, center the spiral in the center of the visible area
       spiralCenter = new Vector2 (visibleOrigin.x  - outputOrigin.x + visibleSize.x / 2, visibleOrigin.y  - outputOrigin.y+ visibleSize.y / 2)
    else
        // center in the output area (this is the selection)
        spiralCenter = new Vector2 (outputSize.x / 2, outputSize.y / 2)
       
    api.translate(spiralCenter.x, spiralCenter.y)
     
    var num = 0
    // get the longest of distances from the center to each corner
    var x1 = spiralCenter.x
    var x2 = outputSize.x - spiralCenter.x
    var y1 = spiralCenter.y
    var y2 = outputSize.y - spiralCenter.y
    var diagonalSq = Math.max (Math.max (x1 * x1 + y1 * y1, x1 * x1 + y2 * y2),
                                                Math.max (x2 * x2 + y1 * y1, x2 * x2 + y2 * y2))
    var diagonal = Math.sqrt(diagonalSq)  
                          + Math.sqrt(patternSize.x * patternSize.x + patternSize.y *patternSize.y)  * 0.5

    for (var radius = 0; Math.abs(radius) <  diagonal ; )
    {
       
         radius = patternSize.y/2 + ((patternSize.y - 1 + parameters.ringOffset * scale) / 360) * angle  // In one revolution (360 degrees) we want to add patternSize.y to the radius
         angleStep = 360 / ( 6.28 * (radius - patternSize.y/2)/ (patternSize.x + parameters.patternOffset * scale)) 
         
         api.pushMatrix()
        
         // Set the seed based on the current row and column - this assures that the color will be modified
         // in the same way for the pattern in the neighboring selected area
         var seed =  Math.floor(((angle * 214013) % 0x7fffffff))
      
         var rc = parameters.colorRandomness  // color randomness
         var br = 1 - parameters.brightnessRandomness + Engine.rand(seed) * parameters.brightnessRandomness*2  // brightness
         api.Color (kFillColor, br *(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2))
       
         // rotate around the center of the selection 
         api.translate(-outputSize.x / 2, -outputSize.y / 2)
         api.rotate(angle)
         api.translate(outputSize.x / 2, outputSize.y / 2)
        
        api.translateRel(0, radius)   // translate relative to the rotated frame
        api.scale (scale, scale)
        
        if (parameters.keepPatternUpright)
            // cancel the rotation above
            api.rotate(-angle)
        else
            // Adjust the rotation set above so that the pattern itself is rotated only in multiples of 2 degrees
            api.rotate( - angle + 2 * Math.floor(0.5+angle/2)) 
            
        api.scale(parameters.patternScale)
        pattern.render(api)
        
        api.popMatrix()
     
        angle += angleStep
    }
}
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu start
// If you want to create a menu in which you change some of the script parameters, include the following code:
// For shipped scripts we can include localized strings, prefixed with $$$/ - call method localize(string) on the prefixed string
// For your own strings, you can query app.locale and select several language versions (for example, your native language and english). Try alert ("Language is: " + app.locale)

var decoMenu = {    //  an object that defines the menu
   menuTitle : localize("$$$/DecoScripts/Spiral/Spiral=Spiral"),
   menuBackground : [0.93, 0.93, 0.93, 1],
   previewBackground : [1, 1, 1, 1],
   panels : [
    { panelName : "", 
       leftColumnWidth : 180,
       unitsWidth : 65,
       dropdownlistWidth : 160,
       panelMenu : [
         { itemName : localize("$$$/DecoScripts/PatternScale=Pattern Scale:") ,  
             itemUnit :  "", itemType : 'slider', itemValue : modelParameters.patternScale, itemMin : 0.1, itemMax : 1.25, itemStep : 0.01, varName : 'patternScale'  }, 

        { itemName : localize("$$$/DecoScripts/Spiral/RingOffset=Ring spacing:"), 
             itemUnit : localize("$$$/DecoScripts/Units/pixels=pixels"), 
             itemType : 'slider', itemValue : modelParameters.ringOffset, itemMin : Math.min(-20, -1 * patternSizeForDialog), itemMax : Math.max(100, 2 * patternSizeForDialog), itemStep : 1, varName : 'ringOffset'  }, 
         
         { itemName : localize("$$$/DecoScripts/Spiral/PatternSpacing=Pattern spacing:"), 
             itemUnit : localize("$$$/DecoScripts/Units/pixels=pixels"), 
             itemType : 'slider', itemValue : modelParameters.patternOffset, itemMin : Math.min(-20, -1 * patternSizeForDialog), itemMax : Math.max(100, 1 * patternSizeForDialog), itemStep : 1, varName : 'patternOffset'  }, 
         
         { itemName : localize("$$$/DecoScripts/Spiral/KeepUpright=Keep pattern upright:"),
              itemUnit : '', itemType : 'checkbox', itemValue : modelParameters.keepPatternUpright, itemMin : 0, itemMax : 0, varName : 'keepPatternUpright'  },

         { itemName : localize("$$$/DecoScripts/ColorRandomness=Color randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.colorRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'colorRandomness'  }, 
         
          { itemName : localize("$$$/DecoScripts/BrightnessRandomness=Brightness randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.brightnessRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'brightnessRandomness'  }, 
         //{ itemName : "Pattern rotate angle:",  itemUnit : 'degrees',  itemType : 'slider', itemValue : " + rotateAngle + ", itemMin : -180, itemMax : 180, itemStep : 1, varName : 'rotateAngle'  }, 
       ] }
   ]  // end of panels
 }  // end of menu

// If livePreview is set to 1, the preview image is updated live. Note that due to limitations of scripted menus the update is slow and the flickering may be disturbing. 
livePreview = 0 // recommended value is 0

// Call Photoshop Script that creates the menu
Engine.evalFile ("_Deco Menu.jsx") 

// Menu finished
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


if (typeof skipRun == 'undefined' || !skipRun)  // run unles we exited the preview window without pressing a button
    run(RenderAPI, modelParameters, 1)


