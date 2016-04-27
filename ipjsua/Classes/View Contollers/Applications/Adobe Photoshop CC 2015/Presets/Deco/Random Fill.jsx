///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here are a few parameters that you can change to modify the behavior of the patterm
// Feel free to modify the values, don't change the variable names

modelParameters = {
    // Density of the covered pattern
    // For example,density of 1 places patters such that if they were aligned one next to another
    // they would cover all pixels. Keep in mind that since the patterns overlap, you will see gaps.
    density : 2,   // use a value between 0.1 to 10. The default is 2.
    
    // Maximum distance from a path 
    // If a path is selected the patterns are placed randomly around it, with a give maximum 
    // distance of the pattern ceter from the parth. The default is 20 pixels.
    maxDistanceFromPath : 20, // in pixels

    // Variation of color of the pattern. 
    // For example, value of 0.2 means that each of the red, green, and blue color components
    // will be multiplied by a DIFFERENT random value from interval 0.8 and 1.2. 
    // Set to 0 if you do not want to modify the pattern color.
    colorRandomness : 0.05,    // use a value between 0 and 1. The default is 0.05.

    // Variation of pattern brightness. 
    // For example, value of 0.6 means that each of the red, green, and blue color components
    // will be multiplied by THE SAME random value from interval 0.4 and 1..6 
    // Set to 0 if you do not want to modify the pattern brightness.
    brightnessRandomness : 0.1,   // use a value between 0 and 1. The default is 0.1.

    // You include random scaling of the patter
    // If you don't want any variation, set both to 1
    patternScaleFrom : 0.5,   // use a values between 0.1 and 3 . The default is 0.5.
    patternScaleTo : 1,          // use a values between 0.1 and 3 , larger or equal to patternScaleFrom. The default is 1.
     
    // Shoud we apply random rotation?
    applyRotation : true   // the default is true - we are doing rotations
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// You can modify the code below but keep in mind that as with any scripting
// you can break things. Keep a backup copy.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Get pattern 
var pattern = RenderAPI.getParameter(kpsPattern)

var inputPaths = RenderAPI.getParameter (kpsSelectedPaths)


// For the script to work with a menu with preview, you need to define a run method with the following three parameters:
// 1. api - render api to be used. Do not use RenderAPI directly, since preview uses a different renderer
// 2. parameter - contains model parameters. Do not refer directly to modelParameters inside the run method or methods called from it.
// 3. scale - preview can be scaled up or down depending on the size of the input pattern. Accomodate scale inside the run method.
function run (api, parameters, scale)
{
     // get the size of the output area  
    var outputSize = api.getParameter(kpsSize)
    var outputOrigin = api.getParameter(kpsOrigin)
 
    var patternSize = pattern.getParameter(kpsSize)

    // scale up if the patternsize is 1 to avoid long loop


    if (patternSize.x == 1 && patternSize.y == 1)
    {
        if (scale == 1)
            scale = 20
        patternSize *= scale
        parameters.applyRotation = false    // 1x1 patterns that are scaled up do not rotate gracefully
    }

    //api.translater(bbox.min[0], bbox.min[1])


    // possibly add spacing
    patternSize.x += 0
    patternSize.y += 0
    // place elements at random locations and size

    if (parameters == previewParameters)
        Engine.rand(1) // sets a fixed seed for preview
    else
        Engine.rand(Math.random()*214013) // random seed for a regular run

    var sizes = new Array()
    sizes.push (5)
    sizes.push (2)
    //sizes.push (1)
    //sizes.push (0.5)
 
    if (inputPaths.length > 0)
    {
        // adjust preview size
        var maxPatternSize = patternSize.x // x only
        
        if (parameters == previewParameters)
        {
            // set scale so that we show about 10-15 patterns
            if (maxPatternSize < previewSize / 15) 
                scale = previewSize / (15 * maxPatternSize)
            else if (maxPatternSize > previewSize / 10)
                scale = previewSize / (10 * maxPatternSize)
                
            if (parameters.maxDistanceFromPath * scale > previewSize / 4)
                scale = previewSize / (4 * parameters.maxDistanceFromPath)
        }
//alert ("scale = " + scale)
         var paths = inputPaths
         if (parameters == previewParameters)
        {
            paths = new Array(0)
            paths.push (new DecoGeometry)
            //paths[0].addLineStrip(new Vector3(0,0,0) + outputOrigin, new Vector3(255, 255, 0) + outputOrigin)
            paths[0].addBezier (new Vector3(10, (previewSize-10), 0) + outputOrigin, 
                                          new Vector3(10, 115,0 ) + outputOrigin, 
                                          new Vector3( (previewSize-10), 140, 0) + outputOrigin, 
                                          new Vector3( (previewSize-10), 10, 0) + outputOrigin)
        }
         
        for (var p = 0; p < paths.length; p++)
        {
            var geometry = paths[p]
            
            var length = geometry.getValue(kGetGeometryLength)
 
            // determine the number approximately based on size of the element and filled area
            var num = length * parameters.maxDistanceFromPath * 2 / (patternSize.x * patternSize.y * scale * scale) * parameters.density
            if (num < 1)
                num = 1

            for (var i = 0; i < num; i++)
            {
                api.pushMatrix()
                var r = Engine.rand()
                var w = parameters.maxDistanceFromPath * Math.pow(r, 0.5);
                
                var spanAlong = length
              
                var l = spanAlong * Engine.rand();
                var pt = geometry.getValue(kGetPointAlongGeometry, l)
                var normal = geometry.getValue(kGetNormalAlongGeometry, l)
                normal.normalize()
                if (Engine.rand() > 0.5)
                    w = -w;

                // place an element randomly along a circle of radius w
                var alpha = Engine.rand() * 2 * Math.PI
                var dx = Math.sin(alpha)
                var dy = Math.cos(alpha)
                pt.x += dx * w
                pt.y += dy * w
               
                api.translate(pt  - outputOrigin)
                
                var localScale = scale * (parameters.patternScaleFrom + (parameters.patternScaleTo - parameters.patternScaleFrom) * (1 - (Math.abs(w) / parameters.maxDistanceFromPath)) )

                api.scale(localScale, localScale)
                 
                if (parameters.applyRotation)
                {
                    var rotate = 360/30 * Math.floor(Engine.rand() * 30) // 30 distinct rotations
                    api.rotate(rotate)
                }
 
                //var seed =  Math.floor(l  *17 + p * 2531011) % 0x7fffffff   //7327)
 
                var rc = parameters.colorRandomness  // color randomness
                var br = 1 - parameters.brightnessRandomness + Engine.rand() * parameters.brightnessRandomness*2  // brightness
                api.Color (kFillColor, br *(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2))

                pattern.render(api)
                api.popMatrix()
            }
        }

        return // we are done
    }

    // to finetune the progress bar, specify how many pixels are covered by patterns
    api.setParameter(kpsPixelCoverage, parameters.density * 0.9 *
                              (parameters.patternScaleTo * parameters.patternScaleTo - parameters.patternScaleFrom * parameters.patternScaleFrom) / 3)


    // determine the number approximately based on size of the element and filled area
    var num = ((outputSize.x + patternSize.x) * (outputSize.y + patternSize.y)) / (patternSize.x * patternSize.y * scale *scale) * parameters.density
    if (num < 1)
        num = 1
 
    for (var n = 0; n < num; n++)
    {
        api.pushMatrix()
           
        var localScale = scale * (parameters.patternScaleFrom + (parameters.patternScaleTo - parameters.patternScaleFrom) * Engine.rand() )

        var spanx = outputSize.x + patternSize.x * localScale
        var spany = outputSize.y + patternSize.y * localScale
     
        var x =  -patternSize.x * localScale * 0.5 + spanx * Engine.rand()
        var y =  -patternSize.y * localScale * 0.5 + spany * Engine.rand()
            
        api.translate(x, y)
        api.scale(localScale, localScale)
         
        if (parameters.applyRotation)
        {
            var rotate = 360/30 * Math.floor(Engine.rand() * 30) // 30 distinct rotations
            api.rotate(rotate)
        }

        var rc = parameters.colorRandomness  // color randomness
        var br = 1 - parameters.brightnessRandomness + Engine.rand() * parameters.brightnessRandomness*2  // brightness
        api.Color (kFillColor, br *(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2))
        
        pattern.render(api)
        api.popMatrix()
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu start
// If you want to create a menu in which you change some of the script parameters, include the following code:
// For shipped scripts we can include localized strings, prefixed with $$$/ - call method localize(string) on the prefixed string
// For your own strings, you can query app.locale and select several language versions (for example, your native language and english). Try alert ("Language is: " + app.locale)

var decoMenu = {    //  an object that defines the menu
   menuTitle : localize("$$$/DecoScripts/RandomFill/RandomFill=Random Fill"),
   menuBackground : [0.93, 0.93, 0.93, 1],
   previewBackground : [1, 1, 1, 1],
   panels : [
    { panelName : "", 
       leftColumnWidth : 180,
       unitsWidth : 65,
       panelMenu : [
         { itemName : localize("$$$/DecoScripts/RandomFill/Density=Density:"),  
             itemUnit : localize("$$$/DecoScripts/Units/pixels=pixels"), 
             itemType : 'slider', itemValue : modelParameters.density, itemMin : 0.1, itemMax : 10, itemStep : 0.1, varName : 'density'  },
         
         { itemName : localize("$$$/DecoScripts/RandomFill/PatternScaleFrom=Minimum scale factor:"),  
             itemUnit : '', 
             itemType : 'slider', itemValue : modelParameters.patternScaleFrom, itemMin : 0.1, itemMax : 3, itemStep : 0.01, itemLEQitem : 2, varName : 'patternScaleFrom'  },
         
         // you can link two sliders together by using itemLEQitem : item_index (less or equal to value in item item_index) 
         // and itemGEQitem: item_index (). This can be used when teh two slides control min and max range for some random parameter.
         { itemName : localize("$$$/DecoScripts/RandomFill/PatternScaleTo=Maximum scale factor:"),  
             itemUnit : '', 
             itemType : 'slider', itemValue : modelParameters.patternScaleTo, itemMin : 0.1, itemMax : 3, itemStep : 0.01, itemGEQitem : 1, varName : 'patternScaleTo'  },
         
         { itemName : localize("$$$/DecoScripts/RandomFill/MaxDistanceToPath=Max distance from path:"),  
             itemUnit : localize("$$$/DecoScripts/Units/pixels=pixels"), 
             itemType : 'slider', itemValue : modelParameters.maxDistanceFromPath, itemMin : 0, itemMax : 250, itemStep : 1, enabled: inputPaths.length > 0, varName : 'maxDistanceFromPath'  },
         
          { itemName : localize("$$$/DecoScripts/RandomFill/RotatePattern=Rotate pattern:"),
              itemUnit : '', itemType : 'checkbox', itemValue : modelParameters.applyRotation, itemMin : 0, itemMax : 0, varName : 'applyRotation'  },

          { itemName : localize("$$$/DecoScripts/ColorRandomness=Color randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.colorRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'colorRandomness'  }, 
         
          { itemName : localize("$$$/DecoScripts/BrightnessRandomness=Brightness randomness:"),  
             itemUnit : "",  itemType : 'slider', itemValue : modelParameters.brightnessRandomness, itemMin : 0, itemMax : 1, itemStep : 0.01, varName : 'brightnessRandomness'  }, 
         
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

