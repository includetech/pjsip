///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here are a few parameters that you can change to modify the behavior of the patterm
// Feel free to modify the values, don't change the variable names

modelParameters = {
    // scaling of the input pattern
    patternScale : 1,
    
    // Offset between rows of pattern expressed in percent of pattern width. 
    // For example 50% is half the width.
    offset : 50,    // use a value between 0 and 100. The default is 50.

    // Spacing between patterns in pixels. 
    // For example,1 creates 1 pixel gap between patterns
    spacing : 0,    // use a value between -10 to 20. The default is 0.

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

    // Rotation of individual patterns.
    rotateAngle : 0 // Use a value between -180 and 180. The default is 0.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// You can modify the code below but keep in mind that as with any scripting
// you can break things. Keep a backup copy.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get pattern and its size
var pattern = RenderAPI.getParameter(kpsPattern)
var patternSize = pattern.getParameter(kpsSize)
var patternSizeForDialog = Math.max (patternSize.x, patternSize.y) 


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
    
    // compute the pattern size in x and y after rotation
    var rotSizeX = patternSize.x * Math.abs( Math.cos(parameters.rotateAngle / 180.0 * Math.PI)) +  patternSize.y * Math.abs( Math.sin(parameters.rotateAngle / 180.0 * Math.PI))
    var rotSizeY = patternSize.x * Math.abs( Math.sin(parameters.rotateAngle / 180.0 * Math.PI)) +  patternSize.y * Math.abs( Math.cos(parameters.rotateAngle / 180.0 * Math.PI))
    
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

    patternSize *= scale
    //patternSize *= 2

    api.translateRel (patternSize.x/2, patternSize.y/2)
 
    // negative origin may happen for preview of very small documents
    if (outputOrigin.x < 0)
        outputOrigin.x = 0
    if (outputOrigin.y < 0)
        outputOrigin.y = 0
    
    api.translateRel ( -(outputOrigin.x % patternSize.x), -(outputOrigin.y % patternSize.y))

    var row = Math.floor( outputOrigin.y / patternSize.y ) 
    var column = Math.floor( outputOrigin.x / patternSize.x )
    var extraRows = 0            // on each side, if needed
    var extraColumns = 0
    if (parameters.rotateAngle != 0 || parameters.spacing < 0)
    {
        // start a few rows or columns earlier because the tips of rotated patterns may show up in the selection 
        // or because the spacing is negative and transparent patterns to the left would still show up
        extraRows = Math.floor (rotSizeY / (2 * patternSize.y) + 0.4999)
        extraColumns = Math.floor (rotSizeX / (2 * patternSize.x) + 0.4999)
        
        row -= extraRows;
        column -= extraColumns;
        api.translateRel ( -patternSize.x * extraColumns, -patternSize.y * extraRows)
    }

    pattern.setParameter (kpsColorBlendMode, kpsBlendMultiply)
    //pattern.setParameter (kpsColorBlendMode, kpsBlendLinearLight)

    //var margin_y = Math.max(patternSize.y, -parameters.spacing)
    var margin_y = patternSize.y + 2 * extraRows * patternSize.y
        
    for (var y = 0; y < outputSize.y + margin_y;  y+= patternSize.y, row++)
    {
        api.pushMatrix()
        
        var x = 0
        
        
        //if ( (row%2) == 1)
        {
            var shift = parameters.offset*0.01 * row
            api.translateRel( ((shift - Math.floor(shift)) - 1)* patternSize.x, 0)
             x = - patternSize.x // one extra, just in case
        }
       
        //var margin_x = Math.max(patternSize.x * (1 + parameters.offset*0.01), -parameters.spacing)
        var margin_x = patternSize.x + 2 * extraColumns * patternSize.x
            
        for (var c = column; x < outputSize.x + margin_x;  x+= patternSize.x, c ++)
        {           
            api.pushMatrix()
            
            // Set the seed based on the current row and column - this assures that the color will be modified
            // in the same way for the pattern in the neighboring selected area
            var seed =  (row * 214013+ c * 2531011) % 0x7fffffff // 
            //Engine.rand(seed)
            //Engine.rand(); // it helps to call rand a few times beforor using it since we are forcing the seed.

            var rc = parameters.colorRandomness  // color randomness
            var br = 1 - parameters.brightnessRandomness + Engine.rand(seed) * parameters.brightnessRandomness*2  // brightness
            api.Color (kFillColor, br *(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2))
             
            //api.scale(rand()*0.1 + 1)
            //api.rotate(-4 + Math.floor(rand()*60) / 7.5)  // 60 distinct rotations - to help cache
            //api.rotate (Math.floor(rand()*10) * 18)
            api.rotate (parameters.rotateAngle)
            api.scale(scale)
            api.scale(parameters.patternScale)
            pattern.render(api)
            
            api.popMatrix()
            api.translateRel(patternSize.x, 0)
            
            if (restart)
                return;
        }

        api.popMatrix()
        api.translateRel(0, patternSize.y)
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu start
// If you want to create a menu in which you change some of the script parameters, include the following code:
// For shipped scripts we can include localized strings, prefixed with $$$/ - call method localize(string) on the prefixed string
// For your own strings, you can query app.locale and select several language versions (for example, your native language and english). Try alert ("Language is: " + app.locale)

var decoMenu = {    //  an object that defines the menu
   menuTitle : localize("$$$/DecoScripts/BrickFill/BrickFill=Brick Fill"),
   menuBackground : [0.93, 0.93, 0.93, 1],
   previewBackground : [1, 1, 1, 1],
   panels : [
    { panelName : "", 
       leftColumnWidth : 180,
       unitsWidth : 65, 
       panelMenu : [
        { itemName : localize("$$$/DecoScripts/PatternScale=Pattern Scale:") ,  
             itemUnit :  "", itemType : 'slider', itemValue : modelParameters.patternScale, itemMin : 0.1, itemMax : 1.25, itemStep : 0.01, varName : 'patternScale'  }, 
        { itemName : localize("$$$/DecoScripts/BrickFill/Spacing=Spacing:"),  
             itemUnit : localize("$$$/DecoScripts/Units/pixels=pixels"), 
             itemType : 'slider', itemValue : modelParameters.spacing, itemMin : -patternSizeForDialog+1, itemMax : Math.max(100, 3 * patternSizeForDialog), itemStep : 1, varName : 'spacing'  },
         
         { itemName : localize("$$$/DecoScripts/BrickFill/Offset=Offset between rows:") ,
             itemUnit : localize("$$$/DecoScripts/Units/percentOfWidth=% of width"),  
             itemType : 'slider', itemValue : modelParameters.offset, itemMin : -100, itemMax : 100, itemStep : 0.1, varName : 'offset'  }, 
         
         { itemName : localize("$$$/DecoScripts/ColorRandomness=Color randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.colorRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'colorRandomness'  }, 
         
          { itemName : localize("$$$/DecoScripts/BrightnessRandomness=Brightness randomness:"),  
             itemUnit : "",  itemType : 'slider' , itemValue : modelParameters.brightnessRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'brightnessRandomness'  }, 
         
        { itemName : localize("$$$/DecoScripts/BrickFill/rotateAngle=Pattern rotate angle:"),  
             itemUnit : localize("$$$/DecoScripts/Units/degrees=degrees"),  
             itemType : 'slider', itemValue : modelParameters.rotateAngle, itemMin : -180, itemMax : 180, itemStep : 1, varName : 'rotateAngle'  }
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
