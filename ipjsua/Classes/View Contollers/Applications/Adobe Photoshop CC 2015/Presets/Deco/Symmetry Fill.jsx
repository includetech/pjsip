///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here are a few parameters that you can change to modify the behavior of the patterm
// Feel free to modify the values, don't change the variable names

modelParameters = {
    // scaling of the input pattern
    patternScale : 1,
    
    // This variable defines the type of symmetry.:
    //   Try   6, 7(*),16, 19, 21(*), 23, 26(*),  27, 29, 31(*), 32(**)
    symmetryType : 32, //  use an integer value between 0 and 32. The default is 32.
    // Here is the list of supported symmetries
    //     0: line reflection
    //     1: point reflection, frameZero
    //     2: rotation symmetry with 4 elements around the center
    //     3: translation symmetry
    //     4: glide reflection

    //     5: dilatation symmetry
    //     6: dilative rotation symmetry
    //     7: a different dilative rotation  symmetry
    //     8: infinite dilative rotation symmetry

    // The Frieze tilings (1D tilings on an infinite line
    //     9:  translation frieze
    //   10: glide reflection frieze
    //   11: translation line reflection frieze
    //   12: translation mirror reflection frieze
    //   13: translation point reflection frieze
    //   14: translation double reflection frieze
    //   15: glide reflection rotation frieze

    // The Wallpaper tilings 2D tilings on an infinite plane
    //   16: wallpaperP1 symmetry
    //   17: wallpaperP2 symmetry
    //   18: wallpaperPM symmetry
    //   19: wallpaperPG symmetry
    //   20: wallpaperPMM symmetry
    //   21: wallpaperCM symmetry
    //   22: wallpaperPMG symmetry
    //   23: wallpaperPGG symmetry
    //   24: wallpaperCMM symmetry
    //   25: wallpaperP4 symmetry
    //   26: wallpaperP4M symmetry
    //   27: wallpaperP4G symmetry
    //  28: wallpaperP3 symmetry
    //  29: wallpaperP3M1 symmetry
    //  30: wallpaperP31M symmetry
    //  31: wallpaperP6 symmetry
    //  32: wallpaperP6M symmetry

    // Pattern translation (in percantage of width and height)
    // By modifying this value you will change the layout of the pattern.
    // Try the following pairs of values: 0, 0 or 0, 75 or 75, 0
    // Note that the behavior is different when your pattern is thin in x or y .
    patternTranslationX : 75,   // The default is  75
    patternTranslationY : 75,   // The default is  75

    // Variation of color of the pattern. 
    // For example, value of 0.2 means that each of the red, green, and blue color components
    // will be multiplied by a DIFFERENT random value from interval 0.8 and 1.2. 
    // Set to 0 if you do not want to modify the pattern color.
    colorRandomness : 0.05,    // use a value between 0 and 1. The default is 0.05.

    // Variation of pattern brightness. 
    // For example, value of 0.6 means that each of the red, green, and blue color components
    // will be multiplied by THE SAME random value from interval 0.4 and 1.6. 
    // Set to 0 if you do not want to modify the pattern brightness.
    brightnessRandomness : 0.1   // use a value between 0 and 1. The default is 0.1.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// You can modify the code below but keep in mind that as with any scripting
// you can break things. Keep a backup copy.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


var pattern = RenderAPI.getParameter(kpsPattern)


function PatternModule(parameters, scale, patternSize) 
{
    this.parameters = parameters
    this.scale = scale
    this.patternSize = patternSize
}


PatternModule.prototype.render = function (api) 
{
        api.pushMatrix()
        // Set the random seed based on the current position - this assures that the color will be modified
        // in the same way for the pattern in the neighboring selected area
        var currentFrame = api.getFrame()
        var seed = (Math.floor(currentFrame.position().x * 2531011)  + Math.floor(currentFrame.position().y * 2531011)) % 2147483647
        if (this.parameters.symmetryType >= 0 && this.parameters.symmetryType <= 8)
            seed = Math.random()*2531011
       //Engine.message ("rendering at position " + currentFrame.position().x + ", " + currentFrame.position().y)
 
        var rc = this.parameters.colorRandomness  // color randomness
        var br = 1 - this.parameters.brightnessRandomness + Engine.rand(seed) * this.parameters.brightnessRandomness*2  // brightness
        api.Color (kFillColor, br *(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2), br*(1 - rc + Engine.rand()*rc*2))

        api.translateRel (this.patternSize.x * this.parameters.patternTranslationX * 0.01, this.patternSize.y * this.parameters.patternTranslationY * 0.01) 
        api.scale(this.scale)
        if (skipRotation)
        {
            var angle = 180 / Math.PI * Math.atan2 (currentFrame.heading().x, currentFrame.heading().y)  // get current rotation
            api.rotate (-angle)    // negate the rotation
        }
        //api.scale(this.parameters.patternScale)
        pattern.render(api)
        api.popMatrix()
 }




var frameZero = new Frame2();
var frameOrigin = new Frame2();
frameOrigin.setPosition(0, 0);
frameOrigin.rotateDeg(0.0);

//symmetry.frame = frameOrigin;  // no need to set the frame if it is a default frame at position (0,0) and no rotation

function setSymmetry(symmetry, parameters, patternSize)
{

	switch (parameters.symmetryType)
	{
	case 0:
		symmetry.set(kSymmetryLineReflection, frameZero, kTransformToLocal);
		break;
	case 1:
		symmetry.set(kSymmetryPointReflection, frameZero, kTransformToLocal);
		break;
	case 2:
         // Type, number of elements around the center, frame specifying local position of an element, optional center of rotation (must be a point)
		symmetry.set(kSymmetryRotation, 4, frameZero);
		break;
	case 3:
		symmetry.set(kSymmetryTranslation, 4, 1.0, frameZero, kTransformToLocal);
		break;
	case 4:
		symmetry.set(kSymmetryGlideReflection, 5, 4.0, frameZero, kTransformToLocal);
		break;


		// Number, initial angle, angle increment, initial scale, scale ratio
	case 5:
		symmetry.set(kSymmetryDilatation, 600, 1.0, 1.0 / 1.01, frameZero, kTransformToLocal); 
		break;
	case 6:
		symmetry.set(kSymmetryDilativeRotation, 600, 0.0, kFibonacciAngleInDegrees, 1.0, 1.0 / 1.01, frameZero, kTransformToLocal); 
		break;
	case 7:
		symmetry.set(kSymmetryDilativeRotation, 200, 0.0, kFibonacciAngleInDegrees, 0.0333, 1.02, frameZero, kTransformToLocal); 
		break;
	case 8:
		symmetry.set(kSymmetryInfiniteDilativeRotation, 100, 0.0, kFibonacciAngleInDegrees, 0.5, 1.01, frameZero, kTransformToLocal); 

	}

	var uStep = patternSize.y * 1.25;
	var vStep = patternSize.x * 1.25;

	// The Frieze tilings (1D tilings on an infinite line
	switch (parameters.symmetryType)
	{
	case 9:
		symmetry.set(kSymmetryFriezeTranslation, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 10:
		symmetry.set(kSymmetryFriezeGlideReflection, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 11:
		symmetry.set(kSymmetryFriezeTranslationLineReflection, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 12:
		symmetry.set(kSymmetryFriezeTranslationMirrorReflection, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 13:
		symmetry.set(kSymmetryFriezeTranslationPointReflection, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 14:
		symmetry.set(kSymmetryFriezeTranslationDoubleReflection, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 15:
		symmetry.set(kSymmetryFriezeGlideReflectionRotation, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	}

	uStep = uStep //* 2.5;
	vStep = vStep //* 2.5

	// The Wallpaper tilings 2D tilings on an infinite plane
	switch (parameters.symmetryType)
	{
	case 16:
		symmetry.set(kSymmetryWallpaperP1, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 17:
		symmetry.set(kSymmetryWallpaperP2, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 18:
		symmetry.set(kSymmetryWallpaperPM, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 19:
		symmetry.set(kSymmetryWallpaperPG, uStep, vStep, frameZero, kTransformToLocal);
		break;
	}

	uStep = uStep * 1.5;
	vStep = vStep * 1.5;

	switch (parameters.symmetryType)
	{
	case 20:
		symmetry.set(kSymmetryWallpaperPMM, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 21:
		symmetry.set(kSymmetryWallpaperCM, uStep, vStep, frameZero, kTransformToLocal);
		break;
	}

	uStep = uStep * 0.75;
	vStep = vStep * 0.75;
	switch (parameters.symmetryType)
	{
	case 22:
		symmetry.set(kSymmetryWallpaperPMG, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 23:
		symmetry.set(kSymmetryWallpaperPGG, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 24:
		symmetry.set(kSymmetryWallpaperCMM, uStep, vStep, frameZero, kTransformToLocal);
		break;
	case 25:
		symmetry.set(kSymmetryWallpaperP4, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 26:
		symmetry.set(kSymmetryWallpaperP4M, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 27:
		symmetry.set(kSymmetryWallpaperP4G, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	}

	uStep = uStep * 1.75;
	vStep = vStep * 1.75;
	switch (parameters.symmetryType)
	{
	case 28:
		symmetry.set(kSymmetryWallpaperP3, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 29:
		symmetry.set(kSymmetryWallpaperP3M1, Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 30:
		symmetry.set(kSymmetryWallpaperP31M, Math.max(uStep, vStep), frameZero, kTransformToLocal); 
		break;
	case 31:
		symmetry.set(kSymmetryWallpaperP6,  Math.max(uStep, vStep), frameZero, kTransformToLocal);
		break;
	case 32:
		symmetry.set(kSymmetryWallpaperP6M, Math.max(uStep, vStep), frameZero, kTransformToLocal);
	break;
	}

	return 1
}

var skipRotation = false 

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
    var maxPatternSize = Math.max ( patternSize.x, patternSize.y) // before patternScale is applied
    patternSize *= parameters.patternScale
    
     if (parameters == previewParameters)
    {
        // set scale so that we show about 10-15 patterns
        if (maxPatternSize < previewSize / 5) 
            scale = previewSize / 5 / maxPatternSize
        else if (maxPatternSize > previewSize / 4)
            scale = previewSize / 4 / maxPatternSize
    }

   scale *= parameters.patternScale

    // scale up if the patternsize is 1 to avoid long loop
    if (patternSize.x == 1 && patternSize.y == 1)
    {
        if (scale == 1)
            scale = 20
        patternSize *= scale
        skipRotation = true    // 1x1 patterns that are scaled up do not rotate gracefully
    }

     // to finetune the progress bar, specify how many pixels are covered by patterns - the bigger the spacing the lower the value, for example
    api.setParameter(kpsPixelCoverage, 1.5)
 
    var patternModule = new PatternModule(parameters, scale, patternSize * scale);

    // Symmetry - module should be added to the system before modules that 
    // are part of the symmetry
    var symmetry = new Symmetry;

    // set the symmetry type
    setSymmetry (symmetry, parameters, patternSize * scale )


    Engine.addModule (symmetry)

    Engine.addModule (patternModule)

    // Add to the symmetry
    symmetry.addModule (patternModule)

    if (parameters.symmetryType >= 16)
        // Setting the bounding box like this will insure that neighboring fills are aligned for 2D tilings
        api.setSceneBBox (outputOrigin.x, outputOrigin.x + outputSize.x,  outputOrigin.y,  outputOrigin.y + outputSize.y)
    else
    {
        // Other symmetries will be centered around the center of the selection bounding box

        api.setSceneBBox (-outputSize.x/2,  outputSize.x/2,  -outputSize.y/2,  outputSize.y/2)
      }
 
    if (parameters == previewParameters)
    {
        api.pushMatrix()
        Engine.render (api)
        api.popMatrix()
     }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu start
// If you want to create a menu in which you change some of the script parameters, include the following code:
// For shipped scripts we can include localized strings, prefixed with $$$/ - call method localize(string) on the prefixed string
// For your own strings, you can query app.locale and select several language versions (for example, your native language and english). Try alert ("Language is: " + app.locale)

var decoMenu = {    //  an object that defines the menu
   menuTitle : localize("$$$/DecoScripts/SymmetryFill/SymmetryFill=Symmetry Fill"),
   menuBackground : [0.93, 0.93, 0.93, 1],
   previewBackground : [1, 1, 1, 1],
   panels : [
    { panelName : "", 
       leftColumnWidth : 200,  
       unitsWidth : 65, 
       dropdownlistWidth : 200,
       panelMenu : [
         { itemName : localize("$$$/DecoScripts/SymmetryFill/SymmetryType=Symmetry type:"),  
             itemUnit : '', 
             itemType : 'dropdownlist',  itemList  : 
[
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry1= 1: line reflection"),  image : "Symmetry Fill/s0.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry2= 2: point reflection"),  image : "Symmetry Fill/s1.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry3= 3: rotation symmetry with 4 elements around the center"),  image : "Symmetry Fill/s2.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry4= 4: translation symmetry"),  image : "Symmetry Fill/s3.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry5= 5: glide reflection"),  image : "Symmetry Fill/s4.png" },

{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry6= 6: dilatation symmetry"),  image : "Symmetry Fill/s5.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry7= 7: dilative rotation symmetry"),  image : "Symmetry Fill/s6.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry8= 8: a different dilative rotation  symmetry"),  image : "Symmetry Fill/s7.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry9= 9: infinite dilative rotation symmetry"),  image : "Symmetry Fill/s8.png" },

// The Frieze tilings (1D tilings on an infinite line
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry10=10: translation frieze"),  image : "Symmetry Fill/s9.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry11=11: glide reflection frieze"),  image : "Symmetry Fill/s10.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry12=12: translation line reflection frieze"),  image : "Symmetry Fill/s11.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry13=13: translation mirror reflection frieze"),  image : "Symmetry Fill/s12.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry14=14: translation point reflection frieze"),  image : "Symmetry Fill/s13.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry15=15: translation double reflection frieze"),  image : "Symmetry Fill/s14.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry16=16: glide reflection rotation frieze"),  image : "Symmetry Fill/s15.png" },

// The Wallpaper tilings 2D tilings on an infinite plane
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry17=17: wallpaperP1 symmetry"),  image : "Symmetry Fill/s16.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry18=18: wallpaperP2 symmetry"),  image : "Symmetry Fill/s17.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry19=19: wallpaperPM symmetry"),  image : "Symmetry Fill/s18.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry20=20: wallpaperPG symmetry"),  image : "Symmetry Fill/s19.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry21=21: wallpaperPMM symmetry"),  image : "Symmetry Fill/s20.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry22=22: wallpaperCM symmetry"),  image : "Symmetry Fill/s21.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry23=23: wallpaperPMG symmetry"),  image : "Symmetry Fill/s22.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry24=24: wallpaperPGG symmetry"),  image : "Symmetry Fill/s23.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry25=25: wallpaperCMM symmetry"),  image : "Symmetry Fill/s24.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry26=26: wallpaperP4 symmetry"),  image : "Symmetry Fill/s25.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry27=27: wallpaperP4M symmetry"),  image : "Symmetry Fill/s26.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry29=28: wallpaperP4G symmetry"),  image : "Symmetry Fill/s27.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry30=29: wallpaperP3 symmetry"),  image : "Symmetry Fill/s28.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry31=30: wallpaperP3M1 symmetry"),  image : "Symmetry Fill/s29.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry32=31: wallpaperP31M symmetry"),  image : "Symmetry Fill/s30.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry33=32: wallpaperP6 symmetry"),  image : "Symmetry Fill/s31.png" },
{ item: localize("$$$/DecoScripts/SymmetryFill//Symmetry34=33: wallpaperP6M symmetry"),  image : "Symmetry Fill/s32.png" },
],
             itemValue : modelParameters.symmetryType, itemMin : 0, itemMax : 0, varName : 'symmetryType'  },
         
         { itemName : localize("$$$/DecoScripts/PatternScale=Pattern Scale:") ,  
             itemUnit :  "", itemType : 'slider', itemValue : modelParameters.patternScale, itemMin : 0.25, itemMax : 1.25, itemStep : 0.01, varName : 'patternScale'  }, 

        { itemName : localize("$$$/DecoScripts/SymmetryFill/PatternTranslationX=Pattern translation along width:") ,
             itemUnit : localize("$$$/DecoScripts/Units/percentOfWidth=% of width"),  
             itemType : 'slider', itemValue : modelParameters.patternTranslationX, itemMin : -100, itemMax : 100, itemStep : 1, varName : 'patternTranslationX'  }, 
         
          { itemName : localize("$$$/DecoScripts/SymmetryFill/PatternTranslationY=Pattern translation along height:") ,
             itemUnit : localize("$$$/DecoScripts/Units/percentOfHeight=% of height"),  
             itemType : 'slider', itemValue : modelParameters.patternTranslationY, itemMin : -100, itemMax : 100, itemStep : 1, varName : 'patternTranslationY'  }, 
         
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
