$def with (content)
String.prototype.Trim = function(){
    return this.replace(/(^\s*)|(\s*$(ELT))/g, "");
}

function isEmpty(val){
    if(val == 'undefined' || val.Trim() == ''){
        return true;
    }
    return false;
}

function fixID(id){
    return id.replace(/([!"#$(ELT)%&'()*+,./:;<=>?@[\]^`{|}~])/g,'\\\$1');
}

function custConfirm(title,message,buttons){
    var confdialog = $(ELT)('<div id="confdialog" title="'+title+'"><p>'+message+'</p></div>').appendTo('body');
    var defbuttons = {
        Cancel: function() {
			$(ELT)(this).dialog('close');
		},
		'$_("confirm")': function() {
		    $(ELT)(this).dialog('close');
		}
	};
    confdialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
    	buttons: buttons || defbuttons,
    	close: function(){
    		confdialog.remove();
    	}
    });
}

function custwarning(title,message,buttons){
    var warningdialog = $(ELT)('<div id="warningdialog" title="'+title+'"><p><span style="margin: 0pt 7px 20px 0pt; float: left;" class="ui-icon ui-icon-alert"/>' + message + '</p></div>').appendTo('body');
    var defbuttons = {
        Cancel: function() {
			$(ELT)(this).dialog('close');
		},
		'$_("confirm")': function() {
		    $(ELT)(this).dialog('close');
		}
	};
    warningdialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
    	buttons: buttons || defbuttons,
    	close: function(){
    		warningdialog.remove();
    	}
    });
}
function custWarning(title,message,cancelcallback,comfirmcallback,options){
    var warningdialog = $(ELT)('<div id="warningdialog" title="'+title+'"><p><span style="margin: 0pt 7px 20px 0pt; float: left;" class="ui-icon ui-icon-alert"/>' + message + '</p></div>').appendTo('body');
    warningdialog.dialog({
        bgiframe: true,
        autoOpen: true,
        resizable: false,
        modal: true,
        buttons: {
            '$_("cancel")': function() {
                                $(ELT)(this).dialog('close');
                if(cancelcallback != undefined){
                    cancelcallback();
                }
                        },
            '$_("confirm")': function() {
                                $(ELT)(this).dialog('close');
                if(comfirmcallback != undefined){
                    comfirmcallback();
                }
                        }
        },
        close: function(){
                warningdialog.remove();
                //if(cancelcallback != undefined){
                //    cancelcallback();
                //}
        }
    });
}

function custAlert(title,message,callback,options){
    var alertdialog = $(ELT)('<div id="alertdialog" title="'+title+'"><p>'+message+'</p></div>').appendTo('body');
    alertdialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
    	buttons: {
            '$_("confirm")': function() {
				$(ELT)(this).dialog('close');
                if(callback != undefined){
                    callback();
                }
			}
    	},
    	close: function(){
    		alertdialog.remove();
            if(callback != undefined){
                callback();
            }
    	}
    });
}
function custInfo(title,message,callback,options){
    var infodialog = $(ELT)('<div id="infodialog" title="'+title+'"><p>'+message+'</p></div>').appendTo('body');
    infodialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
        width: 'auto',
    	modal: true,
    	buttons: {
            '$_("confirm")': function() {
				$(ELT)(this).dialog('close');
                if(callback != undefined){
                    callback();
                }
			}
    	},
    	close: function(){
    		infodialog.remove();
            if(callback != undefined){
                callback();
            }
    	}
    });
}

function custLoading(message){
    var loaddialog = $(ELT)('<div id="loaddialog" title="" style="min-height:75px;"><p class="loadword" style="text-align:center">' + message + '</p><p style="background:url(/static/theme/redmond/images/gfx/ajaxLoader.gif);width:43px;height:43px; line-height:43px; font-size:12px; font-weight:bold; margin:auto; padding:0" id="set_second">0</p></div>').appendTo('body');
    var flag = 0;
    loaddialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
        height: 130,
        width: 300,
        open: function(){
            $(ELT)(this).parent().find('.ui-dialog-titlebar').hide();
            //flag = setInterval(getprocessing, 100);
            //getprocessing();
			flag = setInterval(set_second, 1000);
        },
        beforeclose: function(){
			clearInterval(flag);
        },
    	close: function(){
    		$(ELT)(this).remove();
    	}
    });
}
function custPopup(title, message,callback){
	//var popupdialog = $(ELT)('<div class="popup"></div>');
	var old_popup = $(ELT)('.popup');
	if (old_popup){ old_popup.remove();}
	var popupdialog = $(ELT)('<div class="popup" title="'+title+'"><p>'+message+'</p></div>').appendTo('body');
    popupdialog.dialog({
    	dialogClass: 'myPopup',
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
        width: 300,
        height: 80,
        position: { my: "right bottom", at: "right bottom", of: window },
    	//modal: true,
    	close: function(){
    		popupdialog.remove();
            if(callback != undefined){
                callback();
            }
    	}
    });
}

function set_second(){
	var set_second=parseInt($(ELT)('#set_second').html());
	set_second++;
	$(ELT)('#set_second').html(set_second);
}

function getSize(size){
    var tempsize = [];
    tempsize.push(size);
    return tempsize;
}

(function($(ELT)){                                           //ip check
        $(ELT).fn.ipaddress = function(params) {
                return $(ELT)(this).each(function(){
                        var $(ELT)this = $(ELT)(this);
                        var options = $(ELT).extend({
                                cidr : false
                        }, params);

                        if (!$(ELT)this.hasClass('ip-enabled')) {
                                $(ELT)this.hide();

                                var ip_split = (this.value) ? this.value.split('/') : ['...','32'];

                                var ip_cidr = ip_split[1] || '32';

                                var ip_value = ip_split[0].split('.');

                                var octets = [];
                                var id_prefix = $(ELT)this.attr('name').replace(/\[/g, '_').replace(/\]/g, '');
                                for (var i = 0; i <=3; i++) {
                                        octets.push('<input type="text" class="ip_octet ' + id_prefix + '" id="' + id_prefix + '_octet_' + (i+1) + '" maxlength="3" value="' + ip_value[i] + '" />');
                                }
                                var octet_html = octets.join('.');
                                if (options.cidr) {
                                        octet_html += '/<input type="text" class="ip_cidr ip_octet" id="' + id_prefix + '_octet_cidr" maxlength="2" value="' + ip_cidr + '" />';
                                }

                                $(ELT)this.after($(ELT)('<div class="ip_container ui-corner-all" style="display: inline' + (($(ELT).browser.msie) ? '' : '-block') + ';"/>').html(octet_html));
                                $(ELT)this.addClass('ip-enabled');
                        }

                        var isNumeric = function(e){
                                if (e.shiftKey) return false;
                                return (e.keyCode >= 48 && e.keyCode <= 57) || (e.keyCode >= 96 && e.keyCode <= 105);
                        };

                        var isValidKey = function(e){
                                var valid = [
                                        8,        // backspace
                                        9,        // tab
                                        13,       // enter
                                        27,       // escape
                                        35,       // end
                                        36,       // home
                                        37,       // left arrow
                                        39,       // right arrow
                                        46,       // delete
                                        48, 96,   // 0
                                        49, 97,   // 1
                                        50, 98,   // 2
                                        51, 99,   // 3
                                        52, 100,  // 4
                                        53, 101,  // 5
                                        54, 102,  // 6
                                        55, 103,  // 7
                                        56, 104,  // 8
                                        57, 105,  // 9
                                        110, 190  // period
                                ];

                                if (options.cidr) {
                                        valid.push(111, 191); // slash
                                }

                                // only allow shift key with tab
                                if (e.shiftKey && e.keyCode != 9) return false;

                                for (var i = 0, c; c = valid[i]; i++) {
                                        if (e.keyCode == c) return true;
                                }

                                return false;
                        };

                        var saveIP = function(el) {
                                // save value to original input if all octets have been entered
                                if ($(ELT)('input.ip_octet:not(.ip_cidr)', $(ELT)(el).parent()).filter(function(){ return this.value.length; }).length == 4) {
                                        var ip_value = [], ip = '', cidr = '';
                                        //alert($(ELT)(el).attr('id'));
                                        $(ELT)('input.ip_octet:not(.ip_cidr)', $(ELT)(el).parent()).each(function(){
                                                ip_value.push(this.value);
                                        });
                                        ip = ip_value.join('.');

                                        if (options.cidr) {
                                                var $(ELT)cidr = $(ELT)('input.ip_cidr', $(ELT)(el).parent());
                                                cidr = ($(ELT)cidr.length) ? '/' + $(ELT)cidr.val() : '/32';
                                        }

                                        $(ELT)this.val(ip + cidr);
                                }
                                else {
                                        $(ELT)this.val('');
                                }
                        }

                        $(ELT)('input.ip_octet.' + $(ELT)this.attr('id')).bind('keydown', function(e){
                                if (!isValidKey(e)) return false;

                                var next_octet = $(ELT)(this).next('input.ip_octet.' + $(ELT)this.attr('id'));
                                var prev_octet = $(ELT)(this).prev('input.ip_octet' + $(ELT)this.attr('id'));

                                // jump to next octet on period if this octet has a value
                                if (e.keyCode == 110 || e.keyCode == 190) {
                                        if (this.value.length) {
                                                if (next_octet.length) {
                                                        next_octet.focus();
                                                        next_octet.select();
                                                }
                                        }
                                        return false;
                                }

                                // set empty octets to zero and jump to cidr input on slash
                                if ((e.keyCode == 111 || e.keyCode == 191) && options.cidr) {
                                        $(ELT)('input.ip_octet:not(.ip_cidr)', $(ELT)(this).parent()).filter(function(){ return !this.value.length; }).val('0');
                                        $(ELT)('input.ip_cidr', $(ELT)(this).parent()).focus();
                                        return false;
                                }
                                if (($(ELT)(this).caret()[1] - $(ELT)(this).caret()[0]) && isNumeric(e)) {
                                        return true;
                                }

                                // jump to next octet if maxlength is reached and number key or right arrow is pressed
                                if ((this.value.length == this.getAttribute('maxlength') && $(ELT)(this).caret()[0] == this.getAttribute('maxlength') && (isNumeric(e) || e.keyCode == 39)) || (e.keyCode == 39 && $(ELT)(this).caret()[0] == this.value.length)) {
                                        if (next_octet.length) {
                                                $(ELT)(this).trigger('blur');
                                                next_octet.focus().caret(0);
                                                return true;
                                        }
                                }

                                // jump to previous octet if left arrow is pressed and caret is at the 0 position
                                if (e.keyCode == 37 && $(ELT)(this).caret()[0] == 0) {
                                        if (prev_octet.length) {
                                                $(ELT)(this).trigger('blur');
                                                prev_octet.caret(prev_octet.val().length);
                                                return false;
                                        }
                                }

                                // jump to previous octet on backspace
                                if (e.keyCode == 8 && $(ELT)(this).caret()[0] == 0 && $(ELT)(this).caret()[0] == $(ELT)(this).caret()[1]) {
                                        if (prev_octet.length) {
                                                $(ELT)(this).trigger('blur');
                                                prev_octet.focus().caret(prev_octet.val().length);
                                                return false;
                                        }
                                }
                }).bind('keyup', function(e){
                                if (this.value > 255) this.value = 255;
                                saveIP(this);
                        });

                        $(ELT)('input.ip_cidr').bind('keyup', function(e){
                                if (this.value > 32) this.value = 32;

                                saveIP(this);
                        });

                });

        };
})(jQuery);
function accMul(arg1,arg2){
    var m=0,s1=arg1.toString(),s2=arg2.toString();
    try{
        m+=s1.split(".")[1].length;
    }
    catch(e){}
    try{
        m+=s2.split(".")[1].length;
    }
    catch(e){}
    return Number(s1.replace(".",""))*Number(s2.replace(".",""))/Math.pow(10,m)
}
Number.prototype.mul = function (arg){
    return accMul(arg, this);
}
function accDiv(arg1,arg2){
    var t1=0,t2=0,r1,r2;
    try{
        t1=arg1.toString().split(".")[1].length;
    }
    catch(e){}
    try{
        t2=arg2.toString().split(".")[1].length;
    }
    catch(e){}
    with(Math){
        r1=Number(arg1.toString().replace(".",""))
        r2=Number(arg2.toString().replace(".",""))
    return (r1/r2)*pow(10,t2-t1);
    }
}
Number.prototype.div = function (arg){
    return accDiv(this, arg);
}
Number.prototype.fixed=function(n){ 
with(Math)return   round(Number(this)*pow(10,n))/pow(10,n) 
} 
//create cookie
function getCookie(p_value){
    if (document.cookie.length>0){
        c_start=document.cookie.indexOf(p_value + "=");
        if (c_start!=-1){
            c_start=c_start + p_value.length+1;
            c_end=document.cookie.indexOf(";",c_start);
            if (c_end==-1) c_end=document.cookie.length;
            return unescape(document.cookie.substring(c_start,c_end));
        }
    }
    return "";
}

function setCookie(p_value,value,expiretime){
    var exdate=new Date();
    exdate.setDate(exdate.getTime()+expiretime);
    document.cookie=p_value+ "=" +escape(value)+((expiretime==null) ? "" : ";expires="+exdate.toUTCString());
}                                

jQuery.validator.addMethod("inputName", function(value, element) {
    return this.optional(element) || /^[0-9a-zA-Z@_.-]+$(ELT)/.test(value);
}, "<img src='/static/theme/redmond/images/iperror.png'>");

jQuery.validator.addMethod("password", function(value, element) {
    return this.optional(element) || /^[0-9a-zA-Z_]+$(ELT)/.test(value);
}, "<img src='/static/theme/redmond/images/iperror.png'>");
    
jQuery.validator.addMethod("byterangelength", function(value, element, param) {
    var bytelength = slength = value.length;
    for(var i = 0;i < slength; i++){
        if(value.charCodeAt(i) > 127){
            bytelength++;    
        }    
    }
    return this.optional(element) || ( bytelength >= param[0] && bytelength <= param[1] );
}, "<img src='/static/theme/redmond/images/iperror.png'>");
Array.prototype.clean = function(deleteValue){
    for (var i = 0;i< this.length; i++){
        if (this[i] == deleteValue){
            this.splice(i,1);
            i--;
        }    
    }  
    return this;
}
function URLencode(sStr){
	return escape(sStr).replace(/\+/g, '%2B').replace(/\"/g,'%22').replace(/\'/g, '%27').replace(/\//g,'%2F');
}
//srcoll ui
/*function next(elem){
do{
elem=elem.nextSibling;
}while(elem&&elem.nodeType!=1);
return elem;
}
function first(elem){
	elem=elem.firstChild;
	return elem && elem.nodeType!=1?next(elem):elem;
}
var Marquee = {
	init : function(id,toFollow,speed){
		this.speed = speed || 10;
		this.boxID = id;
		this.toFollow=toFollow;
		this.scrollBox = document.getElementById(id);
		if(this.toFollow=="top"||this.toFollow=="bottom"){
			this.appendBox=first(this.scrollBox).cloneNode(true);
			this.scrollBox.appendChild(this.appendBox);
		}else{
			var templateLeft = "<table cellspacing='0' cellpadding='0' style='border-collapse:collapse;display:inline;'><tr><td noWrap=true style='white-space: nowrap;word-break:keep-all;'>"+this.scrollBox.innerHTML+"</td><td noWrap=true style='white-space: nowrap;word-break:keep-all;'>"+this.scrollBox.innerHTML+"</td></tr></table>";
			this.scrollBox.innerHTML=templateLeft;
			this.appendBox=first(first(first(first(this.scrollBox))));
		}
		this.objs = {
			scrollBox : this.scrollBox,
			appendBox : this.appendBox,
			toFollow : this.toFollow,
			speed : this.speed,
			id : this.boxID
		};
		return this;
		},
	scrollUp : function(){
		var self = this.objs;
		self.begin = function(){
			if(self['toFollow']=="top"){
				self.doScr = setInterval(function(){
					if(self['appendBox'].offsetHeight<=self['scrollBox'].scrollTop){
						self['scrollBox'].scrollTop-=first(self['scrollBox']).offsetHeight;
					}else{
						self['scrollBox'].scrollTop++;
					}
				},self.speed);
			}else if(self['toFollow']=="bottom"){
				self.doScr = setInterval(function(){
					if(self['scrollBox'].scrollTop<=0){
						self['scrollBox'].scrollTop=self['appendBox'].offsetHeight;
					}else{
						self['scrollBox'].scrollTop--;
					}
				},self.speed);
			}else if(self['toFollow']=="left"){
				self.doScr = setInterval(function(){
					if(self['appendBox'].offsetWidth<=self['scrollBox'].scrollLeft){
						self['scrollBox'].scrollLeft-=self['appendBox'].offsetWidth;
					}else{
						self['scrollBox'].scrollLeft++;
					}
				},self.speed);
			}else if(self['toFollow']=="right"){
				self.doScr = setInterval(function(){
					if(self['scrollBox'].scrollLeft<=0){
						self['scrollBox'].scrollLeft=self['appendBox'].offsetWidth;
					}else{
						self['scrollBox'].scrollLeft--;
					}
				},self.speed);
			}			
		}
		self.begin();
		self.scrollBox.onmouseover = function(){
			clearInterval(self.doScr);
		}
		self.scrollBox.onmouseout = function(){
			self.begin();
		}
	}
} 
*/
function correctPNG() // correctly handle PNG transparency in Win IE 5.5 & 6.
{
    var arVersion = navigator.appVersion.split("MSIE")
    var version = parseFloat(arVersion[1])
    if ((version >= 5.5) && (document.body.filters))
    {
       for(var j=0; j<document.images.length; j++)
       {
          var img = document.images[j]
          var imgName = img.src.toUpperCase()
          if (imgName.substring(imgName.length-3, imgName.length) == "PNG")
          {
             var imgID = (img.id) ? "id='" + img.id + "' " : ""
             var imgClass = (img.className) ? "class='" + img.className + "' " : ""
             var imgTitle = (img.title) ? "title='" + img.title + "' " : "title='" + img.alt + "' "
             var imgStyle = "display:inline-block;" + img.style.cssText
             if (img.align == "left") imgStyle = "float:left;" + imgStyle
             if (img.align == "right") imgStyle = "float:right;" + imgStyle
             if (img.parentElement.href) imgStyle = "cursor:hand;" + imgStyle
             var strNewHTML = "<span " + imgID + imgClass + imgTitle
             + " style=\"" + "width:" + img.width + "px; height:" + img.height + "px;" + imgStyle + ";"
             + "filter:progid:DXImageTransform.Microsoft.AlphaImageLoader"
             + "(src=\'" + img.src + "\', sizingMethod='scale');\"></span>"
             img.outerHTML = strNewHTML
             j = j-1
          }
       }
    }
}
function checkSession(){
    url = '/checksession?' + new Date().getTime();
    $(ELT).ajax({
        url: url,
        dataType: 'text',
        type: 'POST',
        success: function(text){
            if(!isNaN(text)){
                return
            }else{
                location.host.replace(':81','');
                custAlert("$_('operwarning')","$_('session expired')", function(){ location.replace('https://' + location.host + '/login?redirect_url=' + location.pathname);});
            }
        },
        error: function(){
        	location.host.replace(':81','');
            custAlert("$_('operwarning')","$_('session expired')", function(){ location.replace('https://' + location.host + '/login?redirect_url=' + location.pathname);});
        }
    });
}
    function getprocessing(){
        var process_url = '/clustergetprocessing?' + new Date().getTime();
        if(getCookie('comet_flag_2') == '1'){
            return false;
        }
        $(ELT).ajax({
            url: process_url,
            type: 'POST',
            dateType: 'text',
            beforeSend: function(xhr){
                setCookie('comet_flag_2', '1', 1);
            $(ELT)(window).bind('beforeunload', function() {
                setCookie('comet_flag_2', '0', 1);
                xhr.abort();
                });
            },
            timeout: 140000,
            success: function(text){
                setCookie('comet_flag_2', '0', 1);
                if($(ELT)('#loaddialog')){
                    $(ELT)('#loaddialog p.loadword').html(text);
                }
                else{
                    custLoading(text);
                }
            },
            error: function(XMLHttpRequest,textStatus,errorThrown){
            setCookie('comet_flag_2', '0', 1);
            },
            complete: function(){
                if($(ELT)('#loaddialog').length > 0){
                    getprocessing();
                }
            }
        });
    }
    
    function getdiskinfo(nodename,diskname, obj){
    	url = "/indexgetdisk?" + new Date().getTime();
    	$(ELT).ajax({
    		url: url,
    		type: 'POST',
    		data: 'clusternodename=' + nodename, 
    		dataType: 'JSON',
    		beforeSend: function(){},
    		success: function(text){
    			var diskdata = eval(text);
    			var adisk = '';
    			var nodisk = '<td class="disk"><span class="nodisk devname">Null</span><span class="graylight"></td>';
    			var nulldisk = '<td class="disk"><span class="nulldisk devname"></span><span></td>';
    			//var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-8" href="/clusternodeview?node=' + diskdata[0].nodename + '"><span class="mleft8"></span><span class="mright8"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-16" href="/clusternodeview?node=' + diskdata[0].nodename + '"><span class="mleft16"></span><span class="mright16"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			//var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-16"><span class="mleft16"></span><span class="mright16"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			//var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-24"><span class="mleft24"></span><span class="mright24"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			var jobd = 6;
    			switch(jobd){
    			case 16:
    			{
    				var diskline = [['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk]];
	    			for(var i = 0; i < diskdata.length; i++){
	    				var position = parseInt(diskdata[i].position);
	    				var devname = diskdata[i].devname;
	    				var devid =  diskdata[i].devname;
	    				var disklight = diskdata[i].light_status;
	    				if(diskdata[i].status == "in_raid"){
	    					var diskclass = diskdata[i].raidname;
	    					adisk = '<td id="' + devid + '" class="disk ' + diskclass +'"><span class="devname">' + devname + '</span><span class="'+disklight+'"></td>';
	    				}
	    				else{
	    					adisk = '<td id="' + devid + '" class="disk"><span class="devname">' + devname + '</span><span class="'+disklight+'"></td>';
	    				}
                        if(position > 0){
                            var colume = position%4;
                        	var row = parseInt(position/4);
                            if(colume == 0){
                                row -= 1;
                                colume = 4;
                           	}       
                            diskline[row][colume] = adisk;
                        }
					}
                    var dline = '';
                    for(var i = 3;i >=0 ;i--){
                            dline = diskline[i].join('') + "</tr>";
                            nodepanel +=dline;
                    }
	    		}
	    		break;
	    		case 6:
	    		{
	    			var diskline = [['<tr class="hdisk">',nodisk,nodisk,nulldisk,nulldisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nulldisk,nulldisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nulldisk,nulldisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nulldisk,nulldisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk], 
    								['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk]];
	    			nodisk = nulldisk;
	    			for(var i = 0; i < diskdata.length; i++){
	    				var position = parseInt(diskdata[i].position);
	    				var devname = diskdata[i].devname;
	    				var devid =  diskdata[i].devname;
	    				var disklight = diskdata[i].light_status;
	    				if(diskdata[i].status == "in_raid"){
	    					var diskclass = diskdata[i].raidname;
	    					adisk = '<td id="' + devid + '" class="disk ' + diskclass +'"><span class="devname">' + devname + '</span><span class="'+disklight+'"></td>';
	    				}
	    				else{
	    					adisk = '<td id="' + devid + '" class="disk"><span class="devname">' + devname + '</span><span class="'+disklight+'"></td>';
	    				}
	    				if(position > 0){
                            var colume = position%2;
                        	var row = parseInt(position/2);
                            if(colume == 0){
                                row -= 1;
                                colume = 2;
                            }
                            diskline[row][colume] = adisk;
                        }    
					}
                    var dline = '';
                    for(var i = 2;i >=0 ;i--){
                            dline = diskline[i].join('') + "</tr>";
                            nodepanel +=dline;
                    }
	    		}
	    		break;
	    		default:
	    		{
	    			//diskline[row][colume] = nodisk;
	    		}
	    		}
				//nodepanel += disklinehtml6;
				//nodepanel += disklinehtml5;
				//nodepanel += disklinehtml4;
				//nodepanel += disklinehtml3;
				//nodepanel += disklinehtml2;
				//nodepanel += disklinehtml1;
				nodepanel += "</table>";
				//$(ELT)("#upspanel").before(nodepanel);
				nodepanel='<div id="mgroup"><div id="mtop"></div>'+nodepanel+'</div><div id="mbottom"></div></div>';
				if($(ELT)(obj).find('#mgroup').length > 0){
					if(diskname != undefined){
						$(ELT)("#"+nodename+"_"+diskname).addClass('highlight')
					}
					return false;
				}else{
					if(obj != undefined){
						$(ELT)(obj).append(nodepanel);
					}else{
						return nodepanel;
					}
				}
				if(diskname != undefined){
					$(ELT)("#"+nodename+"_"+diskname).addClass('highlight')
				}
				$(ELT)(".disk").each(
			    	function(){
			    		/*$(ELT)(this).bind('mouseover', function(){
			    			$(ELT)(this).addClass('highlight');});
			    		$(ELT)(this).bind('mouseout', function(){
			    			$(ELT)(this).removeClass('highlight');});*/
			    		$(ELT)(this).click(function(){
			    			$(ELT)(this).toggleClass('highlight');
			    		});
			    });
			    /*$(ELT)("#" + nodename).bind('mouseenter', function(){
			    	$(ELT)("#machineinfo").html("");
			    	$(ELT)("#machineinfo").show(getraidinfo(nodename));
			    	$(ELT)("#" + nodename).append($(ELT)("#machineinfo"));
			    	var nodetop = parseInt($(ELT)("#" + nodename).position().top);
			    	$(ELT)("#machineinfo").addClass("machineinfo");
			    	$(ELT)("#machineinfo").attr("style","top:" + nodetop + "px;");
			    	$(ELT)("#machineinfo").addClass('ui-corner-all');
			    });
			    $(ELT)("#" + nodename).bind('click', function(){
    				window.location.href=$(ELT)(this).attr('href').toString();
				});
			    $(ELT)("#" + nodename).bind('mouseleave', function(){
			    	$(ELT)("#machineinfo").hide();
			    });*/
    		}
    	});
    }
    function getUrl(treeNode){
    	param = "expandarray=" + escape(getCookie('expandstr'));
    	return "/clusterserviceviewload?" + param;
    }

    function onMouseDown(event, treeId, treeNode, clickFlag) {
        var aObj = $(ELT)("#" + treeNode.tId);
        aObj.addClass('light');
    }
    function onMouseUp(event, treeId, treeNode, clickFlag) {
        var aObj = $(ELT)("#" + treeNode.tId);
        aObj.removeClass('light');
    }

    function addDiyDom(treeId, treeNode) {
        var aObj = $(ELT)("#" + treeNode.tId);
        //alert(treeId);
        var editStr = '';
        aObj.prepend(editStr);
    }
    function addHoverDom(treeId, treeNode) {
        if (treeNode.parentNode && treeNode.parentNode.id!=1) return;
        var aObj = $(ELT)("#" + treeNode.tId);
        aObj.addClass('light');
    }
    function removeHoverDom(treeId, treeNode) {
        if (treeNode.parentNode && treeNode.parentNode.id!=1) return;
        var aObj = $(ELT)("#" + treeNode.tId);
        aObj.removeClass('light');
    }

    function beforeExpand(treeId, treeNode) {
        if (!treeNode.isAjaxing) {
            tmpexpand = getCookie('expandstr');
        	if (tmpexpand.indexOf('|'+treeNode.PATH + '|') < 0){
            	tmpexpand += '|' + treeNode.PATH;
        	}
            ajaxGetNodes(treeNode, "refresh");
            return true;
        } else {
            return false;
        }
    }
    function zTreeBeforeCollapse(treeId, treeNode) {
        if (!treeNode.isAjaxing) {
            treeNode.times = 1;
            tmpexpand = getCookie('expandstr');
            tmpexpandarray = tmpexpand.split('|');
            for(var i = 0; i<tmpexpandarray.length;i++){
                if (tmpexpandarray[i] && tmpexpandarray[i].indexOf(treeNode.PATH) >= 0){
                    tmpexpandarray[i] = '';
                }
            }
            tmpexpand = tmpexpandarray.clean("").join('|');
            return true;
        } else {
            return false;
        }
    }
    function onAsyncSuccess(event, treeId, treeNode, msg) {
        var zTree = $(ELT).fn.zTree.getZTreeObj("treeDemo");
        treeNode.icon = "";
        zTree.updateNode(treeNode);
        tmpexpand = getCookie('expandstr');
        if (tmpexpand.indexOf('|'+treeNode.PATH + '|') < 0){
            tmpexpand += '|' + treeNode.PATH;
            setCookie('expandstr',tmpexpand + '|', 3600*1000);
        }
        //zTree.expandNode(treeNode, true, true);
        refreshPage();
        zTree.selectNode(treeNode.childs[0]);
    }
    function onAsyncError(event, treeId, treeNode, XMLHttpRequest, textStatus, errorThrown) {
        var zTree = $(ELT).fn.zTree.getZTreeObj("treeDemo");
        treeNode.icon = "";
        zTree.updateNode(treeNode);
    }
    function ajaxGetNodes(treeNode, reloadType) {
        var zTree = $(ELT).fn.zTree.getZTreeObj("treeDemo");
        if (reloadType == "refresh") {
            addDiyDom(treeNode, reloadType);
            zTree.updateNode(treeNode);
        }
        //refreshPage();
        zTree.reAsyncChildNodes(treeNode, reloadType, true);
    }
    function init_navleft(){
		var setting = {
	    	data: {
				key: {
					title: "fullName"
				}
			},
	        async: {
	            enable: true,
	            url: "/clusterserviceviewload" 
	        },
	        view: {
	        	showTitle: true,
	            expandSpeed: "normal",
	            selectedMulti: false,
	            showLine: false,
	            addDiyDom: addDiyDom,
	            addHoverDom: addHoverDom,
	            removeHoverDom: removeHoverDom 
	        },
	        callback: {
	            beforeExpand: beforeExpand,
	            beforeCollapse: zTreeBeforeCollapse,
	            onAsyncSuccess: onAsyncSuccess,
	            onAsyncError: onAsyncError,
	            onMouseDown: onMouseDown,
	            onMouseUp: onMouseUp
	        }
	    };
	    var zNodes = [{'name':'$_("clustergroup")',isParent:"true", 'fullName':'$_("clustergroup")'},{'name':'$_("clusterservice")',isParent:"true", 'fullName':'$_("clusterservice")'}];
    	$(ELT).fn.zTree.init($(ELT)("#treeDemo"), setting, zNodes);
    	//checkSession();
        var url = '/clusterinitnavleft?' + new Date().getTime();
        //var data = 'expandarray=' + escape(getCookie('expandstr'));
        $(ELT).ajax({
            url:url,
            type: 'POST',
            //data: data,
            dataType: 'JSON',
            beforeSend: function(){
            	//fresh_flag = 1;
            },
            //error:,
            success: function(text){
        		zNodes = eval(text);
        		alter(zNodes);
        		var treeObj = $(ELT).fn.zTree.getZTreeObj("treeDemo");
        		addDiyDom(zNodes,'refresh');
            }
        });
    }
     
 
Date.prototype.Format = function(fmt) 
{ //author: meizz 
	var o = { 
    	"M+" : this.getMonth()+1,                  
    	"d+" : this.getDate(),                    
    	"h+" : this.getHours(),                    
    	"m+" : this.getMinutes(),                  
    	"s+" : this.getSeconds(),                 
    	"q+" : Math.floor((this.getMonth()+3)/3), 
    	"S"  : this.getMilliseconds()             
  	}; 
  	if(/(y+)/.test(fmt)) 
    	fmt=fmt.replace(RegExp.$(ELT)1, (this.getFullYear()+"").substr(4 - RegExp.$(ELT)1.length)); 
  	for(var k in o) 
    	if(new RegExp("("+ k +")").test(fmt)) 
  	fmt = fmt.replace(RegExp.$(ELT)1, (RegExp.$(ELT)1.length==1) ? (o[k]) : (("00"+ o[k]).substr((""+ o[k]).length))); 
  	return fmt; 
}
