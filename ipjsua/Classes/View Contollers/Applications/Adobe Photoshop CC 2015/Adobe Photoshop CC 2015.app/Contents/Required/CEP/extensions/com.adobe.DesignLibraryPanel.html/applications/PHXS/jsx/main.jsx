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

/*jslint vars: true, plusplus: true, devel: true, nomen: true, regexp: true, indent: 4, maxerr: 50, sloppy: true, continue: true, unparam: true */
/*global $, Folder, app, DocumentFill, ActionDescriptor, ActionReference, DialogModes, File, UnitValue,
         TypeUnits, ActionList, SolidColor, executeAction, executeActionGet, PhotoshopSaveOptions, SaveOptions, PNGSaveOptions,
         LayerKind, DescValueType, cssToClip, svg, ColorModel, JSXGlobals, TEXT, COLOR, BRUSH, LAYERSTYLE, PSClass, PSEnum, PSType,
         PSForm, PSUnit, PSString, PSKey, PSEvent, PurgeTarget */
var params = {};


var cTID = function (s) {
    if (app.charIDToTypeID) {
        return app.charIDToTypeID(s);
    }
};
var sTID = function (s) {
    if (app.stringIDToTypeID) {
        return app.stringIDToTypeID(s);
    }
};

var tTSID = function (tid) {
    if (app.typeIDToStringID) {
        return app.typeIDToStringID(tid);
    }
};


//Include our other scripts order is important
var extensionPath = $.fileName.split('/').slice(0, -1).join('/') + '/';
$.evalFile(extensionPath + 'constants.jsx');
$.evalFile(extensionPath + 'util.jsx');
$.evalFile(extensionPath + 'color.jsx');
$.evalFile(extensionPath + 'brush.jsx');
$.evalFile(extensionPath + 'text.jsx');
$.evalFile(extensionPath + 'layerstyle.jsx');

function getPropertyFromDesc(propertykey, desc) {
    if (desc && desc.hasKey(propertykey)) {
        switch (desc.getType(propertykey)) {
        // Add handling of new types here as needed
        case DescValueType.OBJECTTYPE:
            return desc.getObjectValue(propertykey);
        case DescValueType.BOOLEANTYPE:
            return desc.getBoolean(propertykey);
        }
    }
    return null;
}

function getAppProperty(propertyKey) {
    var ref = new ActionReference();
    ref.putProperty(PSClass.Property, propertyKey);
    ref.putEnumerated(PSClass.Application, PSType.Ordinal, PSEnum.Target);
    var appDesc = app.executeActionGet(ref);
    return getPropertyFromDesc(propertyKey, appDesc);
}

function getLayerProperty(propertyKey) {
    var ref = new ActionReference();
    ref.putProperty(PSClass.Property, propertyKey);
    ref.putEnumerated(PSClass.Layer, PSType.Ordinal, PSEnum.Target);
    var layerDesc = executeActionGet(ref);
    return getPropertyFromDesc(propertyKey, layerDesc);
}

function duplicateLayer() {
    var desc1 = new ActionDescriptor();
    var ref1 = new ActionReference();
    ref1.putClass(PSClass.Document);
    desc1.putReference(PSString.Null, ref1);
    var ref2 = new ActionReference();
    ref2.putEnumerated(PSClass.Layer, PSType.Ordinal, PSEnum.Target);
    desc1.putReference(PSKey.Using, ref2);
    desc1.putInteger(PSClass.Version, 5);
    executeAction(PSEvent.Make, desc1, DialogModes.NO);
}

function duplicateLayerInsideDocument() {
    var desc1 = new ActionDescriptor();
    var ref1 = new ActionReference();
    ref1.putEnumerated(PSClass.Layer, PSType.Ordinal, PSEnum.Target);
    desc1.putReference(PSString.Null, ref1);
    desc1.putInteger(PSClass.Version, 5);
    executeAction(PSEvent.Duplicate, desc1, DialogModes.NO);
}

function insertSmartObject(filePath) {
    var desc1 = new ActionDescriptor();
    desc1.putPath(PSString.Null, new File(filePath));
    desc1.putEnumerated(PSKey.FreeTransformCenterState, PSType.QuadCenterState, PSEnum.QCSAverage);
    var desc2 = new ActionDescriptor();
    desc2.putUnitDouble(PSKey.Horizontal, PSUnit.Pixels, 20);
    desc2.putUnitDouble(PSKey.Vertical, PSUnit.Pixels, 20);
    desc1.putObject(PSKey.Offset, PSKey.Offset, desc2);
    executeAction(PSEvent.Place, desc1, DialogModes.NO);
}

function cropUsingPercentage(bounds) {
    var desc1 = new ActionDescriptor();
    var desc2 = new ActionDescriptor();
    desc2.putUnitDouble(PSKey.Top, PSUnit.Percent, bounds.top);
    desc2.putUnitDouble(PSKey.Left, PSUnit.Percent, bounds.left);
    desc2.putUnitDouble(PSKey.Bottom, PSUnit.Percent, bounds.bottom);
    desc2.putUnitDouble(PSKey.Right, PSUnit.Percent, bounds.right);
    desc1.putObject(PSKey.To, PSClass.Rectangle, desc2);
    desc1.putUnitDouble(PSKey.Angle, PSUnit.Angle, 0);
    desc1.putBoolean(PSEvent.Delete, true);
    desc1.putEnumerated(sTID("cropAspectRatioModeKey"), sTID("cropAspectRatioModeClass"), sTID("pureAspectRatio"));
    desc1.putBoolean(PSKey.ConstrainProportions, false);
    executeAction(PSEvent.Crop, desc1, DialogModes.NO);
}


// Works only in Photoshop 15.1 & above
function getSelectedLayerIndicesOrIDs(wantIDs) {
    // Work-around for screwy layer indexing.
    var backgroundIndexOffset = 1;
    try {
        // This throws an error if there's no background
        if (app.activeDocument.backgroundLayer) {
            backgroundIndexOffset = 0;
        }
    } catch (ignore) {}

    var ktargetLayers = wantIDs ? sTID('targetLayersIDs') : sTID('targetLayers');

    var resultLayerIndices = [];
    var ref = new ActionReference();
    var args = new ActionDescriptor();
    ref.putProperty(PSClass.Property, ktargetLayers);
    ref.putEnumerated(PSClass.Document, PSType.Ordinal, PSEnum.Target);
    args.putReference(PSString.Null, ref);
    var resultDesc = executeAction(cTID('getd'), args, DialogModes.NO);

    if (!resultDesc.hasKey(ktargetLayers)) {
        return [];
    }

    var selIndexList = resultDesc.getList(ktargetLayers);
    var i;
    for (i = 0; i < selIndexList.count; ++i) {
        if (wantIDs) {
            resultLayerIndices.push(selIndexList.getReference(i).getIdentifier(PSClass.Layer));
        } else {
            resultLayerIndices.push(selIndexList.getReference(i).getIndex(PSClass.Layer) + backgroundIndexOffset);
        }
    }

    return resultLayerIndices;
}

function getSelectedLayerIndices() {
    return getSelectedLayerIndicesOrIDs(false);
}

function getSelectedLayerIDs() {
    return getSelectedLayerIndicesOrIDs(true);
}

function getNumSelectedLayers() {
    var indices = [];
    try {
        indices = getSelectedLayerIndices();
    } catch (e) { return false; }

    return indices.length;
}


function importLayers(filePath, isPSD, libraryName, itemName, elementRef, modifiedTime, adobeStockId, adobeStockLicense, isLinked) {
    var placeDesc = new ActionDescriptor();
    placeDesc.putPath(PSKey.Target, new File(filePath));

    if (itemName) {
        placeDesc.putString(PSKey.LAYER_NAME, itemName);
    }

    var elementDesc = new ActionDescriptor();

    if (elementRef) {
        elementDesc.putString(PSKey.ELEMENT_REF, elementRef);
    }

    if (modifiedTime) {
        elementDesc.putDouble(PSKey.DATE_MODIFIED, modifiedTime);
    }

    if (itemName) {
        elementDesc.putString(PSKey.Name, itemName);
    }

    if (libraryName) {
        elementDesc.putString(PSKey.LIBRARY_NAME, libraryName);
    }

    if (adobeStockId) {
        elementDesc.putString(PSKey.ADOBE_STOCK_ID, adobeStockId);
        elementDesc.putEnumerated(PSKey.ADOBE_STOCK_LICENSE_STATE, PSKey.ADOBE_STOCK_LICENSE_STATE, adobeStockLicense ? PSKey.Licensed : PSKey.Unlicensed);
    }

    placeDesc.putObject(PSKey.LIB_ELEMENT, PSKey.LIB_ELEMENT, elementDesc);

    if (isLinked) {
        placeDesc.putBoolean(PSKey.LINKED, isLinked);
    } else {
        placeDesc.putBoolean(PSKey.UNWRAP_LAYERS, true);
    }

    executeAction(PSKey.PLACE_EVENT, placeDesc, DialogModes.ERROR);
}



// Makes an action list containing a reference to selected sheets
function makeSelectedSheetsTargetSheetReferenceList() {
    //Create an action refernece containing all of the sheets we want to export
    var targetSheetsRef = new ActionReference();
    targetSheetsRef.putEnumerated(PSKey.LayerKey, PSKey.OrdinalKey, PSKey.TargetEnum);

    // Put the reference containing the sheets into a list, cause that's how it's done
    var refList = new ActionList();
    refList.putReference(targetSheetsRef);

    return refList;
}

// Makes a save descriptor that describes the location, name, format, options, etc
// for the file that the layers will be saved to
// saveFile: a File object indicating where the file shoudl be saved to
function makePSDSaveDescriptor(fileObject) {
    var saveDesc = new ActionDescriptor();

    // Format options
    var formatOptDesc = new ActionDescriptor();
    formatOptDesc.putBoolean(PSKey.MaximizeCompatibility, true);

    saveDesc.putObject(PSKey.As, PSKey.Photoshop35Format, formatOptDesc);
    saveDesc.putPath(PSKey.IN, fileObject);
    saveDesc.putBoolean(PSKey.LowerCase, true);

    return saveDesc;
}

function makeRepresentationDescriptor(saveFolder, saveFileNameNoExt) {
    var representationDesc = new ActionDescriptor();
    representationDesc.putPath(PSString.IN, saveFolder);
    representationDesc.putString(PSKey.Name, saveFileNameNoExt);

    return representationDesc;
}

function makeExternalPreviewDescriptor(saveFile, maxWidth, maxHeight) {
    var externalPreviewDesc = new ActionDescriptor();
    externalPreviewDesc.putPath(PSString.IN, saveFile);
    externalPreviewDesc.putInteger(PSString.PixelWidth, maxWidth);
    externalPreviewDesc.putInteger(PSString.PixelHeight, maxHeight);

    return externalPreviewDesc;
}

$._ext_PHXS = {
    replaceColor: COLOR.replaceColor,
    setFont: TEXT.setFont,
    createFontLayer: TEXT.createFontLayer,
    replaceTextStyle: TEXT.replaceTextStyle,
    isFontAvailable: TEXT.isFontAvailable,
    saveTextStylePreview: TEXT.saveTextStylePreview,
    loadAndSelectBrush: BRUSH.loadAndSelectBrush,
    saveLayerStyle: LAYERSTYLE.saveLayerStyle,
    applyLayerStyle: LAYERSTYLE.applyLayerStyle,
    refreshFonts: function () {
        app.refreshFonts();
    },
    showFileSelectionDialog: function () {
        var selectedFiles = app.openDialog();
        var filePaths = [];
        var f;
        for (f = 0; f < selectedFiles.length; f++) {
            filePaths.push({'name': selectedFiles[f].name, 'path': selectedFiles[f].fsName});
        }
        return JSON.stringify(filePaths);
    },
    placeAsset: function (filePath, isPSD, libraryName, itemName, elementRef, modifiedTime, creationTime, adobeStockId, adobeStockLicense, isLinked) {
        try {
            if (app && app.documents.length > 0) {
                importLayers(filePath, isPSD, libraryName, itemName, elementRef, modifiedTime, adobeStockId, adobeStockLicense, isLinked);
            } else {
                // Need to make a copy of the file to avoid overwrite
                var srcFile = new File(filePath);
                var fileExtension = filePath.substr(filePath.lastIndexOf(".") + 1).toLowerCase();
                var destPath = Folder.temp.fsName + '/' + new Date().valueOf() + '.' + fileExtension;
                srcFile.copy(destPath);

                app.open(new File(destPath));
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-placeAsset()', ex);
        }
    },
    openAsset: function (filePath) {
        try {
            var newDoc = app.open(new File(filePath));
            return newDoc.name;
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-openAsset()', ex);
        }
    },
    openAssetForEdit: function (filePath, previewPath) {
        try {
            var openDesc = new ActionDescriptor();
            openDesc.putPath(PSString.Null, new File(filePath));

            // Tell PS to generate an updated preview whenever the file is saved
            var pngFile = new File(previewPath);
            var previewParams = makeExternalPreviewDescriptor(pngFile, JSXGlobals.previewMaxWidth, JSXGlobals.previewMaxHeight);
            openDesc.putObject(PSString.ExternalPreview, PSString.ExternalPreview, previewParams);
            openDesc.putEnumerated(PSKey.FileOpenContext, PSType.Ordinal, PSEnum.FileOpenContextCCLibraries);

            // Suppresses file choosing dialog while allowing format options
            openDesc.putBoolean(PSKey.OverrideOpen, true);

            // Tell PS to open the file
            executeAction(PSString.Open, openDesc, DialogModes.ALL);
            return app.activeDocument.name;
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-openAssetForEdit()', ex);
        }
    },
    getLayerBounds: function () {
        try {
            var selectedLayer = app.activeDocument.activeLayer;
            var allBounds;
            var info = {};
            info.name = $._ext_CORE.shortenString(selectedLayer.name, false, JSXGlobals.maxNameLength);
            info.bounds = { };

            // We don't need to figure out bounds when doing hidden PSD(B) export in Ps 15.1
            if (parseFloat(app.version) < 15.1) {
                if (selectedLayer.typename === "LayerSet") {
                    var dupeLayer = selectedLayer.duplicate();
                    var mergedLayer = dupeLayer.merge();
                    allBounds = mergedLayer.bounds;
                    mergedLayer.remove();
                } else {
                    allBounds = selectedLayer.bounds;
                }

                info.bounds = {
                    'left': allBounds[0].toString(),
                    'top': allBounds[1].toString(),
                    'right': allBounds[2].toString(),
                    'bottom': allBounds[3].toString()
                };
            }

            return JSON.stringify(info);
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getLayerBounds()', ex);
        }
    },
    groupHasNonVectorLayers: function (layerGroup) {
        try {
            var i = 0;
            var layers = layerGroup.layers;
            var hasNonVectorLayers;
            for (i = 0; i < layers.length; i++) {
                if (layers[i].typename === "LayerSet") {
                    hasNonVectorLayers = $._ext_PHXS.groupHasNonVectorLayers(layers[i]);
                    if (hasNonVectorLayers === true) {
                        return true;
                    }
                } else {
                    if (layers[i].kind !== LayerKind.SOLIDFILL && layers[i].kind !== LayerKind.TEXT) {
                        return true;
                    }
                }
            }
            return false;
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-groupHasNonVectorLayers()', ex);
        }
    },
    deleteHiddenLayers: function (layerGroup) {
        try {
            var i = 0;
            var layers = layerGroup.layers;

            for (i = 0; i < layers.length; i++) {
                if (layers[i].visible === false) {
                    layers[i].remove();
                    i--;
                    continue;
                }
                if (layers[i].typename === "LayerSet") {
                    $._ext_PHXS.deleteHiddenLayers(layers[i]);
                }
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-deleteHiddenLayers()', ex);
        }
    },
    generateRepresentationAndPreview: function (repFolderPath, repNameNoExt, previewPath) {
        var repFolder = new File(repFolderPath);
        var previewFile = new File(previewPath);

        var repDesc = makeRepresentationDescriptor(repFolder, repNameNoExt);
        var previewDesc = makeExternalPreviewDescriptor(previewFile, JSXGlobals.previewMaxWidth, JSXGlobals.previewMaxHeight);

        var exportDesc = new ActionDescriptor();
        exportDesc.putClass(PSKey.Using, PSKey.SaveForCCLibrariesElement);
        exportDesc.putObject(PSKey.Representation, PSKey.Representation, repDesc);
        exportDesc.putObject(PSString.ExternalPreview, PSString.ExternalPreview, previewDesc);

        var refList = null;
        refList = makeSelectedSheetsTargetSheetReferenceList();
        exportDesc.putList(PSKey.Target, refList);

        var dimensions = {'width': 0, 'height': 0};
        var resultDesc = executeAction(PSKey.Export, exportDesc, DialogModes.ERROR);

        if (resultDesc && resultDesc.hasKey(PSString.PixelWidth)) {
            dimensions.width = resultDesc.getInteger(PSString.PixelWidth);
        }

        if (resultDesc && resultDesc.hasKey(PSString.PixelHeight)) {
            dimensions.height = resultDesc.getInteger(PSString.PixelHeight);
        }

        var repFile = resultDesc.getPath(PSString.IN);

        var result = {};
        result.dimensions = dimensions;
        result.repPath = repFile.fsName;

        return result;
    },
    saveAsPSD: function (psdPath, pngPath) {
        var destFile = new File(psdPath);
        var pngFile = new File(pngPath);
        var saveDesc = makePSDSaveDescriptor(destFile);

        var previewParams = makeExternalPreviewDescriptor(pngFile, JSXGlobals.previewMaxWidth, JSXGlobals.previewMaxHeight);
        saveDesc.putObject(PSString.ExternalPreview, PSString.ExternalPreview, previewParams);

        var expLyrsDesc = new ActionDescriptor();
        expLyrsDesc.putObject(PSKey.Save, PSKey.Save, saveDesc);
        expLyrsDesc.putClass(PSKey.Using, PSKey.Save);

        var refList = null;
        refList = makeSelectedSheetsTargetSheetReferenceList();
        expLyrsDesc.putList(PSKey.Target, refList);

        var dimensions = {'width': 0, 'height': 0};
        var resultDesc = executeAction(PSKey.Export, expLyrsDesc, DialogModes.NO);

        if (resultDesc && resultDesc.hasKey(PSString.PixelWidth)) {
            dimensions.width = resultDesc.getInteger(PSString.PixelWidth);
        }

        if (resultDesc && resultDesc.hasKey(PSString.PixelHeight)) {
            dimensions.height = resultDesc.getInteger(PSString.PixelHeight);
        }

        var repFile = resultDesc.getPath(PSString.IN);

        var result = {};
        result.dimensions = dimensions;
        result.repPath = repFile.fsName;

        return result;
    },
    saveAsPNG: function (psdPath, pngPath, maxSize) {
        try {
            var newDoc = app.open(new File(psdPath));//app.activeDocument;
            //var newLayer = newDoc.layers[0];

            /*
        // Flatten the layer(s)
        if(newLayer.hasOwnProperty("artLayers")) {
            newLayer = newLayer.merge();
        }

        if (newLayer.kind == LayerKind.SOLIDFILL || newLayer.kind == LayerKind.TEXT) {
            newLayer.rasterize(RasterizeType.ENTIRELAYER);
        }
        // End of flattening
        */
            // Resize:
            if (maxSize) {
                var width = newDoc.width.as('px');
                var height = newDoc.height.as('px');

                if (width > height) {
                    height = Math.round(maxSize * height / width);
                    width = maxSize;
                } else {
                    width = Math.round(maxSize * width / height);
                    height = maxSize;
                }

                newDoc.resizeImage(new UnitValue(width, 'px'), new UnitValue(height, 'px'));
            }
            newDoc.saveAs(new File(pngPath), new PNGSaveOptions(), true);
            newDoc.close(SaveOptions.DONOTSAVECHANGES);
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-saveAsPNG()', ex);
        }
    },
    saveAssets: function (info, generateSecondaryFormat) {
        try {
            // Add an extra 'p' to the end of the preview filename incase the
            // representation file turns out to also be a PNG.
            var pngPath = Folder.temp.fsName + "/" + info.name + "p" + ".png";
            var result = $._ext_PHXS.generateRepresentationAndPreview(Folder.temp.fsName, info.name, pngPath);

            var filePath = result.repPath;

            var strLayerName = $._ext_PHXS.getLayerName();
            var fileData = {
                'layerName': strLayerName,
                files: [{'path': filePath, 'relationship': 'primary'}],
                rendition: pngPath,
                'dimensions': result.dimensions
            };

            var layerIds = getSelectedLayerIDs();
            if (layerIds) {
                fileData.layerIds = layerIds;
            }

            if (app.activeDocument) {
                fileData.documentId = app.activeDocument.id;
            }

            return JSON.stringify(fileData);
            /*if (generateSecondaryFormat) {
                // ** Save as SVG file **
                //var hasNonVectorLayers = $._ext_PHXS.groupHasNonVectorLayers(newDoc);

                try {
                    if (cssToClip) {
                        cssToClip.documentIndexOffset = 0; // Workaround to generate SVG for correct layer
                    }
                } catch (ignore) {}

                params.layerID = newLayer.id;
                params.layerScale = 1;

                // We use Photoshop Generator to save SVG files
                // <Photoshop Path>/Required/Generator-builtin/lib/jsx/getLayerSVG.jsx is loaded when panel opens
                // svg & svg.createSVGDesc are defined in above file
                try {
                    var svgDesc = svg.createSVGDesc();
                    var svgData = svgDesc.getString(sTID("svgText"));
                    svgData = decodeURI(svgData);

                    filePath = Folder.temp.fsName + "/" + info.name + ".svg";
                    var svgFile = new File(filePath);
                    svgFile.encoding = "UTF-8"; // Must match the SVG header
                    svgFile.open('w');
                    svgFile.write(svgData);
                    svgFile.close();

                    filePaths.push(filePath);
                    // ** End of - Save as SVG file **

                    // ** Save as PNG file **
                    //filePath=Folder.temp.fsName + "/" + info.name + ".png";
                    //$._ext_PHXS.saveAsPNG(filePath);
                } catch (ignore1) {
                    // TODO: the svg extraction sometimes fails. Is this supposed to happen?
                }
            }*/
        } catch (ex1) {
            $._ext_CORE.writeToLog('PHXS.jsx-saveAssets()', ex1);
        }
    },
    setColor: COLOR.setColor,
    setFillColor: COLOR.setFillColor,
    setStrokeColor: COLOR.setStrokeColor,
    setColorOverlay: COLOR.setColorOverlay,
    getCurrentState: function () {
        try {
            if (app.documents && app.documents.length > 0) {
                var selectedLayerId = -1;
                var selectedLayer = app.activeDocument.activeLayer;
                var docPath = app.activeDocument.name;

                if (selectedLayer) {
                    selectedLayerId = selectedLayer.id;
                }
                try {
                    if (app.activeDocument.fullName) {
                        docPath = app.activeDocument.fullName.fsName;
                    }
                } catch (ex2) {
                    JSON.stringify({
                        'path': docPath,
                        'layerID': selectedLayerId
                    });
                }

                return JSON.stringify({
                    'path': docPath,
                    'layerID': selectedLayerId
                });
            }
            return JSON.stringify({
                'name': '',
                'layerID': -1
            });
        } catch (ex) {}
    },
    getLayerName: function () {
        try {
            return app.activeDocument.activeLayer.name;
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getLayerName()', ex);
            return '';
        }
    },
    getLayerInfo: function () {
        try {
            var layerObject = {'name': '', 'fullName': ''};
            var layerColors = [];

            var areEqual = function (colorData1, colorData2) {
                var key;
                if (colorData1 && colorData2) {
                    for (key in colorData1[0].value) {
                        if (colorData1[0].value.hasOwnProperty(key)) {
                            if (!colorData2[0].value.hasOwnProperty(key) || Math.round(colorData1[0].value[key]) !== Math.round(colorData2[0].value[key])) {
                                return false;
                            }
                        }
                    }
                }
                return true;
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

            layerObject.enableApplyText = false;

            var selectedLayer = false; // For multi-layer selections, this is the top layer
            var numSelectedLayers = getNumSelectedLayers();
            if (numSelectedLayers === 1) {
                selectedLayer = app.activeDocument.activeLayer;
                if (selectedLayer && selectedLayer.kind === LayerKind.TEXT && selectedLayer.textItem.contents.length > 0) {
                    layerObject.enableApplyText = true;
                    layerObject.text = $._ext_CORE.shortenString(selectedLayer.textItem.contents);

                    layerObject.fontInfo = TEXT.getLayerFont();
                    pushUnique(layerObject.fontInfo.color, JSXGlobals.PS_TEXT);
                }

                pushUnique(COLOR.getSolidFillColor(), JSXGlobals.FILL);
                pushUnique(COLOR.getSolidStrokeColor(), JSXGlobals.STROKE);
                pushUnique(COLOR.getLayerEffectColor(PSKey.SolidFill), JSXGlobals.PS_EFFECT_FILL);
                pushUnique(COLOR.getLayerEffectColor(PSKey.FrameFX), JSXGlobals.PS_EFFECT_STROKE);

                layerObject.name = $._ext_CORE.shortenString(selectedLayer.name, false, JSXGlobals.maxNameLength);
                layerObject.fullName = selectedLayer.name;
                layerObject.hasLayerStyles = LAYERSTYLE.hasLayerStyles();
            } else if (numSelectedLayers > 1) {
                selectedLayer = app.activeDocument.activeLayer;
                if (selectedLayer && selectedLayer.kind === LayerKind.TEXT) {
                    layerObject.enableApplyText = true;
                }
            }
            pushUnique(COLOR.solidColorToData(app.foregroundColor), JSXGlobals.PS_FOREGROUND);

            layerObject.colors = layerColors;
            layerObject.kind = "";
            layerObject.selectionExists = (numSelectedLayers > 0);
            layerObject.enableApplyStyle = (numSelectedLayers > 1 || (numSelectedLayers === 1 && !selectedLayer.isBackgroundLayer));
            layerObject.enableShapeLayerApplyOperations = false;
            layerObject.libraryLinked = false;

            if (numSelectedLayers === 1 && selectedLayer.kind) {
                layerObject.kind = selectedLayer.kind.toString();

                if (selectedLayer.kind === LayerKind.SMARTOBJECT) {
                    var soDesc = getLayerProperty(PSKey.SmartObject);
                    if (soDesc && soDesc.hasKey(PSKey.LINK) && soDesc.getType(PSKey.LINK) === DescValueType.OBJECTTYPE && soDesc.getClass(PSKey.LINK) === PSKey.LIB_ELEMENT) {
                        layerObject.libraryLinked = true;
                    }
                }
            }

            if (numSelectedLayers > 0 && selectedLayer.kind) {
                // Only show apply fill/stroke color for Shape layers, not Solid Color Fill layers
                if (selectedLayer.kind === LayerKind.SOLIDFILL) {
                    var hasVectorMask = getLayerProperty(PSKey.HasVectorMask);
                    layerObject.enableShapeLayerApplyOperations = hasVectorMask;
                }
            }

            return JSON.stringify(layerObject);
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getLayerInfo()', ex);
        }
    },
    getLayerColor: COLOR.getLayerColor,
    getLayerFontStyle: function (docName) {
        try {
            var docRef = $._ext_CORE.getDocument(docName);
            if (docRef) {
                docRef.activeLayer = docRef.layers[0];
                var selectedLayer = docRef.activeLayer;
                var layerObject = {};

                layerObject.text = $._ext_CORE.shortenString(selectedLayer.textItem.contents);

                var layerFonts = TEXT.getLayerFont();
                layerObject.fontInfo = layerFonts.font;
                layerObject.textColor = layerFonts.font.color;

                return JSON.stringify(layerObject);
            }
            return undefined;
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-TEXT.getLayerFontStyle()', ex);
        }
    },
    getIMSUserID: function () {
        var userId = "";
        try {
            var bhnc = cTID("bhnc");
            var ref = new ActionReference();
            ref.putProperty(cTID("Prpr"), bhnc);
            ref.putEnumerated(cTID("capp"), cTID("Ordn"), cTID("Trgt"));
            var appDesc = app.executeActionGet(ref);
            if (appDesc.hasKey(bhnc)) {
                userId = appDesc.getString(bhnc);
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getIMSUserID()', ex);
        }

        return userId;
    },
    getTooltipState: function () {
        var tooltipsEnabled = true;
        try {
            tooltipsEnabled = getAppProperty(PSKey.ShowToolTips);
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getTooltipState', ex);
        }
        return tooltipsEnabled;
    },
    getReadableFileExtensions: function () {
        var readableExtensions = [];
        try {
            var ref = new ActionReference();
            ref.putProperty(PSClass.Property, PSKey.FileFormats);
            ref.putEnumerated(PSClass.Application, PSType.Ordinal, PSEnum.Target);

            var appDesc = app.executeActionGet(ref);

            if (appDesc.hasKey(PSKey.FileFormats)) {
                var fileFormatsDesc = appDesc.getObjectValue(PSKey.FileFormats);
                if (fileFormatsDesc.hasKey(PSKey.ReadableFileExtensions)) {
                    var listDesc = fileFormatsDesc.getList(PSKey.ReadableFileExtensions);
                    var index = 0;
                    for (index = 0; index < listDesc.count; ++index) {
                        readableExtensions.push(listDesc.getString(index));
                    }
                }
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getReadableFileExtensions', ex);
        }
        return JSON.stringify(readableExtensions);
    },
    isAnalyticsEnabled: function () {
        var userTrackingEnabled = false;
        try {
            var welcome          = cTID('wlcm');
            var koptinStr        = sTID("optin");
            var welcomeDesc = getAppProperty(welcome);
            if (welcomeDesc && welcomeDesc.hasKey(koptinStr)) {
                userTrackingEnabled = welcomeDesc.getBoolean(koptinStr);
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-isAnalyticsEnabled', ex);
        }
        return userTrackingEnabled;
    },
    getConfigInfoFromPS: function () {
        var configInfo = { generalLinkingEnabled: false };
        try {
            var ccLibConfigDesc = getAppProperty(PSKey.CCLibrariesConfig);
            if (ccLibConfigDesc && ccLibConfigDesc.hasKey(PSKey.EnableGeneralLinking)) {
                configInfo.generalLinkingEnabled = ccLibConfigDesc.getBoolean(PSKey.EnableGeneralLinking);
            }
        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-getConfigInfoFromPS', ex);
        }
        return JSON.stringify(configInfo);
    },
    reportEvent: function (eventName, properties, force) {
        try {
            if (eventName === "createElement" || eventName === "useElement" || eventName === "createLink" || force) {
                // Log events to Highbeam so Design Library usage can be compared
                // to usage of other Photoshop features.

                // This is the data group name; it should be identical across all calls for a given
                // data group and self descriptive.
                // By not including eventName in the group name, we are recording a single data
                // group for Design Library (eventName gets added as a property).
                var highbeamDataGroupName = "Design Library";

                 // Helper to handle null and undefined properties which can't be added to
                // an ActionDescriptor. When properties aren't set, log "N/A" rather than
                // letting Highbeam fill in a default value.
                var safeAddStringPropertyToDesc = function (descriptor, key, property) {
                    if (property) {
                        descriptor.putString(key, property);
                    } else {
                        descriptor.putString(key, "N/A");
                    }
                };

                var desc = new ActionDescriptor();

                // Required params:
                //   - eventRecord: Data group name
                safeAddStringPropertyToDesc(desc, PSEvent.Record, highbeamDataGroupName);

                // There should be a well defined and limited set of values for each property
                // which is what you want to make Highbeam data most usable. Library/element
                // IDs are an exception as we log them to look at aggregate per-library and
                // per-element data.
                safeAddStringPropertyToDesc(desc, PSKey.HighbeamEventName, eventName);
                safeAddStringPropertyToDesc(desc, PSKey.HighbeamLibraryID, properties.libraryID);
                safeAddStringPropertyToDesc(desc, PSKey.HighbeamLibraryElementCount, properties.libraryElementCount);
                safeAddStringPropertyToDesc(desc, PSKey.HighbeamElementID, properties.elementID);
                safeAddStringPropertyToDesc(desc, PSKey.HighbeamElementType, properties.elementType);

                // Add properties that we only want for specific event types
                if (eventName === "useElement") {
                    safeAddStringPropertyToDesc(desc, PSKey.HighbeamRepresentationType, properties.representationType);
                    safeAddStringPropertyToDesc(desc, PSKey.HighbeamOpType, properties.opType);
                    safeAddStringPropertyToDesc(desc, PSKey.HighbeamDetails, properties.details);
                }

                if (eventName === "createLink") {
                    safeAddStringPropertyToDesc(desc, PSKey.HighbeamOpType, properties.opType);
                }

                app.executeAction(PSEvent.HeadlightsInfo, desc, DialogModes.NO);

                return true;
            }

        } catch (ex) {
            $._ext_CORE.writeToLog('PHXS.jsx-reportEvent()', ex);
        }

        return false;
    },
    getStringID: function (strValue) {
        if (typeof strValue === 'string') {
            return sTID(strValue);
        }

        return strValue.map(sTID);
    }
};

