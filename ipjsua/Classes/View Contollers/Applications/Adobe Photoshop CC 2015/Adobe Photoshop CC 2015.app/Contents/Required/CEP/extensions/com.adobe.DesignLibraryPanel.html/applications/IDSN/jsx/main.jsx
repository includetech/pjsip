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
/*globals $, app, ToolTipOptions, COLOR, ColorSpace, JSXGlobals, File, Folder, ExportFormat, StrokeFillTargetOptions, TEXT, UndoModes, ColorModel */

var extensionPath = $.fileName.split('/').slice(0, -1).join('/') + '/';
$.evalFile(extensionPath + 'color.jsx');
$.evalFile(extensionPath + 'text.jsx');

$._ext_IDSN = {
    setThumbnailExportOptions: function () {
        //Set options on application
        app.setCloudLibraryOptions(JSXGlobals.previewMaxWidth, JSXGlobals.previewMaxHeight);
    },
    setLibraryCollection: function (data) {
        app.setCloudLibraryCollection(data);
    },
    getTooltipState: function () {
        if (app.generalPreferences.toolTips !== ToolTipOptions.NONE) {
            return 'true';
        }
        return 'false';
    },
    getIMSUserID: function () {
        return app.userGuid;
    },
    isAnalyticsEnabled: function () {
        if (app.isUserSharingAppUsageData()) {
            return 'true';
        }
        return 'false';
    },
    openAssetForEdit: function (filePath) {
        try {
            var result = app.openCloudAssetForEdit(new File(filePath));
            if (result) {
                return app.activeDocument.name;
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-openAsset()', ex);
        }
    },
    placeAsset: function (filePath, isPSD, libraryName, itemName, elementRef, modifiedTime, creationTime, adobeStockId, adobeStockLicense, isLinked) {
        try {
            var doc;
            try {
                if (app.documents.length !== 0) {
                    //app.activeDocument throws if the only documents that are open, are opened in background.
                    doc = app.activeDocument;
                }
            } catch (ignore) {}

            if (!doc) {
                doc = app.documents.add();
            }

            var obj = {};
            obj.elementRef = elementRef;
            if (adobeStockId) {
                obj.adobeStock = {};
                obj.adobeStock.id = adobeStockId;
                obj.adobeStock.license = adobeStockLicense;
            }

            obj.isLinkedAsset = isLinked;
            var args = JSON.stringify(obj);

            doc.placeCloudAsset(args);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-placeAsset()', ex);
        }
    },
    setColor: COLOR.setColor,
    setFillColor: COLOR.setFillColor,
    setStrokeColor: COLOR.setStrokeColor,
    getLayerColor: function () {
        try {
            return JSON.stringify(JSXGlobals.colorModifiedByUser);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getLayerColor()', ex);
        }
    },
    replaceColor: function (colorData) {
        try {
            var colorPickerData,
                originalColorValue = {};
            var colorChanged = false;

            originalColorValue = COLOR.dataToColorValue(colorData);
            colorPickerData = app.invokeColorPicker(originalColorValue.space, originalColorValue.vals);

            if (colorPickerData && colorPickerData !== 'undefined') {
                var modifiedColor = JSON.parse(colorPickerData);
                var modifiedColorSpace;
                if (modifiedColor.colorSpace === 'RGB') {
                    modifiedColorSpace = ColorSpace.RGB;
                } else if (modifiedColor.colorSpace === 'CMYK') {
                    modifiedColorSpace = ColorSpace.CMYK;
                } else if (modifiedColor.colorSpace === 'Lab') {
                    modifiedColorSpace = ColorSpace.LAB;
                }

                colorChanged = (modifiedColorSpace !== originalColorValue.space);
                if (!colorChanged) {
                    colorChanged = !COLOR.areColorValsEqual(modifiedColor.colorVals, originalColorValue.vals);
                }

                originalColorValue.space = modifiedColorSpace;
                originalColorValue.vals = modifiedColor.colorVals;
            }

            JSXGlobals.colorModifiedByUser = COLOR.colorValueToData(originalColorValue);
            return colorChanged ? 'true' : 'false';
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-replaceColor()', ex);
        }
    },
    isFontAvailable: TEXT.isFontAvailable,
    setFont: TEXT.setFont,
    createTextLayer: TEXT.createFontLayer,
    saveTextStylePreview: TEXT.saveTextStylePreview,
    getLayerName: function () {
        try {
            var finalName = '';
            // If the selection has exactly one item, and that item has a name, then return that.
            // Otherwise, return the empty string.
            if (app.selection.length === 1 && app.selection[0] && app.selection[0].name !== "") {
                finalName = app.selection[0].name;
            }
            return finalName;
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getLayerName()', ex);
            return '';
        }
    },
    getLayerInfo: function () {
        try {
            var layerObject = {};
            var layerColors = [];

            var areEqual = function (colorData1, colorData2) {
                return colorData1 && colorData2 && JSON.stringify(colorData1[0].value) === JSON.stringify(colorData2[0].value);
            };

            var pushUnique = function (colorData, colorType, colorName) {
                if (colorData === undefined) {
                    return;
                }
                var index;
                for (index = 0; index < layerColors.length; index++) {
                    if (areEqual(layerColors[index].data, colorData)) {
                        return;
                    }
                }
                layerColors.push({'colorType': colorType, 'data': colorData, 'name': colorName});
            };

            try {
                //With selection as document structure node, or a page, the stroke/fill proxy gets
                //disabled, and asserts when accessed. We report no color or selection in such cases.
                var selectionClass = app.selection[0].__class__;
                if (selectionClass === 'XMLElement' || selectionClass === 'XMLAttribute' ||
                        selectionClass === 'XMLComment' || selectionClass === 'XMLInstruction' ||
                        selectionClass === 'Page') {
                    return; //undefined
                }
            } catch (ignore) {
                //Possibly no-doc state,  move on.
            }

            var fillTooltip = JSXGlobals.FILL;
            var strokeTooltip = JSXGlobals.STROKE;

            var selectionTarget = app.strokeFillProxySettings.target;
            if (selectionTarget === 'undefined') {
                return;
            }

            if (selectionTarget === StrokeFillTargetOptions.formattingAffectsText) {
                // We are targeting text color when 'Formatting affects text' is ON, either explicitly,
                // or implicitly by virtue of text selection.
                fillTooltip = JSXGlobals.ID_TEXT_FILL;
                strokeTooltip = JSXGlobals.ID_TEXT_STROKE;
            }

            var colorValue;
            try {
                var fillColor = app.strokeFillProxySettings.fillColor;
                colorValue = COLOR.colorToColorValue(fillColor);
                if (colorValue) {
                    pushUnique(COLOR.colorValueToData(colorValue), fillTooltip, colorValue.name);
                }
            } catch (ignore1) {}

            try {
                var strokeColor = app.strokeFillProxySettings.strokeColor;
                colorValue = COLOR.colorToColorValue(strokeColor);
                if (colorValue) {
                    pushUnique(COLOR.colorValueToData(colorValue), strokeTooltip, colorValue.name);
                }
            } catch (ignore2) {}

            layerObject.colors = layerColors;

            //Starting with selectionExists as false. For no doc state, app.selection below
            //throws, and would otherwise result in 'Add Graphic' being enabled.
            layerObject.selectionExists = false;

            try {
                var selection = app.selection;

                layerObject.selectionExists = (selection && selection.length > 0);
                layerObject.textSelectionExists = false;

                if (selection.length > 0) {
                    if (app.selection[0].__class__ === 'Guide') {
                        //If the selection is only guides, we disable 'Add Graphic'.
                        //Checking just the first item is good enough as guides can't be
                        //selected along with other page items.
                        layerObject.selectionExists = false;
                    } else if (app.selection[0].__class__ === 'Cell') {
                        //If the selection is only table cells, we disable 'Add Graphic'.
                        //Checking just the first item is good enough as table cells can't be
                        //selected along with other page items.
                        layerObject.selectionExists = false;
                    }
                }

                layerObject.kind = '';
                if (selection.length === 1 && selection[0] && selection[0].hasOwnProperty('characters')) {
                    if (selection[0].__class__ !== 'TextFrame' && selection[0].__class__ !== 'Cell') {
                        // If we have text selection, turn off selectionExists to avoid enabling of 'Add Graphic'
                        // in the DL panel. Add a new flag textSelectionExists in that case to still allow application
                        // of color to text.
                        layerObject.selectionExists = false;
                        layerObject.textSelectionExists = true;

                        var contents = $._ext_CORE.shortenString(selection[0].contents);
                        layerObject.text = (contents !== '') ? contents : ' '; // For enabling 'Add Text Style' for InsertionPoint selection
                    } else {
                        layerObject.text = $._ext_CORE.shortenString(selection[0].contents);
                    }

                    //We no longer need to collect text attributes for IDSN.
                    //layerObject.fontInfo = TEXT.getTextAttributesObject(selection[0].texts[0]);
                    layerObject.kind = 'LayerKind.TEXT';

                    var text = selection[0].texts[0];
                    layerObject.isAppliedCharStyleSupported = TEXT.canAddTypeStyle(text, true);
                    layerObject.isAppliedParaStyleSupported = TEXT.canAddTypeStyle(text, false);
                }

                layerObject.name = 'None';
                if (layerObject.selectionExists) {
                    if (selection[0] && selection[0].name !== '') {
                        layerObject.name = selection[0].name;
                        layerObject.fullName = selection[0].name;
                    } else {
                        if (app.activeDocument.activeLayer) {
                            layerObject.name = app.activeDocument.activeLayer.name;
                            layerObject.fullName = app.activeDocument.activeLayer.name;
                        }
                    }
                    layerObject.name = $._ext_CORE.shortenString(layerObject.name, false, JSXGlobals.maxNameLength);
                }
            } catch (ignore3) {}

            return JSON.stringify(layerObject);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getLayerInfo()', ex);
        }
    },
    getCurrentState: function () {
        if (app.documents.length > 0) {
            var docPath = '';
            try {
                if (app.activeDocument.saved) {
                    docPath = app.activeDocument.fullName;
                } else {
                    // Not saved even once
                    docPath = app.activeDocument.name;
                }
            } catch (ignore) {}

            if (docPath !== '') {
                //We are not using the LayerID anymore. Hence hardcode it.
                // TODO: Safely get rid of layerID after MAX
                return JSON.stringify({
                    'path': docPath,
                    'layerID': 2
                });
            }
        }
        return JSON.stringify({
            'path': '',
            'layerID': -1
        });
    },
    saveAssets: function (info, generateSecondaryFormat, dragAssetId) {
        try {
            var fileData = {};
            fileData.files = [];

            //Generate renditions for DL
            //IDMS (InDesign snippet) as primary, PDF as alternate, and PNG for DL local rendition.
            var basePath = Folder.temp.fsName + '/' + info.name;

            var idmsPath = basePath + '.idms';
            var idmsFile = new File(idmsPath);
            app.exportSelectionForCloudLibrary(idmsFile);

            if (!idmsFile.exists) {
                throw 'Could not extract selection.';
            }

            //Add snippet as primary rendition
            fileData.files.push({path: idmsPath, relationship: 'primary'});

            //Add PDF as alternate rendition
            var pdfPath = basePath + '.pdf';
            var pdfFile = new File(pdfPath);
            if (!pdfFile.exists) {
                throw 'Could not extract selection.';
            }

            fileData.files.push({path: pdfPath, relationship: 'rendition'});

            //Add the DL rendition
            var pngPath = basePath + '.png';
            var pngFile = new File(pngPath);
            if (pngFile.exists) {
                fileData.rendition = pngPath;
            }

            fileData.layerName = $._ext_IDSN.getLayerName();

            // TODO: Set layerIds and documentId on fileData if you want it to be returned with the
            // "com.adobe.DesignLibraries.events.ElementCreated" CSEvent that is sent
            // after element creation.

            return JSON.stringify(fileData);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-saveAssets()', ex);
        }
    },
    getColorDataFromJSON: function (data) {
        //convert a colorValue represented as JSON data to colorData
        //sent by host in drag/drop data, color related events etc.
        var colorObj = JSON.parse(data);

        var colorValue = {};
        colorValue.name = '';

        var colorSpace = colorObj.colorSpace;
        if (colorSpace === 'CMYK') {
            colorValue.space = ColorSpace.CMYK;
        } else if (colorSpace === 'RGB') {
            colorValue.space = ColorSpace.RGB;
        } else if (colorSpace === 'LAB') {
            colorValue.space = ColorSpace.LAB;
        } else {
            return; //undefined
        }

        colorValue.vals = colorObj.colorValues;

        var colorModel = colorObj.colorModel;
        if (colorModel === 'spot') {
            colorValue.model = ColorModel.SPOT;
            colorValue.name = colorObj.colorName;
        } else if (colorModel === 'process') {
            colorValue.model = ColorModel.PROCESS;
        } else {
            return; //undefined
        }

        var colorData = COLOR.colorValueToData(colorValue);
        return JSON.stringify(colorData);
    },
    getCharStyleInfo: function () {
        try {
            var styleObj = TEXT.getTypeStyleInfo(true);
            return JSON.stringify(styleObj);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getCharStyleInfo()', ex);
        }
    },
    getParaStyleInfo: function () {
        try {
            var styleObj = TEXT.getTypeStyleInfo(false);
            return JSON.stringify(styleObj);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getParaStyleInfo()', ex);
        }
    },
    getCharStyleInfoByID : function (documentID, styleID) {
        try {
            var styleObj = TEXT.getTypeStyleInfoByID(documentID, styleID, true);
            return JSON.stringify(styleObj);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getCharStyleInfoByID()', ex);
        }

    },
    getParaStyleInfoByID :  function (documentID, styleID) {
        try {
            var styleObj = TEXT.getTypeStyleInfoByID(documentID, styleID, false);
            return JSON.stringify(styleObj);
        } catch (ex) {
            $._ext_CORE.writeToLog('IDSN.jsx-getParaStyleInfoByID()', ex);
        }
    }
};
