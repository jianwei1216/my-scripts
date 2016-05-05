$def with (content)
/***************************************************************************
 *   Copyright (C) 2008 by pythonSysInfo - A PHP System Information Script    *
 *   http://phpsysinfo.sourceforge.net/                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//
// $(ELT)Id: phpsysinfo.js 419 2011-01-18 08:28:56Z jacky672 $(ELT)
//

/*global $(ELT), jQuery */

"use strict";

var langxml = [], langcounter = 1, filesystemTable, cookie_language = "", cookie_template = "", plugin_liste = [], langarr = [];
/**
 * generate a cookie, if not exist, and add an entry to it<br><br>
 * inspired by <a href="http://www.quirksmode.org/js/cookies.html">http://www.quirksmode.org/js/cookies.html</a>
 * @param {String} name name that holds the value
 * @param {String} value value that needs to be stored
 * @param {Number} days how many days the entry should be valid in the cookie
 */
function createCookie(name, value, days) {
    var date = new Date(), expires = "";
    if (days) {
        date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
        expires = "; expires=" + date.toGMTString();
    }
    else {
        expires = "";
    }
    document.cookie = name + "=" + value + expires + "; path=/";
}

/**
 * read a value out of a cookie and return the value<br><br>
 * inspired by <a href="http://www.quirksmode.org/js/cookies.html">http://www.quirksmode.org/js/cookies.html</a>
 * @param {String} name name of the value that should be retrieved
 * @return {String}
 */
function readCookie(name) {
    var nameEQ = "", ca = [], c = '', i = 0;
    nameEQ = name + "=";
    ca = document.cookie.split(';');
    for (i = 0; i < ca.length; i += 1) {
        c = ca[i];
        while (c.charAt(0) === ' ') {
            c = c.substring(1, c.length);
        }
        if (!c.indexOf(nameEQ)) {
            return c.substring(nameEQ.length, c.length);
        }
    }
    return null;
}

/**
 * round a given value to the specified precision, difference to Math.round() is that there
 * will be appended Zeros to the end if the precision is not reached (0.1 gets rounded to 0.100 when precision is set to 3)
 * @param {Number} x value to round
 * @param {Number} n precision
 * @return {String}
 */
function round(x, n) {
    var e = 0, k = "";
    if (n < 1 || n > 14) {
        return 0;
    }
    e = Math.pow(10, n);
    k = (Math.round(x * e) / e).toString();
    if (k.indexOf('.') === -1) {
        k += '.';
    }
    k += e.toString().substring(1);
    return k.substring(0, k.indexOf('.') + n + 1);
}

/**
 * activates a given style and disables the old one in the document
 * @param {String} template template that should be activated
 */
function switchStyle(template) {
    $(ELT)('link[rel*=style][title]').each(function getTitle(i) {
        if (this.getAttribute('title') === 'PSI_Template') {
            this.setAttribute('href', './templates/' + template + ".css");
        }
    });
    createCookie('template', template, 365);
}

/**
 * internal function to get a given translation out of the translation file
 * @param {Number} langId id of the translation expression
 * @param {String} [plugin] name of the plugin
 * @return {String} translation string
 */
function getTranslationString(langId, plugin) {
    var plugname = cookie_language + "_";
    if (plugin === undefined) {
        plugname += "pythonSysInfo";
    }
    else {
        plugname += plugin;
    }
    if (langxml[plugname] === undefined) {
        langxml.push(plugname);
        getLanguage(cookie_language, plugin, plugname);
    }
    return langarr[plugname][langId.toString()];
}

/**
 * generate a span tag with an unique identifier to be html valid
 * @param {Number} id translation id in the xml file
 * @param {Boolean} generate generate lang_id in span tag or use given value
 * @param {String} [plugin] name of the plugin for which the tag should be generated
 * @return {String} string which contains generated span tag for translation string
 */
function genlang(id, generate, type) {
    var html = "", idString = "", plugname = "";
    if (type === undefined) {
        plugname = "None";
    }
    else {
        plugname = type 
    }
    if (id < 100) {
        if (id < 10) {
            idString = "00" + id.toString();
        }
        else {
            idString = "0" + id.toString();
        }
    }
    else {
        idString = id.toString();
    }
    if (generate) {
        html += "<span id=\"lang_" + idString + "-" + langcounter.toString() + "\">";
        langcounter += 1;
    }
    else {
        html += "<span id=\"lang_" + idString + "\">";
    }
    html += plugname + "</span>";
    return html;
}

/**
 * translates all expressions based on the translation xml file<br>
 * translation expressions must be in the format &lt;span id="lang???"&gt;&lt;/span&gt;, where ??? is
 * the number of the translated expression in the xml file<br><br>if a translated expression is not found in the xml
 * file nothing would be translated, so the initial value which is inside the span tag is displayed
 * @param {String} [plugin] name of the plugin
 */
function changeLanguage(plugin) {
    var langId = "", langStr = "";
    $(ELT)('span[id*=lang_]').each(function translate(i) {
        langId = this.getAttribute('id').substring(5);
        if (langId.indexOf('-') !== -1) {
            langId = langId.substring(0, langId.indexOf('-')); //remove the unique identifier
        }
//        langStr = getTranslationString(langId, plugin);
        if (langStr !== undefined) {
            if (langStr.length > 0) {
                this.innerHTML = langStr;
            }
        }
    });
}

/**
 * generate the filesystemTable and activate the dataTables plugin on it
 */
function filesystemtable() {
    var html = "";
    html += "        <h2>" + genlang(30, false, '$_("mounted filesystems")') + "</h2>\n";
    html += "        <table id=\"filesystemTable\" cellspacing=\"0\">\n";
    html += "          <thead>\n";
    html += "            <tr>\n";
    html += "              <th>" + genlang(31, false, '$_("mountpoint")') + "</th>\n";
    html += "              <th>" + genlang(34, false, '$_("type")') + "</th>\n";
    html += "              <th>" + genlang(32, false, '$_("partition")') + "</th>\n";
    html += "              <th>" + genlang(33, false, '$_("usage")') + "</th>\n";
    html += "              <th class=\"right\">" + genlang(35, true, '$_("free")') + "</th>\n";
    html += "              <th class=\"right\">" + genlang(36, true, '$_("used")') + "</th>\n";
    html += "              <th class=\"right\">" + genlang(37, true, '$_("size")') + "</th>\n";
    html += "            </tr>\n";
    html += "          </thead>\n";
    html += "          <tfoot>\n";
    html += "            <tr style=\"font-weight : bold\">\n";
    html += "              <td>&nbsp;</td>\n";
    html += "              <td>&nbsp;</td>\n";
    html += "              <td>" + genlang(38, false, '$_("totals")') + "</td>\n";
    html += "              <td id=\"s_fs_total\"></td>\n";
    html += "              <td class=\"right\"><span id=\"s_fs_tfree\"></span></td>\n";
    html += "              <td class=\"right\"><span id=\"s_fs_tused\"></span></td>\n";
    html += "              <td class=\"right\"><span id=\"s_fs_tsize\"></span></td>\n";
    html += "            </tr>\n";
    html += "          </tfoot>\n";
    html += "          <tbody>\n";
    html += "          </tbody>\n";
    html += "        </table>\n";
    
    $(ELT)("#filesystem").append(html);
    
    filesystemTable = $(ELT)("#filesystemTable").dataTable({
        "bPaginate": false,
        "bLengthChange": false,
        "bFilter": false,
        "bSort": true,
        "bInfo": false,
        "bProcessing": true,
        "bAutoWidth": false,
        "bStateSave": true,
        "aoColumns": [{
            "sType": 'span-string',
            "sWidth": "100px"
        }, {
            "sType": 'span-string',
            "sWidth": "50px"
        }, {
            "sType": 'span-string',
            "sWidth": "200px"
        }, {
            "sType": 'span-number'
        }, {
            "sType": 'span-number',
            "sWidth": "80px",
            "sClass": "right"
        }, {
            "sType": 'span-number',
            "sWidth": "80px",
            "sClass": "right"
        }, {
            "sType": 'span-number',
            "sWidth": "80px",
            "sClass": "right"
        }]
    });
}

/**
 * fill all errors from the xml in the error div element in the document and show the error icon
 * @param {jQuery} xml pythonSysInfo-XML
 */
function populateErrors(xml) {
    var values = false;
    $(ELT)("Errors Error", xml).each(function getError(id) {
        $(ELT)("#errorlist").append("<b>" + $(ELT)(this).attr("Function") + "</b><br/><br/><pre>" + $(ELT)(this).text() + "</pre><hr>");
        values = true;
    });
    if (values) {
        $(ELT)("#warn").css("display", "inline");
    }
}

/**
 * show the page
 * @param {jQuery} xml pythonSysInfo-XML
 */
function displayPage(xml) {
    var versioni = "";
    if (cookie_template !== null) {
        $(ELT)("#template").val(cookie_template);
    }
    if (cookie_language !== null) {
        $(ELT)("#lang").val(cookie_language);
    }
    $(ELT)("#loader").hide();
    $(ELT)("#container").fadeIn("slow");
//    versioni = $(ELT)("Generation", xml).attr("version").toString();
//    $(ELT)("#version").html(versioni);
    
    $(ELT)("Options", xml).each(function getOptions(id) {
        var showPickListLang = "", showPickListTemplate = "";
        showPickListLang = $(ELT)(this).attr("showPickListLang");
        showPickListTemplate = $(ELT)(this).attr("showPickListTemplate");
        if (showPickListTemplate === 'false') {
            $(ELT)('#template').hide();
            $(ELT)('span[id=lang_044]').hide();
        }
        if (showPickListLang === 'false') {
            $(ELT)('#lang').hide();
            $(ELT)('span[id=lang_045]').hide();
        }
    });
}

/**
 * format seconds to a better readable statement with days, hours and minutes
 * @param {Number} sec seconds that should be formatted
 * @return {String} html string with no breaking spaces and translation statements
 */
function formatUptime(sec) {
    var txt = "", intMin = 0, intHours = 0, intDays = 0;
    intMin = sec / 60;
    intHours = intMin / 60;
    intDays = Math.floor(intHours / 24);
    intHours = Math.floor(intHours - (intDays * 24));
    intMin = Math.floor(intMin - (intDays * 60 * 24) - (intHours * 60));
    if (intDays) {
        txt += intDays.toString() + "&nbsp;" + genlang(48, false, '$_("days")') + "&nbsp;";
    }
    if (intHours) {
        txt += intHours.toString() + "&nbsp;" + genlang(49, false, '$_("hours")') + "&nbsp;";
    }
    return txt + intMin.toString() + "&nbsp;" + genlang(50, false, '$_("minutes")');
}

/**
 * format a given MHz value to a better readable statement with the right suffix
 * @param {Number} mhertz mhertz value that should be formatted
 * @return {String} html string with no breaking spaces and translation statements
 */
function formatHertz(mhertz) {
    if (mhertz && mhertz < 1000) {
        return mhertz.toString() + "&nbsp;" + genlang(92, true, 'Hz');
    }
    else {
        if (mhertz && mhertz >= 1000) {
            return round(mhertz / 1000, 2) + "&nbsp;" + genlang(93, true, 'MHz');
        }
        else {
            return "";
        }
    }
}

/**
 * format the byte values into a user friendly value with the corespondenting unit expression<br>support is included
 * for binary and decimal output<br>user can specify a constant format for all byte outputs or the output is formated
 * automatically so that every value can be read in a user friendly way
 * @param {Number} bytes value that should be converted in the corespondenting format, which is specified in the config.php
 * @param {jQuery} xml pythonSysInfo-XML
 * @return {String} string of the converted bytes with the translated unit expression
 */
function formatBytes(bytes, xml) {
    var byteFormat = "", show = "";
    
    $(ELT)("Options", xml).each(function getByteFormat(id) {
        byteFormat = $(ELT)(this).attr("byteFormat");
    });
    
    switch (byteFormat) {
    case "PiB":
        show += round(bytes / Math.pow(1024, 5), 2);
        show += "&nbsp;" + genlang(90, true, 'PiB');
        break;
    case "TiB":
        show += round(bytes / Math.pow(1024, 4), 2);
        show += "&nbsp;" + genlang(86, true, 'TiB');
        break;
    case "GiB":
        show += round(bytes / Math.pow(1024, 3), 2);
        show += "&nbsp;" + genlang(87, true, 'GiB');
        break;
    case "MiB":
        show += round(bytes / Math.pow(1024, 2), 2);
        show += "&nbsp;" + genlang(88, true, 'MiB');
        break;
    case "KiB":
        show += round(bytes / Math.pow(1024, 1), 2);
        show += "&nbsp;" + genlang(89, true, 'KiB');
        break;
    case "PB":
        show += round(bytes / Math.pow(1000, 5), 2);
        show += "&nbsp;" + genlang(91, true, 'PB');
        break;
    case "TB":
        show += round(bytes / Math.pow(1000, 4), 2);
        show += "&nbsp;" + genlang(85, true, 'TB');
        break;
    case "GB":
        show += round(bytes / Math.pow(1000, 3), 2);
        show += "&nbsp;" + genlang(41, true, 'GB');
        break;
    case "MB":
        show += round(bytes / Math.pow(1000, 2), 2);
        show += "&nbsp;" + genlang(40, true, 'MB');
        break;
    case "KB":
        show += round(bytes / Math.pow(1000, 1), 2);
        show += "&nbsp;" + genlang(39, true, 'KB');
        break;
    case "auto_decimal":
        if (bytes > Math.pow(1000, 5)) {
            show += round(bytes / Math.pow(1000, 5), 2);
            show += "&nbsp;" + genlang(91, true, 'PB');
        }
        else {
            if (bytes > Math.pow(1000, 4)) {
                show += round(bytes / Math.pow(1000, 4), 2);
                show += "&nbsp;" + genlang(85, true, 'TB');
            }
            else {
                if (bytes > Math.pow(1000, 3)) {
                    show += round(bytes / Math.pow(1000, 3), 2);
                    show += "&nbsp;" + genlang(41, true, 'GB');
                }
                else {
                    if (bytes > Math.pow(1000, 2)) {
                        show += round(bytes / Math.pow(1000, 2), 2);
                        show += "&nbsp;" + genlang(40, true, 'MB');
                    }
                    else {
                        show += round(bytes / Math.pow(1000, 1), 2);
                        show += "&nbsp;" + genlang(39, true, 'KB');
                    }
                }
            }
        }
        break;
    default:
        if (bytes > Math.pow(1024, 5)) {
            show += round(bytes / Math.pow(1024, 5), 2);
            show += "&nbsp;" + genlang(90, true, 'PiB');
        }
        else {
            if (bytes > Math.pow(1024, 4)) {
                show += round(bytes / Math.pow(1024, 4), 2);
                show += "&nbsp;" + genlang(86, true, 'TiB');
            }
            else {
                if (bytes > Math.pow(1024, 3)) {
                    show += round(bytes / Math.pow(1024, 3), 2);
                    show += "&nbsp;" + genlang(87, true, 'GiB');
                }
                else {
                    if (bytes > Math.pow(1024, 2)) {
                        show += round(bytes / Math.pow(1024, 2), 2);
                        show += "&nbsp;" + genlang(88, true, 'MiB');
                    }
                    else {
                        show += round(bytes / Math.pow(1024, 1), 2);
                        show += "&nbsp;" + genlang(89, true, 'KiB');
                    }
                }
            }
        }
    }
    return show;
}

/**
 * format a celcius temperature to fahrenheit and also append the right suffix
 * @param {String} degreeC temperature in celvius
 * @param {jQuery} xml pythonSysInfo-XML
 * @return {String} html string with no breaking spaces and translation statements
 */
function formatTemp(degreeC, xml) {
    var tempFormat = "", degree = 0;
    
    $(ELT)("Options", xml).each(function getOptions(id) {
        tempFormat = $(ELT)(this).attr("tempFormat").toString().toLowerCase();
    });
    
    degree = parseFloat(degreeC);
    if (isNaN(degreeC)) {
        return "---";
    }
    else {
        switch (tempFormat) {
        case "f":
            return round((((9 * degree) / 5) + 32), 1) + "&nbsp;" + genlang(61, true);
        case "c":
            return round(degree, 1) + "&nbsp;" + genlang(60, true);
        case "c-f":
            return round(degree, 1) + "&nbsp;" + genlang(60, true) + "<br><i>(" + round((((9 * degree) / 5) + 32), 1) + "&nbsp;" + genlang(61, true) + ")</i>";
        case "f-c":
            return round((((9 * degree) / 5) + 32), 1) + "&nbsp;" + genlang(61, true) + "<br><i>(" + round(degree, 1) + "&nbsp;" + genlang(60, true) + ")</i>";
        }
    }
}

/**
 * create a visual HTML bar from a given size, the layout of that bar can be costumized through the bar css-class
 * @param {Number} size
 * @return {String} HTML string which contains the full layout of the bar
 */
function createBar(size) {
	if(!Number(size)){
		size=0;
	}
    return "<div class=\"bar\" style=\"float:left; width: " + size + "px;\">&nbsp;</div>&nbsp;" + size + "%";
}

/**
 * (re)fill the vitals block with the values from the given xml
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshVitals(xml) {
    var hostname = "", ip = "", kernel = "", distro = "", icon = "", uptime = "", users = 0, loadavg = "";
    $(ELT)("Vitals", xml).each(function getVitals(id) {
        hostname = $(ELT)(this).attr("Hostname");
        ip = $(ELT)(this).attr("IPAddr");
        kernel = $(ELT)(this).attr("Kernel");
        distro = $(ELT)(this).attr("Distro");
        icon = $(ELT)(this).attr("Districon");
        uptime = formatUptime(parseInt($(ELT)(this).attr("Uptime"), 10));
        users = parseInt($(ELT)(this).attr("Users"), 10);
        loadavg = $(ELT)(this).attr("LoadAvg");
        if ($(ELT)(this).attr("CPULoad") !== undefined) {
            loadavg = loadavg + "<br/>" + createBar(parseInt($(ELT)(this).attr("CPULoad"), 10));
        }
        document.title = '$_("system information")' + " :" + hostname + " (" + ip + ")";
        $(ELT)("#s_hostname_title").html(hostname);
        $(ELT)("#s_ip_title").html(ip);
        $(ELT)("#s_hostname").html(hostname);
        $(ELT)("#s_ip").html(ip);
        $(ELT)("#s_kernel").html(kernel);
        $(ELT)("#s_distro").html("<img src='/static/theme/redmond/images/gfx/images/" + icon + ".png" + "' alt='Icon' height='16' width='16' style='vertical-align:middle;' />&nbsp;" + distro);
        $(ELT)("#s_uptime").html(uptime);
        $(ELT)("#s_users").html(users);
        $(ELT)("#s_loadavg").html(parseFloat(loadavg).toFixed(2)+'&nbsp;%');
    });
}

/**
 * build the cpu information as table rows
 * @param {jQuery} xml pythonSysInfo-XML
 * @param {Array} tree array that holds the positions for treetable plugin
 * @param {Number} rootposition position of the parent element
 * @param {Array} collapsed array that holds all collapsed elements hwne opening page
 */
function fillCpu(xml, tree, rootposition, collapsed) {
    var cpucount = 0, html = "";
    $(ELT)("Hardware CPU CpuCore", xml).each(function getCpuCore(cpuCoreId) {
        var model = "", speed = 0, bus = 0, cache = 0, bogo = 0, temp = 0, load = 0, cpucoreposition = 0, virt = "";
        cpucount += 1;
        model = $(ELT)(this).attr("Model");
        speed = parseInt($(ELT)(this).attr("CpuSpeed"), 10);
        cache = parseInt($(ELT)(this).attr("Cache"), 10);
        virt = $(ELT)(this).attr("Virt");
        bus = parseInt($(ELT)(this).attr("BusSpeed"), 10);
        temp = parseInt($(ELT)(this).attr("Cputemp"), 10);
        bogo = parseInt($(ELT)(this).attr("Bogomips"), 10);
        load = parseInt($(ELT)(this).attr("Load"), 10);

        html += "<tr><td colspan=\"2\">" + model + "</td></tr>\n";
        cpucoreposition = tree.push(rootposition);
        if (!isNaN(speed)) {
            html += "<tr><td style=\"width:50%\">" + genlang(13, true, '$_("cpu speed")') + ":</td><td>" + formatHertz(speed) + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
        collapsed.push(cpucoreposition);
        if (!isNaN(cache)) {
            html += "<tr><td style=\"width:50%\">" + genlang(15, true, '$_("cache size")') + ":</td><td>" + formatBytes(cache) + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
        if (virt != undefined) {
            html += "<tr><td style=\"width:50%\">" + genlang(94, true, '$_("virtualization")') + ":</td><td>" + virt + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
        if (!isNaN(bus)) {
            html += "<tr><td style=\"width:50%\">" + genlang(14, true, '$_("bus speed")') + ":</td><td>" + formatHertz(bus) + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
        if (!isNaN(bogo)) {
            html += "<tr><td style=\"width:50%\">" + genlang(16, true, '$_("system bogomips")') + ":</td><td>" + bogo.toString() + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
        if (!isNaN(temp)) {
            html += "<tr><td style=\"width:50%\">" + genlang(51, true, '$_("cpu temp")') + ":</td><td>" + formatTemp(temp, xml) + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
        if (!isNaN(load)) {
            html += "<tr><td style=\"width:50%\">" + genlang(9, true) + ":</td><td>" + createBar(load) + "</td></tr>\n";
            tree.push(cpucoreposition);
        }
    });
    if (cpucount === 0) {
        html += "<tr><td colspan=\"2\">" + genlang(42, true) + "</td></tr>\n";
        tree.push(rootposition);
    }
    return html;
}

/**
 * build rows for a treetable out of the hardwaredevices
 * @param {jQuery} xml pythonSysInfo-XML
 * @param {String} type type of the hardware device
 * @param {Array} tree array that holds the positions for treetable plugin
 * @param {Number} rootposition position of the parent element
 */
function fillHWDevice(xml, type, tree, rootposition) {
    var devicecount = 0, html = "";
    $(ELT)("Hardware " + type + " Device", xml).each(function getPciDevice(deviceId) {
        var name = "", count = 0;
        devicecount += 1;
        name = $(ELT)(this).attr("Name");
        count = parseInt($(ELT)(this).attr("Count"), 10);
        if (!isNaN(count) && count > 1) {
            name = "(" + count + "x) " + name;
        }
        html += "<tr><td colspan=\"2\">" + name + "</td></tr>\n";
        tree.push(rootposition);
    });
    if (devicecount === 0) {
        html += "<tr><td colspan=\"2\">" + genlang(42, true) + "</td></tr>\n";
        tree.push(rootposition);
    }
    return html;
}

/**
 * (re)fill the hardware block with the values from the given xml
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshHardware(xml) {
    var html = "", tree = [], closed = [], index = 0;
    $(ELT)("#hardware").empty();
    html += "  <h2>" + genlang(10, false, '$_("hardware information")') + "</h2>\n";
    html += "  <table id=\"HardwareTree\" class=\"tablemain\" style=\"width:100%;\">\n";
    html += "   <tbody class=\"tree\">\n";
    
    html += "    <tr><td colspan=\"2\"><b>" + genlang(11, false, '$_("Processors")') + "</b></td></tr>\n";
    html += fillCpu(xml, tree, tree.push(0), closed);
    
    html += "    <tr><td colspan=\"2\"><b>" + genlang(17, false, '$_("PCI")') + "</b></td></tr>\n";
    index = tree.push(0);
    closed.push(index);
    html += fillHWDevice(xml, 'PCI', tree, index);
    
    html += "    <tr><td colspan=\"2\"><b>" + genlang(18, false, '$_("IDE")') + "</b></td></tr>\n";
    index = tree.push(0);
    closed.push(index);
    html += fillHWDevice(xml, 'IDE', tree, index);
    
    html += "    <tr><td colspan=\"2\"><b>" + genlang(19, false, '$_("SCSI")') + "</b></td></tr>\n";
    index = tree.push(0);
    closed.push(index);
    html += fillHWDevice(xml, 'SCSI', tree, index);
    
    html += "    <tr><td colspan=\"2\"><b>" + genlang(20, false, '$_("USB")') + "</b></td></tr>\n";
    index = tree.push(0);
    closed.push(index);
    html += fillHWDevice(xml, 'USB', tree, index);
    
    html += "   </tbody>\n";
    html += "  </table>\n";
    $(ELT)("#hardware").append(html);
    
    $(ELT)("#HardwareTree").jqTreeTable(tree, {
        openImg: "/static/theme/redmond/images/gfx/treeTable/tv-collapsable.gif",
        shutImg: "/static/theme/redmond/images/gfx/treeTable/tv-expandable.gif",
        leafImg: "/static/theme/redmond/images/gfx/treeTable/tv-item.gif",
        lastOpenImg: "/static/theme/redmond/images/gfx/treeTable/tv-collapsable-last.gif",
        lastShutImg: "/static/theme/redmond/images/gfx/treeTable/tv-expandable-last.gif",
        lastLeafImg: "/static/theme/redmond/images/gfx/treeTable/tv-item-last.gif",
        vertLineImg: "/static/theme/redmond/images/gfx/treeTable/vertline.gif",
        blankImg: "/static/theme/redmond/images/gfx/treeTable/blank.gif",
        collapse: closed,
        column: 0,
        striped: true,
        highlight: false,
        state: false
    });
}

/**
 *(re)fill the network block with the values from the given xml
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshNetwork(xml) {
    var name = "", rx = 0, tx = 0, er = 0, dr = 0;
    $(ELT)("#tbody_network").empty();
    $(ELT)("Network NetDevice", xml).each(function getDevice(id) {
        name = $(ELT)(this).attr("Name");
        rx = parseInt($(ELT)(this).attr("RxBytes"), 10);
        tx = parseInt($(ELT)(this).attr("TxBytes"), 10);
        er = parseInt($(ELT)(this).attr("Errors"), 10);
        dr = parseInt($(ELT)(this).attr("Drops"), 10);
        $(ELT)("#tbody_network").append("<tr><td>" + name + "</td><td class=\"right\">" + formatBytes(rx, xml) + "</td><td class=\"right\">" + formatBytes(tx, xml) + "</td><td class=\"right\">" + er.toString() + "/" + dr.toString() + "</td></tr>");
    });
}

/**
 * (re)fill the memory block with the values from the given xml
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshMemory(xml) {
    var html = "", tree = [], closed = [];
    
    $(ELT)("#memory").empty();
    html += "  <h2>" + genlang(27, false, '$_("memory usage")') + "</h2>\n";
    html += "  <table id=\"MemoryTree\" class=\"tablemain\" style=\"width:100%;\">\n";
    html += "   <thead>\n";
    html += "    <tr>\n";
    html += "     <th style=\"width:200px;\">" + genlang(34, true, '$_("type")') + "</th>\n";
    html += "     <th style=\"width:285px;\">" + genlang(33, true, '$_("usage")') + "</th>\n";
    html += "     <th class=\"right\" style=\"width:100px;\">" + genlang(35, true, '$_("free")') + "</th>\n";
    html += "     <th class=\"right\" style=\"width:100px;\">" + genlang(36, true, '$_("used")') + "</th>\n";
    html += "     <th class=\"right\" style=\"width:100px;\">" + genlang(37, true, '$_("size")') + "</th>\n";
    html += "    </tr>\n";
    html += "   </thead>\n";
    html += "   <tbody class=\"tree\">\n";
    
    $(ELT)("Memory", xml).each(function getMemory(id) {
        var free = 0, total = 0, used = 0, percent = 0, memoryindex = 0;
        free = parseInt($(ELT)(this).attr("Free"), 10);
        used = parseInt($(ELT)(this).attr("Used"), 10);
        total = parseInt($(ELT)(this).attr("Total"), 10);
        percent = parseInt($(ELT)(this).attr("Percent"), 10);
        html += "<tr><td style=\"width:200px;\">" + genlang(28, false, '$_("physical memory")') + "</td><td style=\"width:285px;\">" + createBar(percent) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(free, xml) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(used, xml) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(total, xml) + "</td></tr>";
        memoryindex = tree.push(0);
        
        $(ELT)("Memory Details", xml).each(function getMemorydetails(id) {
            var app = 0, appp = 0, buff = 0, buffp = 0, cached = 0, cachedp = 0;
            app = parseInt($(ELT)(this).attr("App"), 10);
            appp = parseInt($(ELT)(this).attr("AppPercent"), 10);
            buff = parseInt($(ELT)(this).attr("Buffers"), 10);
            buffp = parseInt($(ELT)(this).attr("BuffersPercent"), 10);
            cached = parseInt($(ELT)(this).attr("Cached"), 10);
            cachedp = parseInt($(ELT)(this).attr("CachedPercent"), 10);
            if (!isNaN(app)) {
                html += "<tr><td style=\"width:184px;\">" + genlang(64, false, '$_("kernel + applications")') + "</td><td style=\"width:285px;\">" + createBar(appp) + "</td><td class=\"right\" style=\"width:100px;\">&nbsp;</td><td class=\"right\" style=\"width:100px\">" + formatBytes(app, xml) + "</td><td class=\"right\" style=\"width:100px;\">&nbsp;</td></tr>";
                tree.push(memoryindex);
            }
            if (!isNaN(buff)) {
                html += "<tr><td style=\"width:184px;\">" + genlang(65, false, '$_("buffers")') + "</td><td style=\"width:285px\">" + createBar(buffp) + "</td><td class=\"rigth\" style=\"width:100px;\">&nbsp;</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(buff, xml) + "</td><td class=\"right\" style=\"width:100px;\">&nbsp;</td></tr>";
                tree.push(memoryindex);
            }
            if (!isNaN(cached)) {
                html += "<tr><td style=\"width:184px;\">" + genlang(66, false, '$_("cached")') + "</td><td style=\"width:285px;\">" + createBar(cachedp) + "</td><td class=\"right\" style=\"width:100px;\">&nbsp;</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(cached, xml) + "</td><td class=\"right\" style=\"width:100px;\">&nbsp;</td></tr>";
                tree.push(memoryindex);
            }
            if (!isNaN(app) || !isNaN(buff) || !isNaN(cached)) {
                closed.push(memoryindex);
            }
        });
    });
    $(ELT)("Memory Swap", xml).each(function getSwap(id) {
        var free = 0, total = 0, used = 0, percent = 0, swapindex = 0;
        free = parseInt($(ELT)(this).attr("Free"), 10);
        used = parseInt($(ELT)(this).attr("Used"), 10);
        total = parseInt($(ELT)(this).attr("Total"), 10);
        percent = parseInt($(ELT)(this).attr("Percent"), 10);
        html += "<tr><td style=\"width:200px;\">" + genlang(29, false, '$_("disk swap")') + "</td><td style=\"width:285px;\">" + createBar(percent) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(free, xml) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(used, xml) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(total, xml) + "</td></tr>";
        swapindex = tree.push(0);
        
        $(ELT)("Memory Swap Mount", xml).each(function getDevices(id) {
            var free = 0, total = 0, used = 0, percent = 0, mpoint = "", mpid = 0;
            closed.push(swapindex);
            free = parseInt($(ELT)(this).attr("Free"), 10);
            used = parseInt($(ELT)(this).attr("Used"), 10);
            total = parseInt($(ELT)(this).attr("Total"), 10);
            percent = parseInt($(ELT)(this).attr("Percent"), 10);
            mpid = parseInt($(ELT)(this).attr("MountPointID"), 10);
            mpoint = $(ELT)(this).attr("MountPoint");
            
            if (mpoint === undefined) {
                mpoint = mpid;
            }
            
            html += "<tr><td style=\"width:184px;\">" + mpoint + "</td><td style=\"width:285px;\">" + createBar(percent) + "</td><td class=\"right\" style=\"width:100px\">" + formatBytes(free, xml) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(used, xml) + "</td><td class=\"right\" style=\"width:100px;\">" + formatBytes(total, xml) + "</td></tr>";
            tree.push(swapindex);
        });
    });
    
    html += "   </tbody>\n";
    html += "  </table>\n";
    $(ELT)("#memory").append(html);
    
    $(ELT)("#MemoryTree").jqTreeTable(tree, {
        openImg: "/static/theme/redmond/images/gfx/treeTable/tv-collapsable.gif",
        shutImg: "/static/theme/redmond/images/gfx/treeTable/tv-expandable.gif",
        leafImg: "/static/theme/redmond/images/gfx/treeTable/tv-item.gif",
        lastOpenImg: "/static/theme/redmond/images/gfx/treeTable/tv-collapsable-last.gif",
        lastShutImg: "/static/theme/redmond/images/gfx/treeTable/tv-expandable-last.gif",
        lastLeafImg: "/static/theme/redmond/images/gfx/treeTable/tv-item-last.gif",
        vertLineImg: "/static/theme/redmond/images/gfx/treeTable/vertline.gif",
        blankImg: "/static/theme/redmond/images/gfx/treeTable/blank.gif",
        collapse: false,
        column: 0,
        striped: true,
        highlight: false,
        state: false
    });
    
}

/**
 * (re)fill the filesystems block with the values from the given xml<br><br>
 * appends the filesystems (each in a row) to the filesystem table in the tbody<br>before the rows are inserted the entire
 * tbody is cleared
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshFilesystems(xml) {
    var total_usage = 0, total_used = 0, total_free = 0, total_size = 0;
    
    filesystemTable.fnClearTable();
    
    $(ELT)("FileSystem Mount", xml).each(function getMount(mid) {
        var mpoint = "", mpid = 0, type = "", name = "", free = 0, used = 0, size = 0, percent = 0, options = "", inodes = 0, inodes_text = "", options_text = "";
        mpid = parseInt($(ELT)(this).attr("MountPointID"), 10);
        type = $(ELT)(this).attr("FSType");
        name = $(ELT)(this).attr("Name");
        free = parseInt($(ELT)(this).attr("Free"), 10);
        used = parseInt($(ELT)(this).attr("Used"), 10);
        size = parseInt($(ELT)(this).attr("Total"), 10);
        percent = parseInt($(ELT)(this).attr("Percent"), 10);
        options = $(ELT)(this).attr("MountOptions");
        inodes = parseInt($(ELT)(this).attr("Inodes"), 10);
        mpoint = $(ELT)(this).attr("MountPoint");
        
        if (mpoint === undefined) {
            mpoint = mpid;
        }
        if (options !== undefined) {
            options_text = "<br/><i>(" + options + ")</i>";
        }
        if (!isNaN(inodes)) {
            inodes_text = "<span style=\"font-style:italic\">&nbsp;(" + inodes.toString() + "%)</span>";
        }
        
        filesystemTable.fnAddData(["<span style=\"display:none;\">" + mpoint + "</span>" + mpoint, "<span style=\"display:none;\">" + type + "</span>" + type, "<span style=\"display:none;\">" + name + "</span>" + name + options_text, "<span style=\"display:none;\">" + percent.toString() + "</span>" + createBar(percent) + inodes_text, "<span style=\"display:none;\">" + free.toString() + "</span>" + formatBytes(free, xml), "<span style=\"display:none;\">" + used.toString() + "</span>" + formatBytes(used, xml), "<span style=\"display:none;\">" + size.toString() + "</span>" + formatBytes(size, xml)]);
        
        total_used += used;
        total_free += free;
        total_size += size;
        total_usage = round((total_used / total_size) * 100, 2);
    });
    
    $(ELT)("#s_fs_total").html(createBar(total_usage));
    $(ELT)("#s_fs_tfree").html(formatBytes(total_free, xml));
    $(ELT)("#s_fs_tused").html(formatBytes(total_used, xml));
    $(ELT)("#s_fs_tsize").html(formatBytes(total_size, xml));
}

/**
 * (re)fill the temperature block with the values from the given xml<br><br>
 * build the block content for the temperature block, this includes normal temperature information in the XML
 * and also the HDDTemp information, if there are no information the entire table will be removed
 * to avoid HTML warnings
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshTemp(xml) {
    var values = false;
    $(ELT)("#tempTable tbody").empty();
    $(ELT)("MBInfo Temperature Item", xml).each(function getTemperatures(id) {
        var label = "", value = "", limit = "";
        label = $(ELT)(this).attr("Label");
        value = $(ELT)(this).attr("Value").replace(/\+/g, "");
        limit = $(ELT)(this).attr("Max").replace(/\+/g, "");
        $(ELT)("#tempTable").append("<tr><td>" + label + "</td><td class=\"right\">" + formatTemp(value, xml) + "</td><td class=\"right\">" + formatTemp(limit, xml) + "</td></tr>");
        values = true;
    });
    if (values) {
        $(ELT)("#temp").show();
    }
    else {
        $(ELT)("#temp").remove();
    }
}

/**
 * (re)fill the voltage block with the values from the given xml<br><br>
 * build the voltage information into a separate block, if there is no voltage information available the
 * entire table will be removed to avoid HTML warnings
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshVoltage(xml) {
    var values = false;
    $(ELT)("#voltageTable tbody").empty();
    $(ELT)("MBInfo Voltage Item", xml).each(function getVoltages(id) {
        var label = "", value = 0, max = 0, min = 0;
        label = $(ELT)(this).attr("Label");
        value = parseFloat($(ELT)(this).attr("Value"), 10);
        max = parseFloat($(ELT)(this).attr("Max"), 10);
        min = parseFloat($(ELT)(this).attr("Min"), 10);
        $(ELT)("#voltageTable tbody").append("<tr><td>" + label + "</td><td class=\"right\">" + round(value, 2) + "&nbsp;" + genlang(62, true) + "</td><td class=\"right\">" + round(min, 2) + "&nbsp;" + genlang(62, true) + "</td><td class=\"right\">" + round(max, 2) + "&nbsp;" + genlang(62, true) + "</td></tr>");
        values = true;
    });
    if (values) {
        $(ELT)("#voltage").show();
    }
    else {
        $(ELT)("#voltage").remove();
    }
}

/**
 * (re)fill the fan block with the values from the given xml<br><br>
 * build the fan information into a separate block, if there is no fan information available the
 * entire table will be removed to avoid HTML warnings
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshFan(xml) {
    var values = false;
    $(ELT)("#fanTable tbody").empty();
    $(ELT)("MBInfo Fans Item", xml).each(function getFans(id) {
        var label = "", value = "", min = "";
        label = $(ELT)(this).attr("Label");
        value = $(ELT)(this).attr("Value");
        min = $(ELT)(this).attr("Min");
        $(ELT)("#fanTable tbody").append("<tr><td>" + label + "</td><td class=\"right\">" + value + "&nbsp;" + genlang(63, true) + "</td><td class=\"right\">" + min + "&nbsp;" + genlang(63, true) + "</td></tr>");
        values = true;
    });
    if (values) {
        $(ELT)("#fan").show();
    }
    else {
        $(ELT)("#fan").remove();
    }
}

/**
 * (re)fill the ups block with the values from the given xml<br><br>
 * build the ups information into a separate block, if there is no ups information available the
 * entire table will be removed to avoid HTML warnings
 * @param {jQuery} xml pythonSysInfo-XML
 */
function refreshUps(xml) {
    var html = "", tree = [], closed = [], index = 0, values = false;
    html += "        <h2><span id=\"lang_068\">UPS information</span></h2>\n";
    html += "        <table class=\"tablemain\" id=\"UPSTree\">\n";
    html += "          <tbody class=\"tree\">\n";
    
    $(ELT)("#ups").empty();
    $(ELT)("UPSInfo UPS", xml).each(function getUps(id) {
        var name = "", model = "", mode = "", start_time = "", upsstatus = "", temperature = "", outages_count = "", last_outage = "", last_outage_finish = "", line_voltage = "", load_percent = "", battery_voltage = "", battery_charge_percent = "", time_left_minutes = "";
        name = $(ELT)(this).attr("Name");
        model = $(ELT)(this).attr("Model");
        mode = $(ELT)(this).attr("Mode");
        start_time = $(ELT)(this).attr("StartTime");
        upsstatus = $(ELT)(this).attr("Status");
        
        temperature = $(ELT)(this).attr("Temperature");
        outages_count = $(ELT)(this).attr("OutagesCount");
        last_outage = $(ELT)(this).attr("LastOutage");
        last_outage_finish = $(ELT)(this).attr("LastOutageFinish");
        line_voltage = $(ELT)(this).attr("LineVoltage");
        load_percent = parseInt($(ELT)(this).attr("LoadPercent"), 10);
        battery_voltage = $(ELT)(this).attr("BatteryVoltage");
        battery_charge_percent = parseInt($(ELT)(this).attr("BatteryChargePercent"), 10);
        time_left_minutes = $(ELT)(this).attr("TimeLeftMinutes");
        
        html += "<tr><td colspan=\"2\"><strong>" + name + " (" + mode + ")</strong></td></tr>\n";
        index = tree.push(0);
        html += "<tr><td style=\"width:160px\">" + genlang(70, false) + "</td><td>" + model + "</td></tr>\n";
        tree.push(index);
        html += "<tr><td style=\"width:160px\">" + genlang(72, false) + "</td><td>" + start_time + "</td></tr>\n";
        tree.push(index);
        html += "<tr><td style=\"width:160px\">" + genlang(73, false) + "</td><td>" + upsstatus + "</td></tr>\n";
        tree.push(index);
        if (temperature !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(84, false) + "</td><td>" + temperature + "</td></tr>\n";
            tree.push(index);
        }
        if (outages_count !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(74, false) + "</td><td>" + outages_count + "</td></tr>\n";
            tree.push(index);
        }
        if (last_outage !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(75, false) + "</td><td>" + last_outage + "</td></tr>\n";
            tree.push(index);
        }
        if (last_outage_finish !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(76, false) + "</td><td>" + last_outage_finish + "</td></tr>\n";
            tree.push(index);
        }
        if (line_voltage !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(77, false) + "</td><td>" + line_voltage + "&nbsp;" + genlang(82, true) + "</td></tr>\n";
            tree.push(index);
        }
        if (!isNaN(load_percent)) {
            html += "<tr><td style=\"width:160px\">" + genlang(78, false) + "</td><td>" + createBar(load_percent) + "</td></tr>\n";
            tree.push(index);
        }
        if (battery_voltage !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(79, false) + "</td><td>" + battery_voltage + "&nbsp;" + genlang(82, true) + "</td></tr>\n";
            tree.push(index);
        }
        if (!isNaN(battery_charge_percent)) {
            html += "<tr><td style=\"width:160px\">" + genlang(80, false) + "</td><td>" + createBar(battery_charge_percent) + "</td></tr>\n";
            tree.push(index);
        }
        if (time_left_minutes !== undefined) {
            html += "<tr><td style=\"width:160px\">" + genlang(81, false) + "</td><td>" + time_left_minutes + "&nbsp;" + genlang(83, false) + "</td></tr>\n";
            tree.push(index);
        }
		values=true;
    });
    html += "          </tbody>\n";
    html += "        </table>\n";
    
    $(ELT)("#ups").append(html);
    
    if (values) {
        $(ELT)("#UPSTree").jqTreeTable(tree, {
            openImg: "/static/theme/redmond/images/gfx/treeTable/tv-collapsable.gif",
            shutImg: "/static/theme/redmond/images/gfx/treeTable/tv-expandable.gif",
            leafImg: "/static/theme/redmond/images/gfx/treeTable/tv-item.gif",
            lastOpenImg: "/static/theme/redmond/images/gfx/treeTable/tv-collapsable-last.gif",
            lastShutImg: "/static/theme/redmond/images/gfx/treeTable/tv-expandable-last.gif",
            lastLeafImg: "/static/theme/redmond/images/gfx/treeTable/tv-item-last.gif",
            vertLineImg: "/static/theme/redmond/images/gfx/treeTable/vertline.gif",
            blankImg: "/static/theme/redmond/images/gfx/treeTable/blank.gif",
            collapse: closed,
            column: 0,
            striped: true,
            highlight: false,
            state: false
        });
        $(ELT)("#ups").show();
    }
    else {
        $(ELT)("#ups").remove();
    }
}

/**
 * reload the page, this means all values are refreshed, except the plugins
 */
/*function reload() {
    $(ELT).ajax({
        url: '/clusternodeviewxml',
        dataType: 'xml',
        error: function error() {
            $(ELT).jGrowl("Error loading XML document!");
        },
        success: function buildblocks(xml) {
            refreshVitals(xml);
            refreshNetwork(xml);
            refreshHardware(xml);
            refreshMemory(xml);
            refreshFilesystems(xml);
            refreshVoltage(xml);
            refreshFan(xml);
            refreshTemp(xml);
            refreshUps(xml);
            
            $(ELT)('.stripeMe tr:nth-child(even)').addClass('even');
            langcounter = 1;
        }
    });
}
*/
/**
 * set a reload timer for the page
 * @param {jQuery} xml pythonSysInfo-XML
 */
function settimer(xml) {
    $(ELT)("Options", xml).each(function getRefreshTime(id) {
        var options, refresh = "";
        options = $(ELT)("Options", xml).get(id);
        refresh = $(ELT)(this).attr("refresh");
        if (refresh !== '0') {
            $(ELT).timer(refresh, reload);
        }
    });
}

cookie_language = readCookie("language");
cookie_template = readCookie("template");

if (cookie_template) {
    switchStyle(cookie_template);
}

/*$(ELT)(document).ready(function buildpage() {
    filesystemtable();
    
    $(ELT).ajax({
        url: '/clusternodeviewxml',
        dataType: 'xml',
        error: function error() {
            $(ELT).jGrowl("Error loading XML document!", {
                sticky: true
            });
        },
        success: function buildblocks(xml) {
            populateErrors(xml);
            
            refreshVitals(xml);
            refreshHardware(xml);
            refreshNetwork(xml);
            refreshMemory(xml);
            refreshFilesystems(xml);
            refreshTemp(xml);
            refreshVoltage(xml);
            refreshFan(xml);
            refreshUps(xml);
            
            changeLanguage();
            displayPage(xml);
            settimer(xml);
            
            $(ELT)('.stripeMe tr:nth-child(even)').addClass('even');
            langcounter = 1;
        }
    });
    
    $(ELT)("#errors").nyroModal();
    
    $(ELT)("#lang").change(function changeLang() {
        var language = "", i = 0;
        language = $(ELT)("#lang").val().toString();
        createCookie('language', language, 365);
        cookie_language = readCookie('language');
        changeLanguage();
        for (i = 0; i < plugin_liste.length; i += 1) {
            changeLanguage(plugin_liste[i]);
        }
        return false;
    });
    
    $(ELT)("#template").change(function changeTemplate() {
        switchStyle($(ELT)("#template").val().toString());
        return false;
    });
});
*/
jQuery.fn.dataTableExt.oSort['span-string-asc'] = function sortStringAsc(a, b) {
    var x = "", y = "";
    x = a.substring(a.indexOf(">") + 1, a.indexOf("</"));
    y = b.substring(b.indexOf(">") + 1, b.indexOf("</"));
    return ((x < y) ? -1 : ((x > y) ? 1 : 0));
};

jQuery.fn.dataTableExt.oSort['span-string-desc'] = function sortStringDesc(a, b) {
    var x = "", y = "";
    x = a.substring(a.indexOf(">") + 1, a.indexOf("</"));
    y = b.substring(b.indexOf(">") + 1, b.indexOf("</"));
    return ((x < y) ? 1 : ((x > y) ? -1 : 0));
};

jQuery.fn.dataTableExt.oSort['span-number-asc'] = function sortNumberAsc(a, b) {
    var x = 0, y = 0;
    x = parseInt(a.substring(a.indexOf(">") + 1, a.indexOf("</")), 10);
    y = parseInt(b.substring(b.indexOf(">") + 1, b.indexOf("</")), 10);
    return ((x < y) ? -1 : ((x > y) ? 1 : 0));
};

jQuery.fn.dataTableExt.oSort['span-number-desc'] = function sortNumberDesc(a, b) {
    var x = 0, y = 0;
    x = parseInt(a.substring(a.indexOf(">") + 1, a.indexOf("</")), 10);
    y = parseInt(b.substring(b.indexOf(">") + 1, b.indexOf("</")), 10);
    return ((x < y) ? 1 : ((x > y) ? -1 : 0));
};

/**
 * generate the block element for a specific plugin that is available
 * @param {String} plugin name of the plugin
 * @param {Number} translationid id of the translated headline in the plugin translation file
 * @param {Boolean} reload controls if a reload button should be appended to the headline
 * @return {String} HTML string which contains the full layout of the block
 */
function buildBlock(plugin, translationid, reload) {
    var block = "", reloadpic = "";
    if (reload) {
        reloadpic = "<img id=\"Reload_" + plugin + "Table\" src=\"/static/theme/redmond/images/gfx/reload.png\" alt=\"reload\" style=\"vertical-align:middle;border=0px;\" />&nbsp;";
    }
    block += "      <div id=\"Plugin_" + plugin + "\" style=\"display:none;float:left;margin:10px 0pt 0pt 10px;padding: 1px;\">\n";
    block += "        <h2>" + reloadpic + genlang(translationid, false, plugin) + "</h2>\n<span id=\"DateTime_" + plugin + "\" style=\"margin-left:10px;\"></span>";
    block += "      </div>\n";
    return block;
}

/**
 * translate a plugin and add this plugin to the internal plugin-list, this is only needed once and shouldn't be called more than once
 * @param {String} plugin name of the plugin  that should be translated
 */
function plugin_translate(plugin) {
    plugin_liste.push(plugin);
    changeLanguage(plugin);
}

/**
 * generate a formatted datetime string of the current datetime
 * @return {String} formatted datetime string
 */
function datetime() {
    var date, day = 0, month = 0, year = 0, hour = 0, minute = 0, days = "", months = "", years = "", hours = "", minutes = "";
    date = new Date();
    day = date.getDate();
    month = date.getMonth() + 1;
    year = date.getFullYear();
    hour = date.getHours();
    minute = date.getMinutes();
    
    // format values smaller that 10 with a leading 0
    days = (day < 10) ? "0" + day.toString() : day.toString();
    months = (month < 10) ? "0" + month.toString() : month.toString();
    years = (year < 1000) ? year.toString() : year.toString();
    minutes = (minute < 10) ? "0" + minute.toString() : minute.toString();
    hours = (hour < 10) ? "0" + hour.toString() : hour.toString();
    
    return days + "." + months + "." + years + "&nbsp;" + hours + ":" + minutes;
}

/**
 * insert dynamically a js script file into the website
 * @param {String} name name of the script that should be included
 */
function appendjs(name) {
    var scrptE, hdEl;
    scrptE = document.createElement("script");
    hdEl = document.getElementsByTagName("head")[0];
    scrptE.setAttribute("src", name);
    scrptE.setAttribute("type", "text/javascript");
    hdEl.appendChild(scrptE);
}

/**
 * insert dynamically a css file into the website
 * @param {String} name name of the css file that should be included
 */
function appendcss(name) {
    var scrptE, hdEl;
    scrptE = document.createElement("link");
    hdEl = document.getElementsByTagName("head")[0];
    scrptE.setAttribute("type", "text/css");
    scrptE.setAttribute("rel", "stylesheet");
    scrptE.setAttribute("href", name);
    hdEl.appendChild(scrptE);
}
