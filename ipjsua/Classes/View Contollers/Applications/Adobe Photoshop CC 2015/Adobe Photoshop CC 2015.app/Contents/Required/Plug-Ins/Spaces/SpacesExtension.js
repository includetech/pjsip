/*
 * Copyright (c) 2014 Adobe Systems Incorporated. All rights reserved.
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * 
 */

/** This file describes the native extensions that allows a Spaces application
to interact with Photoshop.

General objects
This following section describes common objects and patterns used by the
interface.

"err"
The err argument is returned as the first argument in a completion callback
provided to a command.
If a command succeeds then the value of the err object is undefined, or
it is an object with a "number" key whose value is 0.
When the command fails, then an object is returned. The returned object has the
following values:
"number" (number, required)
    An error code describing the failure. @see _spaces.errorCodes
"message" (string, optional)
    An english string describing failure.
"stack" (string, optional)
    A stack describing the failure location.
"fileName" (string, optional)
    Name of the file where the error occurred
"lineNumber" (number, optional)
    Line number for the location of the error
"columnNumber" (number, optional)
    Column number for the location of the error


"paintOptions"
This object describes how the Photoshop canvas is updated with respect to
OS drawing.
When this object is not present, then Photoshop will use its default
update logic.
The following keys are recognized:
"immediateUpdate" (boolean)
    If true then the Photoshop canvas is updated immediately. This typically
    means that the canvas is updated after executing one or more commands
    right before executing the completion callback.
"quality" (string)
    Describes the update quality. Possible values are: "draft", "medium", "final".
    When draft is specified then the update may not be fully accurate. "draft" is
    used when a number of subsequent commands are executed. When "final" is specified
    then the canvas is updated fully to its final form. "medium" is a quality that is
    between draft and final.
    When "draft" or "medium" is provided then Photoshop will automatically update
    to "final" at a later time.
"documentId" (int)
    The ID of the document to update. If this key is missing, or if it is less than 0,
    then the active document will be used as the target.
*/


var _spaces;

if (!_spaces)
  _spaces = {};

// ==========================================================================
// Definitions for the _spaces scope

/** _spaces.version
Returns the version for the Spaces runtime layer.
The version has three component: major, minor, patch
When the runtime breaks backwards compatibility the major version is updated.
When the runtime adds new functionality without breaking existing functionality,
the minor version is updated.
When the runtime is updated without changing the API, then the patch version
is updated
*/
Object.defineProperty(_spaces, "version", {
    writeable: false,
    enumerable: true,
    configurable: false,
    get: function () {
        native function pgGetVersion();
        return pgGetVersion();
    }
});

/* Error codes used by Spaces.
Typically such information is returned in the err object to callback methods.
@see description in "Common objects"
*/
_spaces.errorCodes = {
    /** Designates success.
    Typically undefined is returned as the err object on success, but if an
    err object with number equal to NO_ERROR is returned, then this state also
    designates success.
    */
    NO_ERROR:0,
    
    /** Some error has occurred.
    */
    UNKNOWN_ERROR:1,

    /** A request could not be dispatched to the host process.
    */
    CANT_DISPATCH_MESSAGE_TO_HOST:101,

    /** General argument error.
    The message of the returned error object may contain additional information
    about the failure type.
    */
    ARGUMENT_ERROR:1000,

    /** The javascript request is missing a required callback argument.
    The callback must be the last argument in the javascript call
    */
    MISSING_NOTIFIER:1001,

    /** The host rejected the request. This error can for example be returned
    if the host is in a modal state that prevents it from processing the request.
    */
    REQUEST_REJECTED:1002,

    /** Error related to converting values between V8, CEF, and PS domains.
    The message of the returned error object may contain additional information
    about the failure type.
    */
    CONVERSION_ERROR:1050,

    /** Attempt to execute a native function that was not recognized by the host */
    UNKNOWN_FUNCTION_ERROR:1051,

    /** An error occurred in the "suite pea" layer of Photoshop.
    The message of the returned error object may contain additional information
    about the failure type.
    */
    SUITEPEA_ERROR:1100,

    /** An attempt to dispatch a new message while another message was already
    being dispatched.
    This error indicates a command processing issue in the Photoshop adapter layer.
    */
    REENTRANCY_ERROR:1500,
};

/** Notifier groups allow javascript to listen to various events.
Different event scopes are mapped to different notifier groups.
The Spaces runtime uses a single slot model where at most a single
javascript notifier can be installed at any time.
Registering for notifications in a notifier group will cause the runtime
to set up necessary runtime hooks,
For efficiency reasons javascript should only register for the notifier
groups that are needed for the current javascript execution mode as opposed
to registering for all groups and then perform mode filtering in the
javascript layer.
Notifier groups are used with _spaces.setNotifier.
*/
_spaces.notifierGroup = {

    /** Used for notifications from Photoshop.
    The callback will have the following signature
    callback(err, notificationKind, info).
    - notificationKind is a string describing the Photoshop event.
    - info is an object containing additional information about the event.
     */
    PHOTOSHOP:               "notifierGroupPhotoshop",


    /** Used for notifications from the operating system.
    The callback will have the following signature
    callback(err, notificationKind, info)
    - notificationKind is a string describing the OS event.
    - info is an object containing additional information about the event.
    */
    OS:                     "notifierGroupOS",

    /** Used for menu selection notifications for javascript driven menus.
    @see installMenu

    The callback will have the following signature
    callback(err, menuCommand, info)
    - menuCommand corresponds to the menu command string that was used when creating the menu.
    - info is currently unused
     */
    MENU:                   "notifierGroupMenu",

    /** Used for notifications related to interaction state changes in
    Photoshop.
    An example of interaction state changes is: show/hide of progress dialogs.
    This group allows for *redirection* of certain interactions and this means that
    this group may change the behavior of Photoshop.
    When calling setNotifier for registering for INTERACTION_STATE, the options
    argument must specify the interaction state changes that are desired. This
    is done by including the key "notificationKind" key whose value one or more
    of the bit-field values described in _spaces.notifierOptions.interaction.
     
    The callback will have the signature:
    callback(err, type, info)
    @see _spaces.notifierGroupOptions for a explanation of type and info for
    each of the various notification types.
    */
    INTERACTION:            "notifierGroupInteraction",
    
    /** This group is used to sign up for (experimental) touch events.
    If a browser requests touch events (on Windows), then these events are not
    propagated to the default handler and this means that Windows will not synthesize
    mouse events.
    */
    TOUCH:                  "notifierGroupTouch"
};

if (!_spaces.notifierOptions)
    _spaces.notifierOptions = {}

/** Options for _spaces.notifierGroup.INTERACTION
*/
_spaces.notifierOptions.interaction = {
    /** Used to request progress notifications from Photoshop.
    This option will suppress default progress UI,
    A progress notification has a type argument whose value is "progress".
    The info argument is an object with three keys:
        "title" whose value is the title of the progress dialog that would have been displayed
        "phase" whose value is one of "start", "update", and "finish"
        "completion" whose value is the percentage of the task that is completed, in the range 0-100
    */
    PROGRESS: 1,

    /** Used to request notifications for error messages from Photoshop.
    This option will suppress default error UI from being displayed.
    An error notification has a type argument whose value is "error".
    The info argument is an object with the key "text" whose value is the text the dialog would have displayed.
    */
    ERROR: 2,

    /** Used to request notifications for "options" dialogs from Photoshop
    Options dialogs are presented modally by certain tools when more information
    may be provided.  An example is the "Create Rectangle" options dialog that is
    presented when a user single-clicks on the document view when the Rectangle tool
    is active.  
    This option will prevent certain options dialogs from being displayed.
    An options notification has a type argument whose value is "options".
    The info argument is an object with the key "message" whose value is the title or message the dialog would have displayed.
    */
    OPTIONS: 4,

    /** Used to request notifications for context menus in Photoshop.
    An options notification has a type argument whose value is "options".
    The info argument is an object with the key "location".  The value of the "location" argument is an object with two
    keys, "x", and "y" representing the screen location within the document view window where the context menu was invoked.
    */
    CONTEXT: 8,

    /** Used to request notifications for user interactions such as the user resizing the application frame.
    When the application frame is resized, then two notifications are received:
    - "interactiveResizeBegin" when the resize interaction begins
    - "interactiveResizeEnd" when the resize interaction ends.
    */
    USER: 16,
};

/** Set a notifier callback for the provided notifier group.
@see _spaces.notifierGroup.

@param notifierGroup (string)   The target notifier group.
@param options (object)         Options specific to the notifier group. See the notifier group description.
@param  callback (function)
    The function to invoke when a notifier is received.
    Use undefined to unsubscribe from the specified notifierGroup.
    If the requested notification can be created, then the
    callback is invoked with a single err argument whose value
    follows the standard err rules. see the general objects
    section.
*/
_spaces.setNotifier = function (notifierGroup, options, callback) {
    native function pgSetNotifier();
    return pgSetNotifier(notifierGroup, options, callback);
};

/** Properties.
@see getPropertyValue and setPropertyValue
Properties:

Tooltip properties:
"ui.tooltip.delay.coldToHot"
    Value is a number describing the delay to use in in seconds. The value must
    be > 0.
    The initial delay used before showing a tooltip.

"ui.tooltip.delay.hotToHot"
    Value is a number describing the delay to use in in seconds. The value must
    be > 0.
    The delay used when changing an existing tooltip.

"ui.tooltip.delay.hotToCold"
    Value is a number describing the delay to use in in seconds. The value must
    be > 0.
    Delay used when dismissing a tooltip.

"ui.tooltip.delay.autoHide"
    Value is a number describing the delay to use in in seconds. If the value is
    <= 0, then auto-hiding is disabled.
    Delay used to determine when a visible tooltip should be hidden.
*/

/** Get the value of a property.
@param propertyName (string)    Specifies the property whose value to return.
@param options      (object)    Specifies options for the operation. Currently
                                unused.
@param callback     (function)  A callback notifier with the signature described below.

callback(err, propertyValue)
"propertyValue" (<multiple>)    The value of the property. The type of the value depends
                                on the property.
*/
_spaces.getPropertyValue = function (propertyName, options, callback) {
    native function pgGetPropertyValue();
    return pgGetPropertyValue(propertyName, options, callback);
};

/** Set the value of a property.
@param propertyName  (string)     Specifies the property whose value to set.
@param propertyValue (<multiple>) Specifies the new value of the property. The
                                  type of the value depends on the property.
@param options       (object)     Specifies options for the operation. Currently
                                  unused.
@param callback     (function)    A callback notifier with an err argument.
*/
_spaces.setPropertyValue = function (propertyName, propertyValue, options, callback) {
    native function pgSetPropertyValue();
    return pgSetPropertyValue(propertyName, propertyValue, options, callback);
};


/** Abort the current Spaces interaction.
This API is intended to be used when the HTML session detects an unrecoverable
error. In that case calling "abort" will return the user to the default experience.
By default the Cef session will be destroyed when "abort" is called. See lifetime
discussion below.
An optional dialog can be shown in the host environment after the HTML surface
has been hidden.

Lifetime:
By default the Cef session will be destroyed when "abort" is called. This can be a
problem while developing the HTML application, as you then loose the ability to
introspect the Cef state after "abort" has been called.
To keep the Cef session alive after the call to "abort", you can set the following key to
true in the "Settings.json" configuration file: "dont_destroy_cef_in_abort".
If "dont_destroy_cef_in_abort" is specified, then the Cef session is kept alive after
"abort" is called and you can introspect the Cef state with the Chromium debugger.
After the state has been analyzed, you will need to call "abort" again with forceDestroy
set to true in order to put Spaces into a state that allows for a relaunch of the HTML
experience without a restart of Photoshop.

@param  options      (object)   Specifies options for the operation.
@param  callback     (function) A callback notifier that may be executed in certain
                                cases (see below). If the command is successful then
                                the callback is *not* executed.

"options"
The following keys are recognized:
"message" (string,optional)
    If provided then an alert is shown in the host environment after the HTML surface
    has been hidden. The text of the alert is the contents of message.
"forceDestroy" (boolean,optional)
    Default value is "false". If true, then the Cef session is destroyed regardless of
    the contents of Sessings.json. This parameter is meant to be used only from an
    interactive Chromium debugger console (after the aborted session has been analyzed).

callback(err)
The callback notifier is typically not executed because abort terminates
the HTML session.
If an error occurs while processing abort, or if the host is unable to abort
the current session, then an error is returned.
*/
_spaces.abort = function (options, callback) {
    native function pgAbort();
    return pgAbort(options, callback);
};

/** open a URL in the default browser 
@param url          (string)    A URL.
@param callback     (function)  Callback error notifier with an "err" argument.
*/
_spaces.openURLInDefaultBrowser = function (url, callback) {
    native function pgOpenURLInDefaultBrowser();
    return pgOpenURLInDefaultBrowser(url, callback);
};


// ==========================================================================
// _spaces.ps  -  Functionality related to Photoshop

if (!_spaces.ps)
   _spaces.ps = {};

/** Return the tool that is currently selected
@param  callback     (function) A callback notifier with the signature described below.


callback(err, info)
"info" (object)
    Object describing the current state. The following keys
    are provided in the info object:
    "title"     (string)    Title of the current tool
    "isModal"   (boolean)   True if the tool is modal
    "key"       (string)    OSType for the tool
*/
_spaces.ps.getActiveTool = function (callback) {
    native function psGetActiveTool();
    return psGetActiveTool(callback);
};

/** end the current modal tool editing state. If Photoshop is not currently
in a modal tool editing state, then this method does nothing.
@param doCommit     (boolean)   If true then the current edit will be committed.
                                If false, then the current edit will be canceled.
@param  callback    (function)  A callback notifier with an err argument.
*/
_spaces.ps.endModalToolState = function (doCommit, callback) {
    native function psEndModalToolState();
    return psEndModalToolState(doCommit, callback);
};

/** Perform a Photoshop menu command
@param options (object)
    Options describing the request. See below for details.
@param  callback (function)
    A callback notifier with the signature described below.

options
"commandId" (number)
    The identifier of the menu command to perform.
"waitForCompletion" (boolean, optional)
    If true, then Photoshop waits for the command to complete before invoking
    the callback notifier. The default value is false.

callback(err, available)
@param available (boolean)
    If the menu command was available then this argument is true and the
    command was performed. If the command was not available then this
    argument will be false and the command was not performed.
*/
_spaces.ps.performMenuCommand = function (options, callback) {
    native function psPerformMenuCommand();
    return psPerformMenuCommand(options, callback);
    };

/** Process all commands that are in the internal Photoshop command queue.
This API can be used to ensure that certain operations have completed.
In some cases performing a Photoshop command (via batchPlay or performMenuCommand)
may result in Photoshop queuing a command for execution a a later time.
Also, some commands post additional commands when they are executed.
Calling this API will cause Photoshop to process all pending commands in the
event queue.
@param  options      (object)   Specifies options for the operation.
@param callback (function)  A callback notifier with an err argument.


"options"
The following keys are recognized:
"invalidateMenus" (boolean, optional)
    If true then Photoshop's menus are invalidated after the operation. This can be
    useful when the pending commands may affect which menu commands that are enabled.
    If a call to performMenuCommand returns "false" for "available" then the issue
    could be that the related command is disabled due to a stale menu state. In this case
    calling this api to invalidate menus may resolve the issue.
    @See performMenuCommand.
*/
_spaces.ps.processQueuedCommands = function (options, callback) {
    native function psProcessQueuedCommands();
    return psProcessQueuedCommands(options, callback);
};

/** Request an image from Photoshop. EXPERIMENTAL API.
@param request (object)
    An object specifying the requested image. The following keys are recognized:
    "documentId" (number)
        The id of the target document related to the image.
        This key is required. If the key is less than 0, then the current document is used.
    "encoding" (string)
        The requested encoding. Currently only "base64" is supported.
@param  callback     (function) a callback notifier with the signature described below.


callback(err, info)
"info"  (string)    a base64 encoded representation of the image (jpeg)
*/
_spaces.ps.requestImage = function (request, callback) {
    native function psRequestImage();
    return psRequestImage(request, callback);
    };

/** Logs an event to the Photoshop Headlights database.
Note: do not dynamically generate event names, rather use a small number of unique names.

@param options (object)
    An object with the following keys:
    "category" (string)     The Headlights category,
    "subcategory" (string)  The Headlights subcategory
    "event" (string)        The Headlights event
@param callback (function)  A callback notifier with an err argument.
*/
_spaces.ps.logHeadlightsEvent = function (options, callback) {
    native function psLogHeadlightsEvent();
    return psLogHeadlightsEvent(options, callback);
};


// ==========================================================================
// _spaces.ps.descriptor  -  Functionality related to executing
//                               Action Descriptors

if (!_spaces.ps.descriptor)
    _spaces.ps.descriptor = {};

/** Options used to control how action descriptors are executed
*/
_spaces.ps.descriptor.interactionMode = {
    /*
    Display dialog only if necessary due to missing parameters or error.
    */
    DONT_DISPLAY: 1,

    /*
    Present the plug-in dialog using descriptor information.
    */
    DISPLAY: 2,

    /*
    Never present a dialog; use only descriptor information;
    if the information is insufficient to run the command,
    an error is returned in the callback method.
    */
    SILENT: 3 
};

/** Common descriptor options
"canExecuteWhileModal" (optional, boolean).
The value determines whether or not the request can be accepted when the host is in a modal state.
The default value is false, which means that a request is rejected if the host is in a
modal state.

"useExtendedReference" (optional, boolean)
Used when an extended action reference is provided to a get, play, or batchPlay command.
*/

/** play a single command in Photoshop (an action descriptor).
@param name (string)
    The Photoshop command to play.
@param descriptor (object)
    Arguments for the Photoshop command
@param options (object)
    Options related to how the command is executed. The following keys are
    accepted:
    "interactionMode" (optional)
        A value from _spaces.ps.descriptor.interactionMode.<some value>
        The default value is SILENT
    "paintOptions" (object, optional)
        Controls how the Photoshop canvas is updated after all commands have
        been executed. @see the general objects section.
    "canExecuteWhileModal" (optional, boolean)
        @see the general objects section.
    "useExtendedReference" (optional, boolean)
        @see common descriptor options
@param callback (function) A callback notifier with the signature described below.


callback(err, descriptor)
"descriptor"    The result descriptor from the play command.
*/
_spaces.ps.descriptor.play = function (name, descriptor, options, callback) {
    native function psDescPlay();
    return psDescPlay(name, descriptor, options, callback);
};

/** Play a list of commands
@param commands (list)
    A list of commands to play. A command is an object conforming to the following
    specification:
    "name" (string)         The Photoshop command to play.
    "descriptor" (object)   Arguments for the Photoshop command
    "options" (object)      Options for the command. The following keys are accepted:
                            "useExtendedReference" (optional, boolean). @see common descriptor options
@param options (object)
    Options controlling the batch process. The following keys are accepted:
    "interactionMode" (optional)
        A value from _spaces.ps.descriptor.interactionMode.<some value>
        The default value is SILENT
    "continueOnError" (boolean, optional)
        If true, then all commands are executed regardless of the return value of
        the individual commands. If false, then the command terminates if any of
        the individual commands return an error. The default value is false.
    "historyStateInfo" (object, optional).
        If present, then all commands are combined into a single history state.
        The requirement for having a single history state is that all commands target
        the same document. The following keys are accepted:
           "name"       (string)            the name to use for the history state
           "target"     (action reference)  action reference specifying the target document
           "coalesce"   (boolean, optional)
                Replace existing saved undo history state if it has the same name.
                Default value is false.
    "paintOptions" (object, optional).
        Controls how the Photoshop canvas is updated after all commands have been executed.
        @see the general objects section.
    "canExecuteWhileModal" (optional, boolean).
        @see the general objects section.
@param callback (function) A callback notifier with the signature described below.

callback(err, descriptors, errors)
@param descriptors Array of result descriptor from the play commands.
@param errors      Array of error values corresponding to the
                   results en the descriptors array. If no error
                   was returned from the Nth command, then the
                   corresponding error value is undefined.
*/
_spaces.ps.descriptor.batchPlay = function (commands, options, callback) {
    native function psDescBatchPlay();
    return psDescBatchPlay(commands, options, callback);
};

/** execute a "get" action descriptor request.
@param reference (object)    A javascript object describing the action reference target for
                            the request.
@param options (object)     options for the getter. See common descriptor options
                                                    @see common descriptor options
@param  callback     (function) A callback notifier with the signature described below.


callback(err, descriptor)
@param descriptor      if the method succeeds, then this argument
                      contains the result descriptor as returned by
                      Photoshop's get implementation
*/
_spaces.ps.descriptor.get = function (reference, options, callback) {
    native function psDescGet();
    
    // legacy HTML calls this method with the following signature:
    // (reference, callback)
    // i.e. in legacy invocations has only two arguments (and the 3rd argument is undefined)
    // 2015-04-07 Design Space use the legacy form in some cases.
    if (callback === undefined) {
        // legacy mode detected
        callback = options;
        options = {};
    }
    
    return psDescGet(reference, options, callback);
};


// ==========================================================================
// _spaces.ps.ui

if (!_spaces.ps.ui)
    _spaces.ps.ui = {};

/** Returns the scale factor that photoshop is using for its user interface elements.
*/
Object.defineProperty(_spaces.ps.ui, "scaleFactor", {
    writeable: false,
    enumerable: true,
    configurable: false,
    get: function () {
        native function psUiGetScaleFactorProp();
        return psUiGetScaleFactorProp();
    }
});

/// "bit field" enumeration describing the various UI types
_spaces.ps.ui.widgetTypes = {
    TOOLBAR: 1,
    CONTROLBAR: 2,
    PALETTE: 4,
    DOCUMENT: 8,
    APPLICATIONBAR: 16,
    DOCUMENT_TABS: 32,
    ALL: 63   // be sure to update when adding any items
};

/** Show/Hide UI types.
@param widgetTypes (number)     The UI types whose visibility should be affected.
                                @see _spaces.ui.widgetTypes
@param visibility (boolean)     Whether or not the provided widget types should be shown
                                or hidden
@param callback (function)      A callback notifier with an err argument.
*/
_spaces.ps.ui.setWidgetTypeVisibility = function (widgetTypes, visibility, callback) {
    native function psUiSetWidgetTypeVisibility();
    return psUiSetWidgetTypeVisibility(widgetTypes, visibility, callback);
};

/// enumeration controlling pointer propagation
_spaces.ps.ui.pointerPropagationMode = {
    /// Propagate the pointer event based on the alpha value under the event point
    ALPHA_PROPAGATE: 0,

    /// Always propagate pointer events. Spaces will never get a pointer event
    ALWAYS_PROPAGATE: 1,

    /// Never propagate pointer events. Spaces consumes all pointer events
    NEVER_PROPAGATE: 2,
};

/** Change the pointer propagation mode.
This mode is used if there is no pointer policies that match the current mouse
event.
@param mode (object)
    The propagation mode that Spaces should use in its OS views.
    The following keys are recognized:
    "defaultMode" (number, optional)
        A value from _spaces.ps.ui.pointerPropagationMode
        The default value is ALPHA_PROPAGATE
@param callback (function)      A callback notifier with an err argument.
*/
_spaces.ps.ui.setPointerPropagationMode = function (mode, callback) {
    native function psUiSetPointerPropagationMode();
    return psUiSetPointerPropagationMode(mode, callback);
};

/** Get the pointer propagation mode
@param callback (function)
    Callback notifier with the following signature: callback(err, mode)
    "mode" (number)     A value from _spaces.ps.ui.pointerPropagationMode
*/
_spaces.ps.ui.getPointerPropagationMode = function (callback) {
    native function psUiGetPointerPropagationMode();
        return psUiGetPointerPropagationMode(callback);
    };

/** Modifier bit fields. The command modifier is only used on Mac OS
*/
_spaces.ps.ui.policyAction = {
    /// Propagate the pointer event based on the alpha value under the event point
    ALPHA_PROPAGATE: 0,

    /// Always propagate pointer events. Spaces will never get a pointer event
    ALWAYS_PROPAGATE: 1,

    /// Never propagate pointer events. Spaces consumes all pointer events
    NEVER_PROPAGATE: 2,
};

/** Change the pointer propagation policy
@param options (list)
    The propagation policy that Spaces should use for OS pointer events.
    The list may have 0 or more policies.
    A policy is an object whose form depends on the type of event. At the moment
    only pointer events are supported.
    A pointer event policy is an object with the following keys:
    "eventKind" (number, required)
        Can be _spaces.os.eventKind.LEFT_MOUSE_DOWN or MOUSE_WHEEL,
    "modifiers" (number)    _spaces.os.eventModifiers,
    "area" (list, optional). Area relative to the Spaces surface that the policy covers.
                            The list is interpreted as: [x, y, width, height]
    "action"                _spaces.ps.ui.policyAction}
    "eventKind", "modifiers", and "area" are filter values that are used to identify
    events that the policy affects. "action" specifies how target events should
    be propagated.
@param  callback    (function)  A callback notifier with an err argument.
*/
_spaces.ps.ui.setPointerEventPropagationPolicy = function (options, callback) {
    native function psUiSetPointerEventPropagationPolicy();
    return psUiSetPointerEventPropagationPolicy(options, callback);
};

/// enumeration controlling keyboard propagation
_spaces.ps.ui.keyboardPropagationMode = {
	/// Propagate keyboard event based on system focus.
	/// Spaces consumes keyboard events if it has focus
	FOCUS_PROPAGATE: 0,

    /// Always propagate keyboard events. Spaces will never get a keyboard event
    ALWAYS_PROPAGATE: 1,

    /// Never propagate keyboard events. Spaces consumes all keyboard events
    NEVER_PROPAGATE: 2,
};

/** Change the keyboard propagation mode.
This mode is used if there is no keyboard policies that match a given keyboard event
event.
@param mode (object)
    The propagation mode that Spaces should use in its OS views.
    The following keys are recognized:
    "defaultMode" (number, optional)
        A value from _spaces.ps.ui.keyboardPropagationMode
        The default value is FOCUS_PROPAGATE
@param  callback    (function)  A callback notifier with an err argument.
*/
_spaces.ps.ui.setKeyboardPropagationMode = function (mode, callback) {
    native function psUiSetKeyboardPropagationMode();
    return psUiSetKeyboardPropagationMode(mode, callback);
};

/** Get the keyboard propagation mode
@param callback (function)
    Callback notifier with the following signature: callback(err, mode)
    "mode" (number)     A value from _spaces.ps.ui.keyboardPropagationMode
*/
_spaces.ps.ui.getKeyboardPropagationMode = function (callback) {
    native function psUiGetKeyboardPropagationMode();
        return psUiGetKeyboardPropagationMode(callback);
    };

/** Change the keyboard propagation policy
@param options (list)
    The propagation policy that Spaces should use for OS keyboard events.
    The list may have 0 or more policies.
    A policy is an object whose form depends on the type of event.
    A keyboard policy is an object with the following keys:
    "eventKind" (number, required)
        Can be _spaces.os.eventKind.KEY_DOWN or KEY_UP,
    "modifiers" (number, optional)  _spaces.os.eventModifiers,
    "keyCode" (string)              _spaces.os.keyCode
    "keyChar" (string)              a "UTF8 char"
    "action" (number, required)     _spaces.ps.ui.policyAction
    If "keyChar" is specified, then "keyCode" is ignored. Either "keyChar" or "keyCode"
    must be specified.
    "eventKind", "modifiers", and "keyCode"/"keyChar" are filter values that are used
    to identify events that the policy affects. action specifies how target events
    should be propagated.
    The upper/lower casing of a keyChar UTF8 specifier is not-significant,
    as all UTF8 matching is performed using case-insensitive and system-locale-specific
    comparisons. Use the modifier argument to match against a specific uppercase/
    lowercase keyChar UTF8 specifier.
@param  callback    (function)  A callback notifier with an err argument.
*/
_spaces.ps.ui.setKeyboardEventPropagationPolicy = function (options, callback) {
    native function psUiSetKeyboardEventPropagationPolicy();
    return psUiSetKeyboardEventPropagationPolicy(options, callback);
};

/// enumeration controlling over-scroll mode
_spaces.ps.ui.overscrollMode = {
    /// Normal over-scroll mode, over-scroll is active when content intersects view edge.
    NORMAL_OVERSCROLL: 0,

    /// Force Photoshop to always be in over-scroll mode.
    ALWAYS_OVERSCROLL: 1,

    /// Never allow Photoshop to enter over-scroll mode.
    NEVER_OVERSCROLL: 2,
};

/** Change the over-scroll mode
@param mode (object)    The over-scroll mode that Photoshop should use in its
                        document views.
                        The argument must be an object with the following key
                          { "mode": _spaces.ps.ui.overscrollMode}
                      @see _spaces.ps.ui.overscrollMode
@param callback     (function)    A callback notifier with an err argument.
*/
_spaces.ps.ui.setOverscrollMode = function (mode, callback) {
    native function psUiSetOverscrollMode();
    return psUiSetOverscrollMode(mode, callback);
};

/** Get the over-scroll mode
@param callback     (function)  A callback notifier with the signature described below.


callback(err, mode)
@param mode (number)    the over-scroll mode. @see _spaces.ps.ui.overscrollMode
*/
_spaces.ps.ui.getOverscrollMode = function (callback) {
    native function psUiGetOverscrollMode();
    return psUiGetOverscrollMode(callback);
};

/** Set whether or not scrollbars are suppressed.
When suppressed, scrollbars never show.
When not suppressed then scrollbars follow default rules meaning that they show
in normal screen mode, but are hidden in fullscreen mode.
@param value          true if scrollbars should be suppressed
@param callback       Callback notifier with the following signature:
                        notifier(err, previousValue)
*/
_spaces.ps.ui.setSuppressScrollbars = function (value, callback) {
    native function psUiSetSuppressScrollbars();
    return psUiSetSuppressScrollbars(value, callback);
};

/** Return whether or not scrollbars are suppressed
@param callback     (function)  A callback notifier with the signature described below.

callback(err, value)
@param value (boolean)  true if scrollbars are currently suppressed.
                        false otherwise.
*/
_spaces.ps.ui.getSuppressScrollbars = function (callback) {
    native function psUiGetSuppressScrollbars();
    return psUiGetSuppressScrollbars(callback);
};

/** Set whether or not target-paths are suppressed.
@param value (boolean)  true if target-paths should be suppressed
@param callback         Callback notifier with the following signature:
                          notifier(err, previousValue)
*/
_spaces.ps.ui.setSuppressTargetPaths = function (value, callback) {
    native function psUiSetSuppressTargetPaths();
    return psUiSetSuppressTargetPaths(value, callback);
};

/** Return whether or not target-paths are suppressed
@param callback     (function)  A callback notifier with the signature described below.

callback(err, value)
@param value (boolean)  true if target-paths are currently suppressed.
                        false otherwise.
*/
_spaces.ps.ui.getSuppressTargetPaths = function (callback) {
    native function psUiGetSuppressTargetPaths();
    return psUiGetSuppressTargetPaths(callback);
};

/** The overlay offset describes how much space on each side that should be
considered opaque with respect to the document.
This value is for example used when centering documents.
Example:
The area used by Spaces (typically the document area) is 200 by 200.
We show UI on the right hand size which is 50 wide and UI at the top that is 15 tall.
In this case we would set the right component of the overlay to 50, the top component to 15,
and other values to 0.
When a new document is created, then it will be centered inside the rectangle that is
calculated by subtracting the overlay from the total area.
For the example, the document would be centered inside the rectangle (left, top, right, bottom):
    (0, 10, 150, 200)

@param options (object)
    The following keys are recognized:
    "offset": (object)   Describing the size. The following keys are recognized:
                         "left", "top", "right" and "bottom"
@param callback     (function)  A callback notifier with the signature described below.


callback(err, previousValue)
@param previousValue (object)
    The following keys are returned:
    "offset": (object) with keys: "left", "top", "right" and "bottom"
        describing the size of the chrome on the provided sides.
*/
_spaces.ps.ui.setOverlayOffsets = function (options, callback) {
    native function psUiSetOverlayOffsets();
    return psUiSetOverlayOffsets(options, callback);
};

/** Return the current size of the overlay. @See _spaces.ps.ui.setOverlayOffsets
@param callback     (function)  A callback notifier with the signature described below.


callback(err, options)
@param options (object)
    The following keys are returned:
    "offset": (object) with keys: "left", "top", "right" and "bottom"
        describing the size of the chrome on the provided sides.
*/
_spaces.ps.ui.getOverlayOffsets = function (callback) {
    native function psUiGetOverlayOffsets();
    return psUiGetOverlayOffsets(callback);
};


/** Menu bar command "kind"
Identifies a given menu "command" in the menuDescription of
_spaces.ps.ui.installMenu() as having a well-known
system-defined action (e.g: copy/paste) that is applicable
to non-Spaces view systems (like open/save dialogs).
Use this optional attribute to indicate menuitems that
invoke copy/paste functionality which also must interoperate
with copy/paste within system open/save dialogs.
Example usage:

    var menuDescription = { id : "ds", menu : [{
        label: "Edit",
        submenu: [
        {
            label: "MyMenuItem",
            command: "EDIT.MYITEMACTION",
            // User-defined action: No commandKind attribute
            shortcut: { keyChar:"q", modifiers:_spaces.os.eventModifiers.COMMAND }
        },
        {
            label: "Copy",
            command: "EDIT.COPY",
            // Well-known system action: commandKind==copy
            commandKind: _spaces.ps.ui.commandKind.COPY
            shortcut: { keyChar:"c", modifiers:_spaces.os.eventModifiers.COMMAND }
        }]
    }]};
*/
_spaces.ps.ui.commandKind = {
    USER_DEFINED:	0,

    /// command kinds related to text editing
    CUT:            1,
    COPY:           2,
    PASTE:          3,
    SELECT_ALL:     4,
    UNDO:           5,
    REDO:           6,
    DELETE:         7,

    /** command kinds related to application visibility.
    These command kinds are only used on OSX.
    These menu items will auto-enable
    */
    HIDE_APPLICATION:           8,
    HIDE_OTHER_APPLICATIONS:    9,
    HIDE_SHOW_ALL_APPLICATIONS: 10,
};

/** Install a menu bar.
TODO: Add documentation.
*
*/
_spaces.ps.ui.installMenu = function (options, menuDescription, callback) {
    native function psUiInstallMenu();
    return psUiInstallMenu(options, menuDescription, callback);
};

// ==========================================================================
// _spaces.os

if (!_spaces.os)
    _spaces.os = {};

/** OS eventKinds
*/
_spaces.os.eventKind = {
    LEFT_MOUSE_DOWN:	1,
    KEY_DOWN:			2,
    KEY_UP:				3,
    FLAGS_CHANGED:		4,
    MOUSE_WHEEL:		5,
};

/** Modifier bit fields. The command modifier is only used on Mac OS.
When used as a policy filter the modifier bit set must match exactly the
current modifier set.
*/
_spaces.os.eventModifiers = {

    // No modifier key may be pressed
    NONE:       0,
    SHIFT:      1,
    CONTROL:    2,
    ALT:        4,
    COMMAND:    8,
};

/** Keyboard KeyCode values. WIN_LEFT/WIN_RIGHT/WIN_MENU are only used on Windows.
When used as a policy filter the key-code must match exactly.
*/
_spaces.os.eventKeyCode = {
	NONE:		0,

	BACKSPACE:	8,
	TAB:		9,
	ENTER:		13,
	ESCAPE:		27,

	PAGE_UP:	33,
	PAGE_DOWN:	34,
	END:		35,
	HOME:		36,
	ARROW_LEFT:	37,
	ARROW_UP:	38,
	ARROW_RIGHT:39,
	ARROW_DOWN:	40,

	INSERT:		45,
	DELETE:		46,

	WIN_LEFT:	91,
	WIN_RIGHT:	92,
	WIN_MENU:	93,

	KEY_F1:		112,
	KEY_F2:		113,
	KEY_F3:		114,
	KEY_F4:		115,
	KEY_F5:		116,
	KEY_F6:		117,
	KEY_F7:		118,
	KEY_F8:		119,
	KEY_F9:		120,
	KEY_F10:	121,
	KEY_F11:	122,
	KEY_F12:	123,
};

/** OS notifiers
*/
_spaces.os.notifierKind = {

    /** event that is sent if Spaces's mouse capture is unexpectedly interrupted.
    Mouse capture is initiated on mouse down. If someone grabs the mouse capture before
    a corresponding mouse up is delivered, then this event is emitted.
    This event is only sent on Windows
    */
    MOUSE_CAPTURE_LOST: "mouseCaptureLost",

    /** Activation changed. Sent when the host application is brought to the
    foreground or is sent to the background.
    The provided "eventInfo" argument is a dictionary of the following
    form: {"becameActive": <boolean>}
    becameActive is true if the host application was brought to the foreground
    */
    ACTIVATION_CHANGED: "activationChanged",

    /** KeyboardFocus changed. Sent when the a change to keyboard focus has
	occurred for the CEF surface.
    The provided "eventInfo" argument is a dictionary of the following
    form: {"isActive": <boolean>}
    isActive is true if CEF surface has keyboard focus, false otherwise.
    */
    KEYBOARDFOCUS_CHANGED: "keyboardFocusChanged",

	/** A mouse down event and it was not routed to Spaces.
    This is typically used to dismiss temporal UI such as popups inside
    the Spaces surface.
    */
    EXTERNAL_MOUSE_DOWN: "externalMouseDown",

	/** A mouse wheel event re-routed to Spaces.
	*/
    EXTERNAL_MOUSE_WHEEL: "externalMouseWheel",

	/** EXTERNAL_KEYEVENT:
    A keyboard event was routed to Spaces via KeyboardPropagationPolicy.
    A keyboard event was intercepted by a ps.ui.policyAction.NEVER_PROPAGATE
    rule in the current KeyboardEventPropagationPolicy and was routed to Spaces.
    NOTE: Notification occurs *BEFORE* delivery of event via javascript onkeydown/up.
    Beware that companion onkeydown/up delivery may not occur due to the underlying
    CEF implementation when: 1) Spaces window does not have focus, or
    2) Spaces window has focus, but HTML focus is not on an editable DOM element.
    In this case, consider workaround using an EventListener("keydown/up") on
    the top level DOM window with the outermost <div tabindex="-1"/>.
    eventInfo provided is a dictionary of the following forms:

        {	eventKind:	_spaces.os.eventKind.KEY_UP|KEY_DOWN,
            modifiers:	_spaces.os.eventModifiers,
            keyCode:	_spaces.os.eventKeyCode }

        {	eventKind:	_spaces.os.eventKind.FLAGS_CHANGED,
            modifiers:	_spaces.os.eventModifiers }
    */
    EXTERNAL_KEYEVENT: "externalKeyEvent",

    /** touch event received on Spaces surface.
    provides a dict with the following key/value pairs:
      "kind":	an int, 0 => direct (tablet), 1 => indirect (trackpad)
      "id":	an int, event sequence number
      "phase":	an int, describes phase of overall event (not that useful, use phase of each touch point instead)
        0: no phase (error state)
        1: begin
        2: move
        3: stay (touch point did not move)
        4: end
        5: cancel (touch was cancelled without ending, program state changed, focus change, etc)
      "time":	a double, event time in seconds from some fixed point in the past
      "xScaleFactor" and "yScaleFactor": floats describing the scale from touch points to application space	
      "touches":	a list of touch points, in the following form:
        "touchID":	touch point ID, will not change while user continues to touch digitizer
        "phase":	phase of the individual touch point, values as above.
        "position":	a dict with the following members:
          "x" and "y":	doubles for the touch point location on the screen
    */
    TOUCH: "touch",

    /** ConvertibleSlateMode changed.
    on windows, convertible devices function as a laptop when their keyboard is attached and they are
    in landscape orientation.  upon keyboard detach or orientation change, such devices function as a 
    touch-enabled tablet. Convertible Slate Mode is true when the device is functioning as a tablet.  
    provides a dict with a single key/value pair:
    "convertibleSlateMode":  a boolean, true => tablet mode, false => laptop mode
    */
    CONVERTIBLE_SLATE_MODE_CHANGED: "convertibleSlateModeChanged",
};

// TODO: Delete after 8/20/2014
_spaces.os.eventTypes = _spaces.os.notifierKind;

/** Register an event listener with the OS.
Note: Only one listener can be active at any time.
TODO:REMOVE
@param callback     (function)  A callback notifier with the signature described below.

callback(err, eventType, eventInfo)
@param eventType (number)   one of the values in _spaces.os.notifierKind
@param eventInfo (variant)  specific to the provided event
*/
_spaces.os.registerEventListener = function (callback) {

        console.log("_spaces.os.registerEventListener is deprecated. Please use _spaces.setNotifier");

        _spaces.setNotifier(_spaces.notifierGroup.OS, {},
            function(err, notifierKind, info) {
                callback(err, notifierKind, info);
            }
        );
};
// TODO:REMOVE
_spaces.os.unRegisterEventListener = function (callback) {
        console.log("_spaces.os.unRegisterEventListener is deprecated. Please use _spaces.setNotifier");
        _spaces.setNotifier(_spaces.notifierGroup.OS, {}, undefined);
};

/** Post an OS event to the event queue.
@param eventInfo (object)	Describes the event that should be synthesized
    "eventInfo" has the following form for pointer events:
    {	eventKind:	_spaces.os.eventKind.LEFT_MOUSE_DOWN,
        location:	[x, y]
    }
    The location is relative to the Spaces surface and its origin is in the
    top left corner.

    "eventInfo" has the following form for keyboard events:
    {	eventKind:	_spaces.os.eventKind.KEY_UP|KEY_DOWN,
        modifiers:	_spaces.os.eventModifiers,
        keyCode:	_spaces.os.eventKeyCode,
    }
@param options (object)     currently unused.
@param callback     (function)    A callback notifier with an err argument.
*/
_spaces.os.postEvent = function (eventInfo, options, callback) {
    native function osPostEvent();
    return osPostEvent(eventInfo, options, callback);
};

if (!_spaces.os.clipboard)
    _spaces.os.clipboard = {};

/** Read data from the OS clipboard.
@param options (object)
    The options argument has the following form:
        "formats":  format_specifier_list
    The order of the format list is significant. In the case where multiple formats
    match the contents of the clipboard, the first format listed will be used.
    Spaces operates with two types of formats:
    - built-in formats. These formats map to well defined OS formats. The following
        lists the built in formats:
        "string"    This format can be used to read and write unicode string data.
                    The data for the values of type string is "string".
    - custom formats. Any format that is not a built-in format is considered a custom format.
        The custom format can have any identifier. On OSX Apple recommends a UTI syntax such
        as the following for custom formats: "com.mycompany.myapp.myspecialfiletype".
        See the following URL for information about custom types on OSX:
        https://developer.apple.com/library/ios/documentation/FileManagement/Conceptual/understanding_utis/understand_utis_conc/understand_utis_conc.html#//apple_ref/doc/uid/TP40001319-CH202-CHDHIJDE
@param callback     (function)  A callback notifier with the signature described below.


callback(err, info)
Info contains the following keys:
    "format":   actual_format
    "data":     clipboard data
If the clipboard does not have the requested format, an error will be returned in err.
Otherwise err will be undefined or 0
*/
_spaces.os.clipboard.read = function (options, callback) {
    native function osClipboardRead();
    return osClipboardRead(options, callback);
};

/** Replace the contents of the OS clipboard with the provided data.
@param options (object)
    The options argument has the following form:
        "format":   format_specifier
        "data":     clipboard data
    See "_spaces.os.clipboard.read" for a description of formats.
@param callback     (function)    A callback notifier with an err argument.
*/
_spaces.os.clipboard.write = function (options, callback) {
    native function osClipboardWrite();
    return osClipboardWrite(options, callback);
};

/** Set the tooltip for the host application.
In general tooltips are managed via normal HTML properties.
This API is provided for the rare case that javascript needs to be able to
control tooltip directly via script.
Note: This API is a no-op if the host application is not the frontmost application.

@param options (object)
    Argument specifying the tooltip to set. The following keys are recognized:
    "label" (optional, string)  the tooltip string to use. If this key is omitted,
                                or if the string is empty, then the tooltip is
                                hidden
@param callback     (function)    A callback notifier with an err argument.
*/
_spaces.os.setTooltip = function (options, callback) {
    native function osSetTooltip();
    return osSetTooltip(options, callback);
};

/** Reset the cursor.
This method will cause the OS cursor to be updated.
This method is typically only needed when control over the cursor changes as a
result of changing mouse policy, or as a result of a change to the modal state.
@param options      (currently unused)
@param callback     (function)    A callback notifier with an err argument.
*/
_spaces.os.resetCursor = function (options, callback) {
    native function osResetCursor();
    return osResetCursor(options, callback);
};

// ==========================================================================
// _spaces.os.keyboardFocus

if (!_spaces.os.keyboardFocus)
    _spaces.os.keyboardFocus = {};

/* Request keyboard focus from host application
@param options              (Currently unused).
@param callback (function)  A callback notifier with an err argument.
*/
_spaces.os.keyboardFocus.acquire = function (options, callback) {
    native function osKeyboardFocusAcquire();
    return osKeyboardFocusAcquire(options, callback);
};

/* Release keyboard focus to host application
@param options              (Currently unused).
@param callback (function)  A callback notifier with an err argument.
*/
_spaces.os.keyboardFocus.release = function (options, callback) {
    native function osKeyboardFocusRelease();
    return osKeyboardFocusRelease(options, callback);
};

/* Query whether Cef surface currently has keyboard focus
@param options              (Currently unused).
@param callback (function)  A callback notifier with the signature described below.

callback(err, isActive)
@param isActive (boolean)   true if Cef has keyboard focus
*/
_spaces.os.keyboardFocus.isActive = function (options, callback) {
    native function osKeyboardFocusIsActive();
    return osKeyboardFocusIsActive(options, callback);
};

// ==========================================================================
// _spaces.os.isConvertibleSlateMode

/** Returns the current ConvertibleSlateMode.
On Windows, convertible devices such as Surface report true when keyboard is detached, or
the orientation is not landscape with keyboard at the bottom.
On Mac, this method returns false.
@param callback     (function)  A callback notifier with the signature described below.

callback(err, mode)
@param mode (boolean) true if convertibleSlateMode is true, i.e., device is in tablet mode
*/
_spaces.os.isConvertibleSlateMode = function (callback) {
    native function osIsConvertibleSlateMode();
    return osIsConvertibleSlateMode(callback);
};

// ==========================================================================
// _spaces.debug
// prefix with "_" to signify that this area is for internal Adobe use only

if (!_spaces._debug)
   _spaces._debug = {};

_spaces._debug.getRemoteDebuggingPort = function () {
    native function pgDebugGetRemoteDebuggingPort();
    return pgDebugGetRemoteDebuggingPort();
};

/** log the provided string by using the host logging system */
_spaces._debug.logMessage = function (message) {
    native function pgDebugLogMessage(message);
    return pgDebugLogMessage(message);
};

/** Show/Hide the developer tools
@param doShow (boolean)     if true, then the dev tools will be shown.
                            if false then the dev tools will be hidden.
@param callback (function)  A callback notifier with an err argument.
*/
_spaces._debug.showHideDevTools = function (doShow, callback) {
    native function pgShowDevTools();
    return pgShowDevTools(doShow, callback);
};

/** Debug method for forcing a call to descriptor.play with an incorrect
number of arguments while the last argument is still a notifier.
@notifier.      The notifier for this method.
                The expected result is that the notifier is invoked
                with an incorrect number of arguments error
*/
_spaces._debug.forcePlayArgumentFailure = function (notifier) {
    native function psDescPlay();
    return psDescPlay(notifier);
};

/** This method converts provided arguments from V8 to action descriptor types, and
back again.
The method is used to verify that the type conversion is working as expected.
The signature of the callback is as follows:
    callback(err, descriptor, reference)
*/
_spaces._debug.descriptorIdentity = function (descriptor, reference, callback) {
    native function psDebugDescIdentity();
    return psDebugDescIdentity(descriptor, reference, callback);
};
        
/** This method forces a C++ exception to occur in the native method dispatching.
Expected result: This method should throw a javascript exception
*/
_spaces._debug.testNativeDispatcherException = function () {
     native function pgDebugTestExecuteException();
     return pgDebugTestExecuteException();
};

/** Enable / disable the contextual context menu in Cef.
The contextual debug menu is enabled by default in debug builds

@param value (boolean)      true if the debug menu should be enabled
@param callback (function)  A callback notifier with an err argument.
*/
_spaces._debug.enableDebugContextMenu = function (value, callback) {
     native function pgDebugEnableDebugContextMenu();
     return pgDebugEnableDebugContextMenu(value, callback);
};

// ==========================================================================
// _spaces.config
// Various options that determine how Spaces/adapter layer operates

if (!_spaces.config)
   _spaces.config = {};

// ==========================================================================
// Deprecated functionality

/** This API controls the execution parameters of the Spaces environment.
* @param options    a dictionary of execution mode parameters:
*                       "latentVisibility": <bool>
*                   controls whether or not the Spaces surface is visible
*                   when its ancestor hierarchy is visible.
*                       "suspended": <bool>
*                   When a Spaces is suspended, then its cpu resource usage is reduced.
*                   The javascript environment can send messages while Spaces
*                   is suspended
* @param callback   Callback notifier with the following signature:
*                   notifier(err, previousValue)
*                        @param err         undefined, or 0 on success.
*                        @param previousValue the mode value before the set call
*/
_spaces.setExecutionMode = function (options, callback) {
    native function pgSetExecutionMode();
    return pgSetExecutionMode(options, callback);
};

/** Return the current value of the execution mode
*/
_spaces.getExecutionMode = function (callback) {
    native function pgGetExecutionMode();
    return pgGetExecutionMode(callback);
};

// temporary alias
var _playground = _spaces;
