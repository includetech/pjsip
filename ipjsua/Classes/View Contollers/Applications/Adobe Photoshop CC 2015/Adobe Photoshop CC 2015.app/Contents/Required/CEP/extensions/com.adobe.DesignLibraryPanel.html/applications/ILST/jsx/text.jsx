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
         SVGFontSubsetting, SVGFontType, ExportType, JSXGlobals, CMYKColor, GrayColor, TEXT, COLOR, UTIL, UnitValue,
         StyleRunAlignmentType, AlternateGlyphsForm, BaselineDirectionType, FontBaselineOption, FontCapsOption, FigureStyleType, AutoKernType,
         LanguageType, FontOpenTypePositionOption, WariChuJustificationType, TabStopInfo, Justification, AutoLeadingType, KinsokuOrderEnum, BurasagariTypeEnum */
var TEXT = {};

TEXT.saveTextStylePreview = function () {
    if (app.selection[0].typename === "TextFrame") {
        var blackClr = new RGBColor();
        blackClr.red = blackClr.green = blackClr.blue = 0;

        var previewPath = Folder.temp.fsName + "/TextPreview" + $.hiresTimer + ".png";
        app.selection[0].generateThumbnailWithTextFrameProperties(JSXGlobals.textPreviewString, JSXGlobals.textPreviewFontSize, blackClr, new File(previewPath));

        return previewPath;
    }
};

TEXT.isFontAvailable = function (style) {
    if (style.adbeFont) {
        try {
            if (app.textFonts.getByName(style.adbeFont.postScriptName)) {
                return true;
            }
        } catch (e) {
            return false;
        }
    } else if (style.fontFamily) {
        //If all we have is the font-family then try to use that
        var i, font, found = false;
        for (i = 0; i < app.textFonts.length; i++) {
            font = app.textFonts[i];
            if (font.family === style.fontFamily) {
                found = true;
                break;
            }
        }

        return found;
    }
    return false;
};


TEXT.setCharacterStyle = function (textRange, style) {
    var value, font;

    if (style.adbeFont) {

        try {
            font = app.textFonts.getByName(style.adbeFont.postScriptName);
            textRange.characterAttributes.textFont = font;
        } catch (e) {}

    } else if (style.fontFamily) {
        //If all we have is the font-family then try to use that
        var i;
        for (i = 0; i < app.textFonts.length; i++) {
            font = app.textFonts[i];
            if (font.family === style.fontFamily) {
                textRange.characterAttributes.textFont = font;
                break;
            }
        }
    }

    if (style.color) {
        textRange.characterAttributes.fillColor = COLOR.dataToSolidColor(style.color);
    }

    if (style.adbeIlstStrokeColor) {
        textRange.characterAttributes.strokeColor = COLOR.dataToSolidColor(style.adbeIlstStrokeColor);
    }
    /* We probably don't want to add a stroke color if there isn't one defined
    else if (!style.adbeIlstStrokeColor && style.color) {
        textRange.characterAttributes.strokeColor = COLOR.dataToSolidColor(style.color);
    }*/

    if (style.fontSize !== undefined) {
        var size = new UnitValue(style.fontSize.value, style.fontSize.type);
        textRange.characterAttributes.size = size.as('pt');
    }

    //CHARACTER

    //Enumerations
    if (style.adbeIlstAlignment) { value = style.adbeIlstAlignment.split('.', 2)[1]; textRange.characterAttributes.alignment = StyleRunAlignmentType[value]; }
    if (style.adbeIlstAlternateGlyphs) { value = style.adbeIlstAlternateGlyphs.split('.', 2)[1]; textRange.characterAttributes.alternateGlyphs = AlternateGlyphsForm[value]; }
    if (style.adbeIlstBaselineDirection) { value = style.adbeIlstBaselineDirection.split('.', 2)[1]; textRange.characterAttributes.baselineDirection = BaselineDirectionType[value]; }
    if (style.adbeIlstFigureStyle) { value = style.adbeIlstFigureStyle.split('.', 2)[1]; textRange.characterAttributes.figureStyle = FigureStyleType[value]; }
    if (style.adbeIlstKerningMethod) { value = style.adbeIlstKerningMethod.split('.', 2)[1]; textRange.characterAttributes.kerningMethod = AutoKernType[value]; }
    if (style.adbeIlstOpenTypePosition) { value = style.adbeIlstOpenTypePosition.split('.', 2)[1]; textRange.characterAttributes.openTypePosition = FontOpenTypePositionOption[value]; }
    if (style.adbeIlstWariChuJustification) {
        value = style.adbeIlstWariChuJustification.split('.', 2)[1];
        textRange.characterAttributes.wariChuJustification = WariChuJustificationType[value];
    }

    //booleans and numbers
    if (style.fontStyle === 'italics') { textRange.characterAttributes.italics = true; }
    if (style.adbeIlstAkiLeft !== undefined) { textRange.characterAttributes.akiLeft = style.adbeIlstAkiLeft; }
    if (style.adbeIlstAkiRight !== undefined) { textRange.characterAttributes.akiRight = style.adbeIlstAkiRight; }
    if (style.adbeAutoLeading) { textRange.characterAttributes.autoLeading = true; }
    if (style.baselineShift !== undefined) {
        var bShift = new UnitValue(style.baselineShift.value, style.baselineShift.type);
        textRange.characterAttributes.baselineShift = bShift.as('pt');
    }
    if (style.adbeIlstConnectionForms) { textRange.characterAttributes.connectionForms = style.adbeIlstConnectionForms; }
    if (style.adbeIlstContextualLigature) { textRange.characterAttributes.contextualLigature = style.adbeIlstContextualLigature; }
    if (style.adbeHorizontalScale !== undefined) { textRange.characterAttributes.horizontalScale = style.adbeHorizontalScale; }
    if (style.lineHeight !== undefined) {
        var lead = new UnitValue(style.lineHeight.value, style.lineHeight.type);
        textRange.characterAttributes.autoLeading = false;
        textRange.characterAttributes.leading = lead.as('pt');
    }

    if (style.whiteSpace && style.whiteSpace === 'nowrap') {
        textRange.characterAttributes.noBreak = true;
    }
    if (style.adbeIlstOverprintFill) { textRange.characterAttributes.overprintFill = style.adbeIlstOverprintFill; }
    if (style.adbeIlstOverprintStroke) { textRange.characterAttributes.overprintStroke = style.adbeIlstOverprintStroke; }
    if (style.adbeIlstProportionalMetrics) { textRange.characterAttributes.proportionalMetrics = style.adbeIlstProportionalMetrics; }
    if (style.adbeIlstRotation !== undefined) { textRange.characterAttributes.rotation = style.adbeIlstRotation; }


    //Only set stroke weight if color is available because it will add a default black color if we set it and color is not set
    if (style.adbeIlstStrokeWeight !== undefined && style.adbeIlstStrokeColor) {
        textRange.characterAttributes.strokeWeight = style.adbeIlstStrokeWeight;
    }
    if (style.adbeIlstTateChuYokoHorizontal !== undefined) { textRange.characterAttributes.tateChuYokoHorizontal = style.adbeIlstTateChuYokoHorizontal; }
    if (style.adbeIlstTateChuYokoVertical !== undefined) { textRange.characterAttributes.tateChuYokoVertical = style.adbeIlstTateChuYokoVertical; }

    //Use our Adobe Tracking property if its available but revert back to converting CSS letter-spacing otherwise
    if (style.adbeTracking !== undefined) {
        textRange.characterAttributes.tracking = style.adbeTracking;
    } else if (style.letterSpacing) {
        textRange.characterAttributes.tracking = style.letterSpacing.value * 1000;
    }

    //text-decoration underline and strike-through
    if (style.textDecoration && style.textDecoration.indexOf('underline') !== -1) {
        textRange.characterAttributes.underline = true;
    } else {
        textRange.characterAttributes.underline = false;
    }

    if (style.textDecoration && style.textDecoration.indexOf('line-through') !== -1) {
        textRange.characterAttributes.strikeThrough = true;
    } else {
        textRange.characterAttributes.strikeThrough = false;
    }


    if (style.adbeIlstTsume !== undefined) { textRange.characterAttributes.Tsume = style.adbeIlstTsume; }

    if (style.adbeVerticalScale !== undefined) { textRange.characterAttributes.verticalScale = style.adbeVerticalScale; }
    if (style.adbeIlstWariChuCharactersAfterBreak !== undefined) { textRange.characterAttributes.wariChuCharactersAfterBreak = style.adbeIlstWariChuCharactersAfterBreak; }
    if (style.adbeIlstWariChuCharactersBeforeBreak !== undefined) { textRange.characterAttributes.wariChuCharactersBeforeBreak = style.adbeIlstWariChuCharactersBeforeBreak; }
    if (style.adbeIlstWariChuEnabled) { textRange.characterAttributes.wariChuEnabled = style.adbeIlstWariChuEnabled; }
    if (style.adbeIlstWariChuLineGap !== undefined) { textRange.characterAttributes.wariChuLineGap = style.adbeIlstWariChuLineGap; }
    if (style.adbeIlstWariChuLines !== undefined) { textRange.characterAttributes.wariChuLines = style.adbeIlstWariChuLines; }
    if (style.adbeIlstWariChuScale !== undefined) { textRange.characterAttributes.wariChuScale = style.adbeIlstWariChuScale; }

    var hasFontFeatureSettings = style.fontFeatureSettings !== undefined && style.fontFeatureSettings.length > 0;

    textRange.characterAttributes.discretionaryLigature = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('dlig') !== -1;

    textRange.characterAttributes.contextualLigature = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('clig') !== -1;

    textRange.characterAttributes.ornaments = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('ornm') !== -1;

    textRange.characterAttributes.ordinals = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('ordn') !== -1;

    textRange.characterAttributes.swash = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('swsh') !== -1;

    textRange.characterAttributes.ligature = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('liga') !== -1;

    textRange.characterAttributes.fractions = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('frac') !== -1;

    textRange.characterAttributes.titling = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('titl') !== -1;

    textRange.characterAttributes.stylisticAlternates = hasFontFeatureSettings && style.fontFeatureSettings.indexOf('salt') !== -1;

    // Superscript subscript normal baseline
    if (hasFontFeatureSettings && style.fontFeatureSettings.indexOf('sups') !== -1) {
        textRange.characterAttributes.baselinePosition = FontBaselineOption.SUPERSCRIPT;
    } else if (hasFontFeatureSettings && style.fontFeatureSettings.indexOf('subs') !== -1) {
        textRange.characterAttributes.baselinePosition = FontBaselineOption.SUBSCRIPT;
    } else {
        textRange.characterAttributes.baselinePosition = FontBaselineOption.NORMALBASELINE;
    }

    // All small caps, small caps. all caps, normal
    if (hasFontFeatureSettings && style.fontFeatureSettings.indexOf('c2sc') !== -1) {
        textRange.characterAttributes.capitalization = FontCapsOption.ALLSMALLCAPS;
    } else if (hasFontFeatureSettings && style.fontFeatureSettings.indexOf('smcp') !== -1) {
        textRange.characterAttributes.capitalization = FontCapsOption.SMALLCAPS;
    } else if (style.textTransform === 'capitalize') {
        textRange.characterAttributes.capitalization = FontCapsOption.ALLCAPS;
    } else {
        textRange.characterAttributes.capitalization = FontCapsOption.NORMALCAPS;
    }


};


TEXT.getCharacterStyleObject = function (textRange) {
    var obj = {};


    if (COLOR.isColorSupported(textRange.characterAttributes.fillColor)) {
        obj.color = COLOR.solidColorToData(textRange.characterAttributes.fillColor);
    }

    // Only add stroke color if we have one
    if (COLOR.isColorSupported(textRange.characterAttributes.strokeColor)) {
        obj.adbeIlstStrokeColor = COLOR.solidColorToData(textRange.characterAttributes.strokeColor);
    }


    // The name provided with illustrator TextFont is the equivalent to the postScriptName in Photoshop
    obj.adbeFont = {
        family: textRange.characterAttributes.textFont.family,
        name: textRange.characterAttributes.textFont.name,
        postScriptName: textRange.characterAttributes.textFont.name,
        style: textRange.characterAttributes.textFont.style
    };

    obj.fontFamily = textRange.characterAttributes.textFont.family;
    obj.fontSize = {type: 'pt', value: textRange.characterAttributes.size};

    //Approximate font-style and font-weight for CSS
    var style = textRange.characterAttributes.textFont.style.toLowerCase();
    if (style.indexOf('italic') !== -1) {
        obj.fontStyle = 'italic';
    } else if (style.indexOf('oblique') !== -1) {
        obj.fontStyle = 'oblique';
    }

    if (style.indexOf('bold') !== -1) {
        obj.fontWeight = 'bold';
    }

    if (style.indexOf('light') !== -1 || style.indexOf('thin') !== -1) {
        obj.fontWeight = 'lighter';
    }


    //CHARACTER ATTRIBUTES

    //Enumerations
    obj.adbeIlstAlignment = textRange.characterAttributes.alignment.toString();
    try {
        obj.adbeIlstAlternateGlyphs = textRange.characterAttributes.alternateGlyphs.toString();
    } catch (e) {
        obj.adbeIlstAlternateGlyphs = AlternateGlyphsForm.DEFAULTFORM.toString();
    }
    obj.adbeIlstBaselineDirection = textRange.characterAttributes.baselineDirection.toString();

    obj.adbeIlstFigureStyle = textRange.characterAttributes.figureStyle.toString();
    try {
        obj.adbeIlstKerningMethod = textRange.characterAttributes.kerningMethod.toString();
    } catch (err) {
        obj.adbeIlstKerningMethod = AutoKernType.AUTO.toString();
    }

    obj.adbeIlstOpenTypePosition = textRange.characterAttributes.openTypePosition.toString();
    obj.adbeIlstWariChuJustification = textRange.characterAttributes.wariChuJustification.toString();

    //booleans and numbers
    obj.adbeIlstAkiLeft = textRange.characterAttributes.akiLeft;
    obj.adbeIlstAkiRight = textRange.characterAttributes.akiRight;

    if (textRange.characterAttributes.autoLeading) {
        obj.adbeAutoLeading = true;
    }


    if (textRange.characterAttributes.baselineShift !== undefined) {
        obj.baselineShift = {type: 'pt', value: textRange.characterAttributes.baselineShift};
    }

    obj.adbeIlstConnectionForms = textRange.characterAttributes.connectionForms;

    obj.adbeHorizontalScale = textRange.characterAttributes.horizontalScale;

    if (textRange.characterAttributes.italics) {
        obj.fontStyle = 'italics';
    }


    if (textRange.characterAttributes.leading !== undefined) {
        obj.lineHeight = {type: 'pt', value: textRange.characterAttributes.leading};
    }

    if (textRange.characterAttributes.noBreak) {
        obj.whiteSpace = 'nowrap';
    }
    obj.adbeIlstOverprintFill = textRange.characterAttributes.overprintFill;
    obj.adbeIlstOverprintStroke = textRange.characterAttributes.overprintStroke;
    obj.adbeIlstProportionalMetrics = textRange.characterAttributes.proportionalMetrics;
    obj.adbeIlstRotation = textRange.characterAttributes.rotation;


    //Only gather the stroke weight if there is a color specified
    if (obj.adbeIlstStrokeColor) {
        obj.adbeIlstStrokeWeight = textRange.characterAttributes.strokeWeight;
    }
    obj.adbeIlstTateChuYokoHorizontal = textRange.characterAttributes.tateChuYokoHorizontal;
    obj.adbeIlstTateChuYokoVertical = textRange.characterAttributes.tateChuYokoVertical;

    // OpenType feature settings
    obj.fontFeatureSettings = [];
    if (textRange.characterAttributes.swash) {
        obj.fontFeatureSettings.push('swsh');
    }


    if (textRange.characterAttributes.ordinals) {
        obj.fontFeatureSettings.push('ordn');
    }


    if (textRange.characterAttributes.ornaments) {
        obj.fontFeatureSettings.push('ornm');
    }


    if (textRange.characterAttributes.discretionaryLigature) {
        obj.fontFeatureSettings.push('dlig');
    }


    if (textRange.characterAttributes.contextualLigature) {
        obj.fontFeatureSettings.push('clig');
    }


    if (textRange.characterAttributes.ligature) {
        obj.fontFeatureSettings.push('liga');
    }


    if (textRange.characterAttributes.fractions) {
        obj.fontFeatureSettings.push('frac');
    }


    if (textRange.characterAttributes.titling) {
        obj.fontFeatureSettings.push('titl');
    }


    if (textRange.characterAttributes.stylisticAlternates) {
        obj.fontFeatureSettings.push('salt');
    }

    if (textRange.characterAttributes.baselinePosition === FontBaselineOption.SUPERSCRIPT) {
        obj.fontFeatureSettings.push('sups');
    }

    if (textRange.characterAttributes.baselinePosition === FontBaselineOption.SUBSCRIPT) {
        obj.fontFeatureSettings.push('subs');
    }

    if (textRange.characterAttributes.capitalization === FontCapsOption.ALLSMALLCAPS) {
        obj.fontFeatureSettings.push('c2sc');
    } else if (textRange.characterAttributes.capitalization === FontCapsOption.SMALLCAPS) {
        obj.fontFeatureSettings.push('smcp');
    } else if (textRange.characterAttributes.capitalization === FontCapsOption.ALLCAPS) {
        obj.textTransform = 'capitalize';
    }

    //If we have no open type settings delete the empty array
    if (obj.fontFeatureSettings.length === 0) {
        delete obj.fontFeatureSettings;
    }


    if (textRange.characterAttributes.tracking !== undefined) {
        obj.adbeTracking = textRange.characterAttributes.tracking;
    }


    if (obj.adbeTracking) {
        obj.letterSpacing = {type: 'em', value: (obj.adbeTracking / 1000.0).toFixed(2)};
    }

    obj.adbeIlstTsume = textRange.characterAttributes.Tsume;

    // text-decoration properties
    if (textRange.characterAttributes.underline) {
        if (obj.textDecoration) {
            obj.textDecoration.push('underline');
        } else {
            obj.textDecoration = ['underline'];
        }
    }

    //Strike through or in CSS speak line-through
    if (textRange.characterAttributes.strikeThrough) {
        if (obj.textDecoration) {
            obj.textDecoration.push('line-through');
        } else {
            obj.textDecoration = ['line-through'];
        }
    }

    obj.adbeVerticalScale = textRange.characterAttributes.verticalScale;
    obj.adbeIlstWariChuCharactersAfterBreak = textRange.characterAttributes.wariChuCharactersAfterBreak;
    obj.adbeIlstWariChuCharactersBeforeBreak = textRange.characterAttributes.wariChuCharactersBeforeBreak;
    obj.adbeIlstWariChuEnabled = textRange.characterAttributes.wariChuEnabled;
    obj.adbeIlstWariChuLineGap = textRange.characterAttributes.wariChuLineGap;
    obj.adbeIlstWariChuLines = textRange.characterAttributes.wariChuLines;
    obj.adbeIlstWariChuScale = textRange.characterAttributes.wariChuScale;

    return obj;

};

TEXT.setOrCreateFont = function (typography, createLayer) {
    'use strict';
    try {
        if (app.documents.length === 0) {
            return;
        }

        var selection = app.selection;
        var hasTextElements = false;
        var i;
        for (i = 0; i < selection.length; i++) {
            if (selection[i].hasOwnProperty("characters")) {
                hasTextElements = true;
            }
        }

        var size = new UnitValue(typography.fontSize ? typography.fontSize.value : 12, typography.fontSize ? typography.fontSize.type : 'pt');
        var fontSize = size.as('pt');

        if (createLayer) {

            var textRef = app.activeDocument.textFrames.add();
            textRef.contents = typography.fontFamily;

            var bounds = {
                'top': 0,
                'left': 0
            };
            var position = UTIL.getArtboardPosition(app.activeDocument.artboards[0].artboardRect, [20, fontSize], bounds);

            textRef.left = position[0];
            textRef.top = position[1];
            textRef.selected = true;

            selection = app.selection = [textRef];

            hasTextElements = true;
        }


        if (hasTextElements) {
            for (i = 0; i < selection.length; i++) {
                if (selection[i].hasOwnProperty("characters")) {
                    TEXT.setCharacterStyle(selection[i].textRange, typography);
                }
            }
        }
    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-setOrCreateFont()', ex);
    }
};

TEXT.setFont = function (typography) {
    'use strict';
    try {
        TEXT.setOrCreateFont(typography, false);
    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-setFont()', ex);
    }
};

TEXT.createFontLayer = function (typography) {
    try {
        TEXT.setOrCreateFont(typography, true);
    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-createFontLayer()', ex);
    }
};

TEXT.getCharacterColors = function (textRange) {
    var i, range, colors = [];


    for (i = 0; i < textRange.textRanges.length; i++) {
        range = textRange.textRanges[i];
        if (COLOR.isColorSupported(range.characterAttributes.fillColor)) {
            colors.push(COLOR.solidColorToData(range.characterAttributes.fillColor));
        }

        // Only add stroke color if we have one
        if (COLOR.isColorSupported(range.characterAttributes.strokeColor)) {
            colors.push(COLOR.solidColorToData(range.characterAttributes.strokeColor));
        }
    }
    return colors;
};

TEXT.replaceTextStyle = function (info) {
    try {
        var docName = "textstyle-" + info.id;
        var newDoc = app.documents.add(DocumentColorSpace.RGB, 400, 100);
        newDoc.layers[0].name = 'Text Layer';

        var textRef = newDoc.textFrames.add();
        textRef.contents = "Text Style";
        textRef.left = 5;
        textRef.top = 50;
        textRef.selected = true;

        $._ext_ILST.setFont(info.content[0]);

        var color = info.content[0].color;
        var newColor = new RGBColor();
        newColor.red = color.red;
        newColor.green = color.green;
        newColor.blue = color.blue;

        app.activeDocument.defaultFillColor = newColor;

        newDoc.activeView.zoom = 1;
        var tmpFile = new File($._ext_CORE.getTempFolder() + "/" + docName + ".ai");
        newDoc.saveAs(tmpFile);

        return newDoc.name;
    } catch (ex) {
        $._ext_CORE.writeToLog('ILST.jsx-replaceTextStyle()', ex);
    }
};
