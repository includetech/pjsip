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
/*globals $, app, ToolTipOptions, document*/

$._ext_PPRO = {
    setThumbnailExportOptions: function () {
        // This appears to be Illustrator specific PPRO does not have this function
    },
    getTooltipState: function () {
        return 'true';
    },
    getIMSUserID: function () {
        return app.userGuid;
    },
    getSelectedLookData: function () {
        return app.getSelectedLookData;
    },
    isAnalyticsEnabled: function () {
        return app.isUserSharingAppUsageData();
    },
    getCurrentState: function () {
        return document.getDocumentState();
    },
    placeAsset: function (pathStr, is_psdB, elemName) {
        app.project.placeAsset(pathStr, is_psdB, elemName);
        return;
    },
    isFontAvailable: function () {
        return 'true';
    },
    replaceColor: function (colorData) {
        // Currently PPRO does not support replaceColor
        return false;
    },
    setColor: function (colorRec) {
        // to be implemented later
        return false;
    },
    getLayerColor: function () {
        // Currently PPRO does not support getLayerColor
        var colorObject;
        return JSON.stringify(colorObject);
    },
    getLayerInfo: function () {
        return app.getLookInfo;
    }
};
