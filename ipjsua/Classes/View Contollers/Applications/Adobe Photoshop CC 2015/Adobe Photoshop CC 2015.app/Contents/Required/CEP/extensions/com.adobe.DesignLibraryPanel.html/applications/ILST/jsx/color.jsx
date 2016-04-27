/*************************************************************************
 *
 * ADOBE CONFIDENTIAL
 * ___________________
 *
 *  Copyright 2013 Adobe Systems Incorporated
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Adobe Systems Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Adobe Systems Incorporated and its
 * suppliers and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Adobe Systems Incorporated.
 **************************************************************************/

/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50, sloppy: true, continue: true */
/*global $, Folder, DocumentColorSpace, app, RGBColor, File, SaveOptions, ImageColorSpace, ColorConvertPurpose, ExportOptionsSVG,
         SVGFontSubsetting, SVGFontType, ExportType, JSXGlobals, CMYKColor, GrayColor, LabColor, COLOR, UTIL, UnitValue, solidColorToData, dataToSolidColor,
         StyleRunAlignmentType, AlternateGlyphsForm, BaselineDirectionType, FontBaselineOption, FontCapsOption, FigureStyleType, AutoKernType,
         LanguageType, FontOpenTypePositionOption, WariChuJustificationType, TabStopInfo, Justification, AutoLeadingType, KinsokuOrderEnum, BurasagariTypeEnum */

var COLOR = {};
COLOR.isColorSupported = function (colorObject) {
    return colorObject.typename !== 'NoColor' &&
            colorObject.typename !== 'GradientColor' &&
            colorObject.typename !== 'PatternColor';
};

COLOR.isModeSupported = function (colorData) {
    return colorData && (colorData.mode === 'RGB' || colorData.mode === 'CMYK' || colorData.mode === 'Gray');
};

COLOR.colorToRGB = function colorToRGB(color) {
    var rgbColor = new RGBColor();
    var colors;
    switch (color.typename) {
    case 'RGBColor':
        rgbColor = color;
        break;
    case 'CMYKColor':
        colors = app.convertSampleColor(ImageColorSpace.CMYK, [color.cyan, color.magenta, color.yellow, color.black], ImageColorSpace.RGB, ColorConvertPurpose.dummypurpose);
        rgbColor.red = colors[0];
        rgbColor.green = colors[1];
        rgbColor.blue = colors[2];
        break;
    case 'LabColor':
        colors = app.convertSampleColor(ImageColorSpace.LAB, [color.l, color.a, color.b], ImageColorSpace.RGB, ColorConvertPurpose.dummypurpose);
        rgbColor.red = colors[0];
        rgbColor.green = colors[1];
        rgbColor.blue = colors[2];
        break;
    case 'GrayColor':
        colors = app.convertSampleColor(ImageColorSpace.GrayScale, [color.gray], ImageColorSpace.RGB, ColorConvertPurpose.dummypurpose);
        rgbColor.red = colors[0];
        rgbColor.green = colors[1];
        rgbColor.blue = colors[2];
        break;
    case 'SpotColor':
        return COLOR.colorToRGB(color.spot.color);
    }
    return rgbColor;
};

COLOR.colorToCMYK = function colorToCMYK(color) {
    var cmykColor = new CMYKColor();
    var colors;
    switch (color.typename) {
    case 'CMYKColor':
        cmykColor = color;
        break;
    case 'RGBColor':
        colors = app.convertSampleColor(ImageColorSpace.RGB, [color.red, color.green, color.blue], ImageColorSpace.CMYK, ColorConvertPurpose.dummypurpose);
        cmykColor.cyan = colors[0];
        cmykColor.magenta = colors[1];
        cmykColor.yellow = colors[2];
        cmykColor.black = colors[3];
        break;
    case 'LabColor':
        colors = app.convertSampleColor(ImageColorSpace.LAB, [color.l, color.a, color.b], ImageColorSpace.CMYK, ColorConvertPurpose.dummypurpose);
        cmykColor.cyan = colors[0];
        cmykColor.magenta = colors[1];
        cmykColor.yellow = colors[2];
        cmykColor.black = colors[3];
        break;
    case 'GrayColor':
        colors = app.convertSampleColor(ImageColorSpace.GrayScale, [color.gray], ImageColorSpace.CMYK, ColorConvertPurpose.dummypurpose);
        cmykColor.cyan = colors[0];
        cmykColor.magenta = colors[1];
        cmykColor.yellow = colors[2];
        cmykColor.black = colors[3];
        break;
    case 'SpotColor':
        return COLOR.colorToCMYK(color.spot.color);
    }
    return cmykColor;
};

COLOR.findRepWithMode = function (reps, mode) {
    var filteredReps = reps.filter(function (item) {
        return item.mode === mode;
    });
    if (filteredReps.length > 0) {
        return filteredReps[0];
    }
};


COLOR.getBestColorRepresentation = function (data) {
    var color = data[0];

    if (COLOR.isModeSupported(color)) {
        return color;
    }
    //Default to RGB if the primary color is not supported
    return COLOR.findRepWithMode(data, 'RGB');
};

COLOR.dataToSolidColor = function dataToSolidColor(data) {
    //If passed an array of representations then first select
    //the best representation
    if (Array.isArray(data)) {
        data = COLOR.getBestColorRepresentation(data);
    }

    // // This code does not work as expected creating a spot color with a name
    // // Does not set it to that spot color request will be issued with illustrator team
    // if (data.type === 'spot') {
    //     var spotColor = new SpotColor();

    //     spotColor.spot = app.activeDocument.spots.add();
    //     spotColor.spot.name = data.spotColorName;
    //     //spotColor.spot.color = COLOR._colorByMode(data);
    //     spotColor.spot.colorType = ColorModel.SPOT;

    //     switch (data.mode) {
    //     case 'RGB':
    //         spotColor.spot.spotKind = SpotColorKind.SpotRGB;
    //         break;
    //     case 'CMYK':
    //         spotColor.spot.spotKind = SpotColorKind.SpotCMYK;
    //         break;
    //     case 'LAB':
    //         spotColor.spot.spotKind = SpotColorKind.SpotLAB;
    //         break;
    //     }

    //     return spotColor;
    // }

    return COLOR._colorByMode(data);
};

//Returns a color object solely based on the color mode
COLOR._colorByMode = function _colorByMode(data) {
    var finalColor = new RGBColor();
    if (data) {
        switch (data.mode) {
        case 'RGB':
            finalColor = new RGBColor();
            finalColor.red = data.value.r;
            finalColor.green = data.value.g;
            finalColor.blue = data.value.b;
            break;
        case 'CMYK':
            finalColor = new CMYKColor();
            finalColor.cyan = data.value.c;
            finalColor.magenta = data.value.m;
            finalColor.yellow = data.value.y;
            finalColor.black = data.value.k;
            break;
        case 'Lab':
            finalColor = new LabColor();
            finalColor.l = data.value.l;
            finalColor.a = data.value.a;
            finalColor.b = data.value.b;
            break;
        case 'Gray':
            finalColor = new GrayColor();
            finalColor.gray = data.value;
            break;
        }
    }

    return finalColor;
};


COLOR.solidColorToData = function solidColorToData(color) {
    //If it is an unsupported color type then simply return undefined
    if (!COLOR.isColorSupported(color)) {
        return;
    }

    var profileName,
        representations = [];
    try {
        profileName = app.activeDocument.colorProfileName;
    } catch (ignore) {}

    // Handle spot color first since it is a special case
    if (color.typename === "SpotColor") {
        representations = solidColorToData(color.spot.color);
        return representations.map(function (colorItem) {
            colorItem.type = 'spot';
            colorItem.spotColorName = color.spot.name;
            return colorItem;
        });
    }

    var rgbColor;
    rgbColor = COLOR.colorToRGB(color);
    representations.push({
        mode: 'RGB',
        value: {
            r: rgbColor.red,
            g: rgbColor.green,
            b: rgbColor.blue
        },
        type: 'process',
        profileName: profileName
    });
    if (color.typename === "CMYKColor") {
        representations.unshift({
            mode: 'CMYK',
            value: {c: color.cyan, m: color.magenta, y: color.yellow, k: color.black},
            type: 'process',
            profileName: profileName
        });
    }

    if (color.typename === "LabColor") {
        representations.unshift({
            mode: 'Lab',
            value: {l: color.l, a: color.a, b: color.b},
            type: 'process',
            profileName: profileName
        });
    }

    if (color.typename === "GrayColor") {
        representations.unshift({
            mode: 'Gray',
            value: color.gray,
            type: 'process',
            profileName: profileName
        });
    }

    return representations;
};

COLOR.setColor = function (color) {
    try {
        if (app.documents.length === 0) {
            return;
        }

        if (app.isFillActive()) {
            COLOR.setFillColor(color);
        }

        if (app.isStrokeActive()) {
            COLOR.setStrokeColor(color);
        }

        return color + ";Fill Color-setFillColor;Stroke Color-setStrokeColor";

    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-setColor()', ex);
    }
};

COLOR.setFillColor = function (color) {
    try {
        var newColor = COLOR.dataToSolidColor(color);
        app.activeDocument.defaultFillColor = newColor;
    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-setFillColor()', ex);
    }
};

COLOR.setStrokeColor = function (color) {
    try {
        var newColor = COLOR.dataToSolidColor(color);
        app.activeDocument.defaultStrokeColor = newColor;
    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-setStrokeColor()', ex);
    }
};