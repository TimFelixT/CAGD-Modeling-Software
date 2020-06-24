/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

WebInspector.SourceCodeTimeline = class SourceCodeTimeline extends WebInspector.Timeline
{
    constructor(sourceCode, sourceCodeLocation, recordType, recordEventType)
    {
        super();

        console.assert(sourceCode);
        console.assert(!sourceCodeLocation || sourceCodeLocation.sourceCode === sourceCode);
        console.assert(recordType);

        this._sourceCode = sourceCode;
        this._sourceCodeLocation = sourceCodeLocation || null;
        this._recordType = recordType;
        this._recordEventType = recordEventType || null;
    }

    // Public

    get sourceCode() { return this._sourceCode; }
    get sourceCodeLocation() { return this._sourceCodeLocation; }
    get recordType() { return this._recordType; }
    get recordEventType() { return this._recordEventType; }

    saveIdentityToCookie(cookie)
    {
        cookie[WebInspector.SourceCodeTimeline.SourceCodeURLCookieKey] = this._sourceCode.url ? this._sourceCode.url.hash : null;
        cookie[WebInspector.SourceCodeTimeline.SourceCodeLocationLineCookieKey] = this._sourceCodeLocation ? this._sourceCodeLocation.lineNumber : null;
        cookie[WebInspector.SourceCodeTimeline.SourceCodeLocationColumnCookieKey] = this._sourceCodeLocation ? this._sourceCodeLocation.columnNumber : null;
        cookie[WebInspector.SourceCodeTimeline.RecordTypeCookieKey] = this._recordType || null;
        cookie[WebInspector.SourceCodeTimeline.RecordEventTypeCookieKey] = this._recordEventType || null;
    }
};

WebInspector.SourceCodeTimeline.TypeIdentifier = "source-code-timeline";
WebInspector.SourceCodeTimeline.SourceCodeURLCookieKey = "source-code-timeline-source-code-url";
WebInspector.SourceCodeTimeline.SourceCodeLocationLineCookieKey = "source-code-timeline-source-code-location-line";
WebInspector.SourceCodeTimeline.SourceCodeLocationColumnCookieKey = "source-code-timeline-source-code-location-column";
WebInspector.SourceCodeTimeline.SourceCodeURLCookieKey = "source-code-timeline-source-code-url";
WebInspector.SourceCodeTimeline.RecordTypeCookieKey = "source-code-timeline-record-type";
WebInspector.SourceCodeTimeline.RecordEventTypeCookieKey = "source-code-timeline-record-event-type";
