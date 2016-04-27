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
/*global $, Folder, DocumentColorSpace, app, File, SaveOptions, ImageColorSpace, ColorConvertPurpose, ExportOptionsSVG,
         SVGFontSubsetting, SVGFontType, ExportType, JSXGlobals, CMYKColor, GrayColor, LabColor, TEXT, UTIL, COLOR, ExportOptionsPNG24, Dimensions, OpenOptions */


var extensionPath = $.fileName.split('/').slice(0, -1).join('/') + '/';
$.evalFile(extensionPath + 'util.jsx');
$.evalFile(extensionPath + 'color.jsx');
$.evalFile(extensionPath + 'text.jsx');


function padZero(str) {
    return "000000".substr(str.length) + str;
}

function getThumbnailOptions() {
    var options = new ExportOptionsPNG24();
    options.antiAliasing = true;
    options.transparency = true;
    var dimn = new Dimensions();
    dimn.width = JSXGlobals.previewMaxWidth;
    dimn.height = JSXGlobals.previewMaxHeight;
    options.dimensions = dimn;

    return options;
}

$._ext_ILST = {
    loadAndSelectBrush: function (filePath, brushName) {
        try {
            app.activeDocument.brushes.add(new File(filePath), brushName);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-loadAndSelectBrush()', ex);
        }
    },
    replaceColor: function (colorData) {
        try {
            var originalColor = COLOR.dataToSolidColor(colorData);
            var modifiedColor = app.showColorPicker(originalColor);
            JSXGlobals.colorModifiedByUser = modifiedColor;
            return (originalColor !== modifiedColor);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-replaceColor()', ex);
        }
    },
    replaceTextStyle: TEXT.replaceTextStyle,
    isFontAvailable: TEXT.isFontAvailable,
    placeAsset: function (filePath, isPSD, libraryName, itemName, elementRef, modifiedTime, creationTime, adobeStockId, adobeStockLicense, isLinked) {
        // https://forums.adobe.com/message/4896032
        // Place doesn't necessarily give you the exact artwork that is in the file,
        // copy/paste is the only way to get this.
        try {
            var assetFile = new File(filePath);
            if (!app || app.documents.length === 0 || !app.activeDocument) {
                // Need to make a copy of the file to avoid overwrite
                var fileExtension = filePath.substr(filePath.lastIndexOf(".") + 1).toLowerCase();
                var destPath = Folder.temp.fsName + '/' + new Date().valueOf() + '.' + fileExtension;

                assetFile.copy(destPath);
                $._ext_ILST.openAsset(destPath);
                return;
            }

            app.activeDocument.importFile(assetFile, isLinked, libraryName, itemName, elementRef,
                modifiedTime, creationTime, adobeStockId, adobeStockLicense);

        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-placeAsset()', ex);
        }
    },
    openAsset: function (filePath) {
        try {
            var newDoc = app.open(new File(filePath));
            return newDoc.name;
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-openAsset()', ex);
        }
    },
    openAssetForEdit: function (filePath, thumbnailPath) {
        if (thumbnailPath) {
            try {
                var options = new OpenOptions();
                options.addToRecentFiles = false;
                var newDoc = app.openCloudLibraryAssetForEditing(new File(filePath), new File(thumbnailPath), options);
                return newDoc.name;
            } catch (ex) {
                $._ext_CORE.writeToLog('ILST.jsx-openAssetForEdit()', ex);
            }
        } else {
            return $._ext_ILST.openAsset(filePath);
        }
    },
    setColor: COLOR.setColor,
    setFillColor: COLOR.setFillColor,
    setStrokeColor: COLOR.setStrokeColor,
    setFont: TEXT.setFont,
    createFontLayer: TEXT.createFontLayer,
    saveTextStylePreview: TEXT.saveTextStylePreview,
    getCurrentState: function () {
        try {
            if (app.documents.length > 0) {
                //if (app.selection && app.selection.length > 0) {
                var selectedLayer = app.activeDocument.activeLayer;

                var docPath = app.activeDocument.name;
                if (app.activeDocument.fullName) {
                    docPath = app.activeDocument.fullName.fsName;
                }
                /*var pageItems = selectedLayer.pageItems;
                var fullSelection = "";
                var i;
                for (i = 0; i < pageItems.length; i++) {
                    if (pageItems[i].selected === true) {
                        fullSelection += $._ext_ILST.getItemID(pageItems[i]) + pageItems[i].typename + ", ";
                    }
                }
                var layerID = $._ext_ILST.getItemID(app.selection[0]) + "_" + $._ext_ILST.getItemID(selectedLayer) + "===" + fullSelection;*/

                //We are not using the LayerID anymore. Hence hardcode it.
                // TODO: Safely get rid of layerID after MAX
                return JSON.stringify({
                    'path': docPath,
                    'layerID': 2
                });
                //}
            }
            return JSON.stringify({
                'path': '',
                'layerID': -1
            });
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-getCurrentState()', ex);
        }
    },
    getItemID: function (layer) {
        try {
            var layerID = layer.name;
            if (layer.hasOwnProperty("zOrderPosition")) {
                try {
                    layerID += "_" + layer.zOrderPosition;
                } catch (ignore) {}
            }
            return layerID;
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-getItemID()', ex);
        }
    },
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
            $._ext_CORE.writeToLog('ILST.jsx-getLayerName()', ex);
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

            var pushUnique = function (colorData, colorType) {
                if (colorData === undefined) {
                    return;
                }
                var index;
                for (index = 0; index < layerColors.length; index++) {
                    if (areEqual(layerColors[index].data, colorData)) {
                        return;
                    }
                }
                layerColors.push({'colorType': colorType, 'data': colorData});
            };

            var fillTooltip = JSXGlobals.FILL;
            var strokeTooltip = JSXGlobals.STROKE;
            if (app.selection) {
                var selection = app.selection;
                layerObject.kind = "";
                if (selection.length === 1 && selection[0] && selection[0].hasOwnProperty("characters") && selection[0].contents !== "") {
                    layerObject.text = $._ext_CORE.shortenString(selection[0].contents);
                    layerObject.fontInfo = TEXT.getCharacterStyleObject(selection[0].textRange);
                    layerObject.kind = "LayerKind.TEXT";
                    fillTooltip = JSXGlobals.AI_TEXT_FILL;
                    strokeTooltip = JSXGlobals.AI_TEXT_STROKE;
                }

                try {
                    var fillColor = app.activeDocument.defaultFillColor;
                    pushUnique(COLOR.solidColorToData(fillColor), fillTooltip);
                } catch (ignore) {}

                try {
                    var strokeColor = app.activeDocument.defaultStrokeColor;
                    pushUnique(COLOR.solidColorToData(strokeColor), strokeTooltip);
                } catch (ignore1) {}

                layerObject.colors = layerColors;

                // Looping through the pageItems is taking too long. Hence commenting it out
                /*var selectedLayer = app.activeDocument.activeLayer;
                var pageItems = selectedLayer.pageItems;
                var numItemsSelected = 0;
                var i;
                for (i = 0; i < pageItems.length; i++) {
                    if (pageItems[i].selected === true) {
                        numItemsSelected++;
                    }
                }*/

                layerObject.name = "None";

                // && pageItems.length !== numItemsSelected
                if (selection[0] && selection[0].name !== "") {
                    layerObject.name = selection[0].name;
                    layerObject.fullName = selection[0].name;
                } else {
                    if (app.activeDocument.activeLayer) {
                        layerObject.name = app.activeDocument.activeLayer.name;
                        layerObject.fullName = app.activeDocument.activeLayer.name;
                    }
                }
                layerObject.name = $._ext_CORE.shortenString(layerObject.name, false, JSXGlobals.maxNameLength);
                layerObject.selectionExists = (app.selection && app.selection.length > 0);

                //If we happen to have a selection that is not an array but a TextRange object
                //then we are in text edit mode and should report no selection
                if (selection && selection.typename === 'TextRange') {
                    layerObject.selectionExists = false;
                }
            }
            return JSON.stringify(layerObject);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-getLayerInfo()', ex);
        }
    },
    getLayerColor: function () {
        try {
            var colorObject = COLOR.solidColorToData(JSXGlobals.colorModifiedByUser);
            return JSON.stringify(colorObject);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-getLayerColor()', ex);
        }
    },
    getLayerFontStyle: function (docName) {
        try {
            var docRef = $._ext_CORE.getDocument(docName);
            if (docRef) {
                docRef.layers[0].textFrames[0].selected = true;
                var layerObject = {};
                var selection = app.selection;
                var selectionFont = selection[0].textRange.characterAttributes.textFont;

                layerObject.text = $._ext_CORE.shortenString(selection[0].contents);
                layerObject.fontInfo = selectionFont.family + ";" + selectionFont.style + ";" + selection[0].textRange.characterAttributes.size;
                layerObject.textColor = COLOR.solidColorToData(app.activeDocument.defaultFillColor).color;
                return JSON.stringify(layerObject);
            }
            return undefined;
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-getLayerFontStyle()', ex);
        }
    },
    getLayerBounds: function () {
        try {
            var selection = app.activeDocument.selection;
            var artboardRect = app.activeDocument.artboards[0].artboardRect;
            var firstItem = UTIL.getRealPosition(artboardRect, selection[0].position);
            var bounds;
            var i;
            var itemBounds, controlBounds;
            for (i = 0; i < selection.length; i++) {
                itemBounds = UTIL.getRealPosition(artboardRect, selection[i].position);
                controlBounds = selection[i].controlBounds;

                itemBounds.width = Math.abs(controlBounds[0] - controlBounds[2]);
                itemBounds.height = Math.abs(controlBounds[1] - controlBounds[3]);
                itemBounds.right = itemBounds.left + itemBounds.width;
                itemBounds.bottom = itemBounds.top + itemBounds.height;

                if (i === 0) {
                    bounds = itemBounds;
                    continue;
                }

                if (itemBounds.top < bounds.top) {
                    bounds.top = itemBounds.top;
                }
                if (itemBounds.left < bounds.left) {
                    bounds.left = itemBounds.left;
                }
                if (itemBounds.right > bounds.right) {
                    bounds.right = itemBounds.right;
                }
                if (itemBounds.bottom > bounds.bottom) {
                    bounds.bottom = itemBounds.bottom;
                }
            }

            if (bounds) {
                bounds.width = bounds.right - bounds.left;
                bounds.height = bounds.bottom - bounds.top;
            }

            var info = {};
            info.name = $._ext_CORE.shortenString(selection[0].name, false, JSXGlobals.maxNameLength);
            info.bounds = bounds;
            info.artboard = artboardRect;
            info.firstItem = firstItem;

            return JSON.stringify(info);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-getLayerBounds()', ex);
        }
    },
    saveAsSVG: function (filePath) {
        try {
            var svgFile = new File(filePath);
            var exportOptions = new ExportOptionsSVG();
            exportOptions.embedRasterImages = true;
            exportOptions.embedAllFonts = true;
            exportOptions.fontSubsetting = SVGFontSubsetting.GLYPHSUSED;
            exportOptions.fontType = SVGFontType.SVGFONT;
            app.activeDocument.exportFile(svgFile, ExportType.SVG, exportOptions);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-saveAsSVG()', ex);
        }
    },
    saveAsPNG: function (filePath, exportFullFile) {
        try {
            var pngFile = new File(filePath);

            var options = getThumbnailOptions();
            if (exportFullFile) {
                app.activeDocument.exportFile(pngFile, ExportType.PNG24, options);
            } else {
                app.activeDocument.exportSelectionAsPNG(pngFile, options);
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-saveAsPNG()', ex);
        }
    },
    setThumbnailExportOptions: function () {
        try {
            // TODO: Enable this when exportCloudLibraryAsset() API bugs are fixed
            //app.setThumbnailOptionsForCloudLibrary(getThumbnailOptions());
        } catch (ex) { }
    },
    saveAssets: function (info, generateSecondaryFormat, dragAssetId) {
        try {
            var fileData = {};
            fileData.files = [];
            /*var svgPath;
            if (generateSecondaryFormat) {
                // ** Save as SVG file **
                svgPath = Folder.temp.fsName + "/" + info.name + ".svg";
                $._ext_ILST.saveAsSVG(svgPath);
            }*/

            var pngPath = Folder.temp.fsName + "/" + info.name + ".png";
            $._ext_ILST.saveAsPNG(pngPath);
            fileData.rendition = pngPath;

            //If we couldn't create the PNG file correctly then there is no rendition
            var checkFile = new File(pngPath);
            if (!checkFile.exists) {
                delete fileData.rendition;
            }

            // ** Save as AI file **
            var aiPath = Folder.temp.fsName + "/" + info.name + ".ai";
            var aiFile = new File(aiPath);
            //app.activeDocument.saveAs(aiFile);
            app.activeDocument.exportSelectionAsAi(aiFile);

            checkFile = new File(aiPath);
            if (!checkFile.exists) {
                throw "Could note extract selection.";
            }

            // TODO: Enable this when exportCloudLibraryAsset() API bugs are fixed
            //app.exportCloudLibraryAsset(aiFile, new File(pngPath), dragAssetId);

            fileData.files.push({path: aiPath, relationship: 'primary'});

            /*if (svgPath) {
                fileData.files.push({path: svgPath, relationship: 'alternate'});
            }*/

            fileData.layerName = $._ext_ILST.getLayerName();

            // TODO: Set layerIds and documentId on fileData if you want it to be returned with the
            // "com.adobe.DesignLibraries.events.ElementCreated" CSEvent that is sent
            // after element creation.

            return JSON.stringify(fileData);
        } catch (ex) {
            $._ext_CORE.writeToLog('ILST.jsx-saveAssets()', ex);
        }
    },
    getTooltipState: function () {
        return app.preferences.getBooleanPreference('showToolTips');
    },
    getIMSUserID: function () {
        return app.userGUID;
    },
    isAnalyticsEnabled: function () {
        return app.isUserSharingAppUsageData();
    },
    closeDocument: function () {
        return app.activeDocument.close();
    }
};

