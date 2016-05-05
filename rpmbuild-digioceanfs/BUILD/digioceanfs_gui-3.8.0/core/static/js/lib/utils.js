String.prototype.Trim = function(){
    return this.replace(/(^\s*)|(\s*$)/g, "");
}

function isEmpty(val){
    if(val == 'undefined' || val.Trim() == ''){
        return true;
    }
    return false;
}

function custConfirm(title,message,buttons){
    var confdialog = $('<div id="confdialog" title="'+title+'"><p>'+message+'</p></div>').appendTo('body');
    var defbuttons = {
        Cancel: function() {
			$(this).dialog('close');
		},
		'$_("OK")': function() {
		    $(this).dialog('close');
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
    alert(11111111);
    var warningdialog = $('<div id="warningdialog" title="'+title+'"><p><span style="margin: 0pt 7px 20px 0pt; float: left;" class="ui-icon ui-icon-alert"/>' + message + '</p></div>').appendTo('body');
    var defbuttons = {
        Cancel: function() {
			$(this).dialog('close');
		},
		'OK': function() {
		    $(this).dialog('close');
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
    alert(11111111);
    var warningdialog = $('<div id="warningdialog" title="'+title+'"><p><span style="margin: 0pt 7px 20px 0pt; float: left;" class="ui-icon ui-icon-alert"/>' + message + '</p></div>').appendTo('body');
    warningdialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
    	buttons: {
            '$_("cancel")': function() {
				$(this).dialog('close');
                if(cancelcallback != undefined){
                    cancelcallback();
                }
			},
            '$_("OK")': function() {
				$(this).dialog('close');
                if(comfirmcallback != undefined){
                    comfirmcallback();
                }
			}
        },
    	close: function(){
    		warningdialog.remove();
    	}
    });
}
function custAlert(title,message,options){
    var alertdialog = $('<div id="alertdialog" title="'+title+'"><p>'+message+'</p></div>').appendTo('body');
    alertdialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
    	buttons: {
            '$_("OK")': function() {
				$(this).dialog('close');
			}
    	},
    	close: function(){
    		alertdialog.remove();
    	}
    });
}

function custLoading(message){
    var loaddialog = $('<div id="loaddialog" title="" style="min-height:75px;"><p class="loadword" style="text-align:center">' + message + '</p><p>' + '<img src="/static/theme/redmond/images/ajax-loader.gif" /></p></div>').appendTo('body');
    loaddialog.dialog({
    	bgiframe: true,
    	autoOpen: true,
        resizable: false,
    	modal: true,
        height: 105,
        width: 300,
    	close: function(){
    		loaddialog.remove();
    	}
    });
    $('.ui-dialog-titlebar').hide();
}

function getSize(size){
    var tempsize = [];
    tempsize.push(size);
    return tempsize;
}

(function($){                                           //ip check
        $.fn.ipaddress = function(params) {
                return $(this).each(function(){
                        var $this = $(this);
                        var options = $.extend({
                                cidr : false
                        }, params);

                        if (!$this.hasClass('ip-enabled')) {
                                $this.hide();

                                var ip_split = (this.value) ? this.value.split('/') : ['...','32'];

                                var ip_cidr = ip_split[1] || '32';

                                var ip_value = ip_split[0].split('.');

                                var octets = [];
                                var id_prefix = $this.attr('name').replace(/\[/g, '_').replace(/\]/g, '');
                                for (var i = 0; i <=3; i++) {
                                        octets.push('<input type="text" class="ip_octet ' + id_prefix + '" id="' + id_prefix + '_octet_' + (i+1) + '" maxlength="3" value="' + ip_value[i] + '" />');
                                }
                                var octet_html = octets.join('.');
                                if (options.cidr) {
                                        octet_html += '/<input type="text" class="ip_cidr ip_octet" id="' + id_prefix + '_octet_cidr" maxlength="2" value="' + ip_cidr + '" />';
                                }

                                $this.after($('<div class="ip_container ui-corner-all" style="display: inline' + (($.browser.msie) ? '' : '-block') + ';"/>').html(octet_html));
                                $this.addClass('ip-enabled');
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
                                if ($('input.ip_octet:not(.ip_cidr)', $(el).parent()).filter(function(){ return this.value.length; }).length == 4) {
                                        var ip_value = [], ip = '', cidr = '';
                                        //alert($(el).attr('id'));
                                        $('input.ip_octet:not(.ip_cidr)', $(el).parent()).each(function(){
                                                ip_value.push(this.value);
                                        });
                                        ip = ip_value.join('.');

                                        if (options.cidr) {
                                                var $cidr = $('input.ip_cidr', $(el).parent());
                                                cidr = ($cidr.length) ? '/' + $cidr.val() : '/32';
                                        }

                                        $this.val(ip + cidr);
                                }
                                else {
                                        $this.val('');
                                }
                        }

                        $('input.ip_octet.' + $this.attr('id')).bind('keydown', function(e){
                                if (!isValidKey(e)) return false;

                                var next_octet = $(this).next('input.ip_octet.' + $this.attr('id'));
                                var prev_octet = $(this).prev('input.ip_octet' + $this.attr('id'));

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
                                        $('input.ip_octet:not(.ip_cidr)', $(this).parent()).filter(function(){ return !this.value.length; }).val('0');
                                        $('input.ip_cidr', $(this).parent()).focus();
                                        return false;
                                }
                                if (($(this).caret()[1] - $(this).caret()[0]) && isNumeric(e)) {
                                        return true;
                                }

                                // jump to next octet if maxlength is reached and number key or right arrow is pressed
                                if ((this.value.length == this.getAttribute('maxlength') && $(this).caret()[0] == this.getAttribute('maxlength') && (isNumeric(e) || e.keyCode == 39)) || (e.keyCode == 39 && $(this).caret()[0] == this.value.length)) {
                                        if (next_octet.length) {
                                                $(this).trigger('blur');
                                                next_octet.focus().caret(0);
                                                return true;
                                        }
                                }

                                // jump to previous octet if left arrow is pressed and caret is at the 0 position
                                if (e.keyCode == 37 && $(this).caret()[0] == 0) {
                                        if (prev_octet.length) {
                                                $(this).trigger('blur');
                                                prev_octet.caret(prev_octet.val().length);
                                                return false;
                                        }
                                }

                                // jump to previous octet on backspace
                                if (e.keyCode == 8 && $(this).caret()[0] == 0 && $(this).caret()[0] == $(this).caret()[1]) {
                                        if (prev_octet.length) {
                                                $(this).trigger('blur');
                                                prev_octet.focus().caret(prev_octet.val().length);
                                                return false;
                                        }
                                }
                }).bind('keyup', function(e){
                                if (this.value > 255) this.value = 255;
                                saveIP(this);
                        });

                        $('input.ip_cidr').bind('keyup', function(e){
                                if (this.value > 32) this.value = 32;

                                saveIP(this);
                        });

                });

        };
})(jQuery);
/*涔樻硶*/
//璇存槑锛歫avascript鐨勪箻娉曠粨鏋滀細鏈夎宸紝鍦ㄤ袱涓诞鐐规暟鐩镐箻鐨勬椂鍊欎細姣旇緝鏄庢樉銆傝繖涓嚱鏁拌繑鍥炶緝涓虹簿纭殑涔樻硶缁撴灉銆�//璋冪敤锛歛ccMul(arg1,arg2)
//杩斿洖鍊硷細arg1涔樹互arg2鐨勭簿纭粨鏋�function accMul(arg1,arg2){
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
//缁橬umber绫诲瀷澧炲姞涓�釜mul鏂规硶銆�Number.prototype.mul = function (arg){
    return accMul(arg, this);
}
/*闄ゆ硶*/
//璇存槑锛歫avascript鐨勯櫎娉曠粨鏋滀細鏈夎宸紝鍦ㄤ袱涓诞鐐规暟鐩搁櫎鐨勬椂鍊欎細姣旇緝鏄庢樉銆傝繖涓嚱鏁拌繑鍥炶緝涓虹簿纭殑闄ゆ硶缁撴灉銆�//璋冪敤锛歛ccDiv(arg1,arg2)
//杩斿洖鍊硷細arg1闄や互arg2鐨勭簿纭粨鏋�function accDiv(arg1,arg2){
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
//缁橬umber绫诲瀷澧炲姞涓�釜div鏂规硶銆�Number.prototype.div = function (arg){
    return accDiv(this, arg);
}
//缁橬umber绫诲瀷澧炲姞涓�釜鍥哄畾灏忔暟浣嶆暟鏂规硶銆�Number.prototype.fixed=function(n){ 
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
    //return this.optional(element) || /^[\u0391-\uFFE5\w]+$/.test(value);
    return this.optional(element) || /^[0-9a-zA-Z@_.-]+$/.test(value);
}, "<img src='/static/theme/redmond/images/iperror.png'>");

jQuery.validator.addMethod("password", function(value, element) {
    //return this.optional(element) || /^[\u0391-\uFFE5\w]+$/.test(value);
    return this.optional(element) || /^[0-9a-zA-Z_]+$/.test(value);
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
//---鏌ユ壘绗竴涓瓙鍏冪礌鐨勫嚱鏁�--//
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

function getdiskinfo(obj){
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
    			var nodisk = '<td class="disk"><span class="nodisk devname">miss</span><span class="redlight"></td>';
    			var diskline1 = ['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk];
    			var diskline2 = ['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk];
    			var diskline3 = ['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk];
    			var diskline4 = ['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk];
    			var diskline5 = ['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk];
    			var diskline6 = ['<tr class="hdisk">',nodisk,nodisk,nodisk,nodisk];
    			//var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-8" href="/clusternodeview?node=' + diskdata[0].nodename + '"><span class="mleft8"></span><span class="mright8"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-16" href="/clusternodeview?node=' + diskdata[0].nodename + '"><span class="mleft16"></span><span class="mright16"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			//var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-16"><span class="mleft16"></span><span class="mright16"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			//var nodepanel = '<div id="' + diskdata[0].nodename + '" class="group-24"><span class="mleft24"></span><span class="mright24"></span><span class="mcenter"><table boder="0" cellspacing="1" style="width:250px;background-color:#707070;line-height:0;">';
    			for(var i = 0; i < diskdata.length; i++){
    				var position = parseInt(diskdata[i].position);
    				var devname = diskdata[i].devname;
    				var devid =  diskdata[i].nodename + '-' + diskdata[i].devname;
    				if(diskdata[i].status == "in_raid"){
    					var diskclass = diskdata[i].raidname;
    					adisk = '<td id="' + devid + '" class="disk ' + diskclass +'"><span class="devname">' + devname + '</span><span class="greenlight"></td>';
    				}
    				else{
    					adisk = '<td id="' + devid + '" class="disk"><span class="devname">' + devname + '</span><span class="greenlight"></td>';
    				}
    				if(position < 5){
    					diskline1[position-0] = adisk;
    				}
    				else if(position >= 5 && position < 9){ 
    					diskline2[position-4] = adisk;
    				}
    				else if(position >= 9 && position < 13){
    					diskline3[position-8] = adisk;
    				}
    				else if(position >= 13 && position < 17){
    					diskline4[position-12] = adisk;
    				}
    				else if(position >= 17 && position < 21){
    					diskline5[position-16] = adisk;
    				}
    				else{
    					diskline6[position-20] = adisk;
    				}
    			}
				if(diskline1.length >1){
					var disklinehtml1 = diskline1.join('') + "</tr>";
				}
				else{
					var disklinehtml1 = '';
				}
				if(diskline2.length >1){
					var disklinehtml2 = diskline2.join('') + "</tr>";
				}
				else{
					var disklinehtml2 = '';
				}
				if(diskline3.length >1){
					var disklinehtml3 = diskline3.join('') + "</tr>";
				}
				else{
					var disklinehtml3 = '';
				}
				if(diskline4.length >1){
					var disklinehtml4 = diskline4.join('') + "</tr>";
				}
				else{
					var disklinehtml4 = '';
				}
				if(diskline5.length >1){
					var disklinehtml5 = diskline5.join('') + "</tr>";
				}
				else{
					var disklinehtml5 = '';
				}
				if(diskline6.length >1){
					var disklinehtml6 = diskline6.join('') + "</tr>";
				}
				else{
					var disklinehtml6 = '';
				}
				//nodepanel += disklinehtml6;
				//nodepanel += disklinehtml5;
				nodepanel += disklinehtml4;
				nodepanel += disklinehtml3;
				nodepanel += disklinehtml2;
				nodepanel += disklinehtml1;
				nodepanel += "</table>";
				//$(ELT)("#upspanel").before(nodepanel);
				$(ELT)(obj).append(nodepanel);
				$(ELT)(".disk").each(
			    	function(){
			    		$(ELT)(this).bind('mouseover', function(){
			    			$(ELT)(this).addClass('highlight');});
			    		$(ELT)(this).bind('mouseout', function(){
			    			$(ELT)(this).removeClass('highlight');});
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

