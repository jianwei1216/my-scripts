/**
fromid:源list的id.
toid:目标list的id.
moveOrAppend参数("move"或者是"append"):
move -- 源list中选中的option会删除.源list中选中的option移动到目标list中,若目标list中已存在则该option不添加.
append -- 源list中选中的option不会删除.源list中选中的option添加到目标list的后面,若目标list中已存在则
该option不添加.

isAll参数(true或者false):是否全部移动或添加
*/
jQuery.listtolist = function(fromid,toid,moveOrAppend,isAll){
    if(moveOrAppend.toLowerCase() == "move"){ //move
        if(isAll == true){ //move all
            var flag = 0;
            $("#" + fromid + " option").each(function(){
                if(!$(this).hasClass("ajaximg")){
                    $(this).appendTo($("#" + toid + ":not(:has(option[value=" + $(this).val() + "]))"));
                }
                else{
                    flag = 1;
                }
            });
            if(!flag){
                $("#" + fromid).empty(); //clear 
            }
        }
        else if(isAll == false){
            var flag = 0;
            $("#" + fromid + " option:selected").each(function(){
                //将源list中的option添加到目标list,当目标list中已有该option时不做任何操作.
                if(!$(this).hasClass("ajaximg")){
                    $(this).appendTo($("#" + toid + ":not(:has(option[value=" + $(this).val() + "]))"));
                }
                else{
                    flag = 1;
                }
                //目标list中已经存在的option并没有移动,仍旧在源list中,将其清空.
                if(!flag){
                    if($("#" + fromid + " option[value=" + $(this).val() + "]").length > 0){
                        $("#" + fromid).get(0).removeChild($("#" + fromid + " option[value=" + $(this).val() + "]").get(0));
                    }
                }
            });
        }
    }
    else if(moveOrAppend.toLowerCase() == "append"){
        if(isAll == True){
            $("#" + fromid + " option").each(function(){
                $("<option></option>").val($(this).val()).text($(this).text()).appendTo($("#" + toid + ":not(:has(option[value=" + $(this).val() + "]))"));
            });
        }
        else if(isAll == false){
            $("#" + fromid + " option:selected").each(function(){
                $("<option></option>").val($(this).val()).text($(this).text()).appendTo($("#" + toid + ":not(:has(option[value=" + $(this).val() + "]))"))
            });
        }
    }
}
