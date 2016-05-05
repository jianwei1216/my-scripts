/* This script and many more are available free online at
The JavaScript Source!! http://javascript.internet.com
Cookie script - Scott Andrew
Popup script, Copyright 2005, Sandeep Gangadharan */

function newCookie(name,value,days){
    var days = 10;// the number at the left reflects the number of days for the cookie to last
    if (days) {
        var date = new Date();
        date.setTime(date.getTime()+(days*24*60*60*1000));
        var expires = "; expires="+date.toGMTString(); 
    }
    else{
        var expires = "";
    }
    document.cookie = name+"="+value+expires+"; path=/";
}

function readCookie(name) {
    var nameSG = name + "=";
    var nuller = '';
    if (document.cookie.indexOf(nameSG) == -1){
        return nuller;
    }        
    var ca = document.cookie.split(';');
    for(var i=0; i<ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0)==' '){
            c = c.substring(1,c.length);
        }
        if (c.indexOf(nameSG) == 0){
            return c.substring(nameSG.length,c.length); 
        }
    }
    return nuller; 
}

function eraseCookie(name) {
    newCookie(name,"",1); 
}

function setCookie2(obj){
    var username = $('#name').val();
    var userpwd = $('#passwd').val();
    if($(obj).attr('checked')){
        if(username != '' && userpwd != ''){
            newCookie('username',username);
            newCookie('userpwd',userpwd);
        }
    }
    else{
        if(readCookie('username')!=''){
            eraseCookie('username');
            eraseCookie('userpwd');
        }
    }
    return true;
}

function getCookie2(){
    if(QueryString('flag')=='1'){
        eraseCookie('username');
        eraseCookie('userpwd');
    }
    username=readCookie('username');
    userpwd=readCookie('userpwd');
    if(username && userpwd){
        $('#remember').attr('checked',true);
    }
    $('#name').attr('value',username);
    $('#passwd').attr('value',userpwd);
}

function QueryString(sName){  
    var sSource = String(window.document.location);
    var sReturn = "";  
    var sQUS = "?";  
    var sAMP = "&";  
    var sEQ = "=";
    var iPos;
    
    iPos = sSource.indexOf(sQUS);
    var strQuery = sSource.substr(iPos,sSource.length - iPos);  
    var strLCQuery = strQuery.toLowerCase();  
    var strLCName = sName.toLowerCase();  
		
    iPos = strLCQuery.indexOf(sQUS + strLCName + sEQ);  
    if (iPos == -1){
        iPos = strLCQuery.indexOf(sAMP + strLCName + sEQ);  
        if(iPos == -1)  
            return "";  
    }
    sReturn = strQuery.substr(iPos + sName.length + 2,strQuery.length-(iPos + sName.length + 2));  
    var iPosAMP = sReturn.indexOf(sAMP);  
		
    if (iPosAMP == -1)  
        return sReturn;  
    else{  
        sReturn = sReturn.substr(0, iPosAMP);  
    }
    return sReturn;  
}   
