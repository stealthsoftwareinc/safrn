function lockInput(button){
    $(button).siblings("input").attr('readonly', true);
    $(button).siblings("button").css('display','');
    $(button).css('display','none');
}

function unlockInput(button){
    $(button).siblings("input").attr('readonly', false);
    $(button).siblings("button").css('display','');
    $(button).css('display','none');
}

function configDownload(sessionName, configDownload){
    const url = '/organization/configfile?sessionName='+sessionName+'&configDownload='+configDownload;
    window.location.href = url;
}

$(function () {
    searchAutofill();
    $("#searchSelect").change(function () {  
        searchAutofill()
    })
})

// default confirm dialog hide
$(function () {
    $("#deleteDraftSession-dialog-confirm").dialog({
        autoOpen: false,
    })
    $("#viewSessionAccept-dialog-confirm").dialog({
        autoOpen: false,
    })
    $("#viewSessionDecline-dialog-confirm").dialog({
        autoOpen: false,
    })
})

function searchAutofill() {
    switch ($(searchSelect).val()) {
        case "organization":
            $.ajax({
                type: "get",
                url: "/organization/allorgnames",
                data: "data",
                dataType: "json",
                success: function (response) {
                    $("#searchList").find('option').remove();
                    $.each(response, function (i, option) { 
                        $("#searchList")
                        .append($("<option>").attr("value",option).text(option));
                    });
                }
            });
            break;
    
        case "session":
            $.ajax({
                type: "get",
                url: "/organization/allsessionnames",
                data: "data",
                dataType: "json",
                success: function (response) {
                    $("#searchList").find('option').remove();
                    $.each(response, function (i, option) { 
                        $("#searchList")
                        .append($("<option>").attr("value",option).text(option));
                    });
                }
            });
            break

        default:
            break;
    }  
}

$(function () { 
    $("#passwordProfileUpdateButton").click(function () {
        if ($("#passwordUpdate").val() && $("#repeatPasswordUpdate").val()) {
            $("#updatePasswordForm").submit();
        }
    })
 })

 function search() {
    searchInput = $('#searchInput').val();
    if ($('#searchSelect').val()==='organization') {
        window.location.href = "/organization/orgdetails?orgname="+searchInput;
    } else if ($('#searchSelect').val()==='session'){
        window.location.href = "/organization/searchsession?sessionName="+searchInput;
    }
 }
  
$(function () {  
    $('form#searchForm input').keydown(function (e) {
        if (e.keyCode == 13) {
            e.preventDefault();
            search();
        }
    });
})

 function showPassword(showPasswordButton) {
    $(showPasswordButton).siblings("input").prop('type','text');
    $(showPasswordButton).hide();
    $(showPasswordButton).siblings(".hidePasswordButtons").show();
 }

 function hidePassword(hidePasswordButton) {
    $(hidePasswordButton).siblings("input").prop('type','password');
    $(hidePasswordButton).hide();
    $(hidePasswordButton).siblings(".showPasswordButtons").show();
 }

function viewSessionAccept(acceptButton) {
    $( "#viewSessionAccept-dialog-confirm" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
        "OK": function() {
            acceptButton.form.submit();
            $( this ).dialog( "close" );
        },
        Cancel: function() {
            $( this ).dialog( "close" );
        }
        }
    });
    $( "#viewSessionAccept-dialog-confirm" ).dialog("open");
}

function viewSessionDecline(declineButton) {
    $( "#viewSessionDecline-dialog-confirm" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
        "OK": function() {
            declineButton.form.submit();
            $( this ).dialog( "close" );
        },
        Cancel: function() {
            $( this ).dialog( "close" );
        }
        }
    });
    $( "#viewSessionDecline-dialog-confirm" ).dialog("open");
}

function deleteDraftSession(deleteDraftSessionButton) {
    $( "#deleteDraftSession-dialog-confirm" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
        "OK": function() {
            deleteDraftSessionButton.form.submit();
            $( this ).dialog( "close" );
        },
        Cancel: function() {
            $( this ).dialog( "close" );
        }
        }
    });
    $( "#deleteDraftSession-dialog-confirm" ).dialog("open");
}

$(document).ready(function () {
    if ($('#draftSessionCount').length) {
        $.get("/organization/countdraftsessions",
            function (data, textStatus, jqXHR) {
                $('#draftSessionCount').text(data);
            }
        )
        .fail(function () {  
            $('#draftSessionCount').text(0);
        });
    }
});