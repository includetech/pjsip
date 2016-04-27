///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here are a few parameters that you can change to modify the behavior of the patterm
// Feel free to modify the values, don't change the variable names

modelParameters = {
    // scaling of the input pattern
    patternScale : 1,
    
    // Spacing between patterns in pixels. 
    // For example,1 creates 1 pixel gap between patterns
    spacing : 0,   // use a value between -10 to 20. The default is 0.

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
    // get the size of the output area  
    var outputSize = api.getParameter(kpsSize)
    // get the location of the top left corner of the bounding rectangle around the selected area
    var outputOrigin = api.getParameter(kpsOrigin)

    var patternSize = pattern.getParameter(kpsSize)
    patternSize *= parameters.patternScale

    // to finetune the progress bar, specify how many pixels are covered by patterns - the bigger the spacing the lower the value, for example
    api.setParameter(kpsPixelCoverage, 0.5 * (patternSize.x * patternSize.y) / ((patternSize.x + parameters.spacing) * (patternSize.y + parameters.spacing ) ) )

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
    }
     
    patternSize *= scale;

    var sizeStep = (patternSize.x + patternSize.y) / 2

     // negative origin may happen for preview of very small documents
    if (outputOrigin.x < 0)
        outputOrigin.x = 0
    if (outputOrigin.y < 0)
        outputOrigin.y = 0
    
   // don't use % since sizeStep may not be integer
    var xRel = outputOrigin.x - Math.floor (outputOrigin.x / sizeStep) * sizeStep
    var yRel = outputOrigin.y - Math.floor (outputOrigin.y / sizeStep) * sizeStep
    api.translateRel ( -xRel, -yRel)

    var row = Math.floor( outputOrigin.y / sizeStep ) 
    var column = Math.floor( outputOrigin.x / sizeStep )

    var mult = ((row*0 + column) % 2) == 0 ? 1 : -1;
    api.translate(-(column%2)* (patternSize.y - patternSize.x)/2, 0)

    var margin_y = Math.max(patternSize.x+patternSize.y, -parameters.spacing)
    for (var y = -patternSize.y/2; y < outputSize.y + margin_y;   row++)
    {
        api.pushMatrix()
        
        // shift each second row
        api.translate(-mult*(row%2)* (patternSize.y - patternSize.x)/2 - 2*sizeStep, y)
     
        var col = column
        var margin_x = Math.max(4*sizeStep, -parameters.spacing)
        for (var x = 0; x < outputSize.x + margin_x;  x+= rotatedSizeX, col++)
        {
            // Set the seed based on the current row and column - this assures that the color will be modified
            // in the same way for the pattern in the neighboring selected area
            var seed =  (row * 214013+ col * 2531011) %0x7fffffff // 
 
            var rc = parameters.colorRandomness  // color randomness
            var br = 1 - parameters.brightnessRandomness + Engine.rand(seed) * parameters.brightnessRandomness*2  // brightness
            api.Color (kFillColor, br *(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2))
 
            var angle = 90 * (col + row + Math.floor(Engine.rand()*2)*2)
            
            var side = (angle / 90) % 2
            rotatedSizeX = (1-side) * patternSize.x + side * patternSize.y
            
            // Use this computation if angle is not a multiple of 90
            //var sinA = Math.abs(Math.sin(angle / 180 * 3.141529) )
            //var cosA = Math.abs(Math.cos(angle / 180 * 3.141529) )
            //var rotatedSizeX = patternSize.x * cosA + patternSize.y * sinA
            //var rotatedSizeY = patternSize.x * sinA + patternSize.y * cosA
     
            api.translate(rotatedSizeX/2, 0)
                   
            // the pattern is centered
            api.pushMatrix()
            api.scale (scale, scale)
            api.rotate(angle)
          
            api.scale(parameters.patternScale)
            pattern.render(api)
            api.popMatrix()
            
            api.translate(rotatedSizeX/2, 0) 
        }

        y+=sizeStep

        api.popMatrix()
     }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu start
// If you want to create a menu in which you change some of the script parameters, include the following code:
// For shipped scripts we can include localized strings, prefixed with $$$/ - call method localize(string) on the prefixed string
// For your own strings, you can query app.locale and select several language versions (for example, your native language and english). Try alert ("Language is: " + app.locale)

var decoMenu = {    //  an object that defines the menu
   menuTitle : localize("$$$/DecoScripts/CrossWeave/CrossWeave=Cross Weave"),
   menuBackground : [0.93, 0.93, 0.93, 1],
   previewBackground : [1, 1, 1, 1],
   panels : [
    { panelName : "", 
       leftColumnWidth : 180,
       unitsWidth : 65,
       panelMenu : [
         { itemName : localize("$$$/DecoScripts/PatternScale=Pattern Scale:") ,  
             itemUnit :  "", itemType : 'slider', itemValue : modelParameters.patternScale, itemMin : 0.10, itemMax : 1.25, itemStep : 0.01, varName : 'patternScale'  }, 

        { itemName : localize("$$$/DecoScripts/CrossWeave/Spacing=Spacing:"),  
             itemUnit : localize("$$$/DecoScripts/Units/pixels=pixels"), 
             itemType : 'slider', itemValue : modelParameters.spacing, itemMin : -patternSizeForDialog+1, itemMax : Math.max(100, 3 * patternSizeForDialog), itemStep : 1, varName : 'spacing'  },
        
          { itemName : localize("$$$/DecoScripts/ColorRandomness=Color randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.colorRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'colorRandomness'  },
 
          { itemName : localize("$$$/DecoScripts/BrightnessRandomness=Brightness randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.brightnessRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'brightnessRandomness'  }
         
       ] }
   ]  // end of panels
 }; // end of menu

// If livePreview is set to 1, the preview image is updated live. Note that due to limitations of scripted menus the update is slow and the flickering may be disturbing. 
livePreview = 0 // recommended value is 0

// Call Photoshop Script that creates the menu
Engine.evalFile ("_Deco Menu.jsx") 

// Menu finished
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

if (typeof skipRun == 'undefined' || !skipRun)  // run unles we exited the preview window without pressing a button
    run(RenderAPI, modelParameters, 1)
