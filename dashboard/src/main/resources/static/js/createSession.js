var createSessionFormData;

function createSessionBack() {
    let thisPageNumber = parseInt($("#pageNumber").html()); 
    let newPageNumber = parseInt($("#pageNumber").html())-1; 
    $('.createSession-page-1').hide();
    $('.createSession-page-2').hide();
    $('.createSession-page-3').hide();
    $('.createSession-page-4').hide();
    $('.createSession-page-'+newPageNumber+':not(:disabled)').show();
    $("#pageNumber").html(newPageNumber);
    $('#createSessionPageLink button:nth-child('+thisPageNumber+')').removeClass('text-dark').addClass('text-primary');
    $('#createSessionPageLink button:nth-child('+newPageNumber+')').removeClass('text-primary').addClass('text-dark');
}
function createSessionNext() {
    let thisPageNumber = parseInt($("#pageNumber").html()); 
    let newPageNumber = parseInt($("#pageNumber").html())+1; 
    $('.createSession-page-1').hide();
    $('.createSession-page-2').hide();
    $('.createSession-page-3').hide();
    $('.createSession-page-4').hide();
    $('.createSession-page-'+newPageNumber+':not(:disabled)').show();
    $("#pageNumber").html(newPageNumber);
    $('#createSessionPageLink button:nth-child('+thisPageNumber+')').removeClass('text-dark').addClass('text-primary');
    $('#createSessionPageLink button:nth-child('+newPageNumber+')').removeClass('text-primary').addClass('text-dark');

    //make session name read only
    // $('#createsessionName').prop('readonly','readonly');
}

function createSessionPreview() {
    $.each($('main select:disabled, main input:disabled'), function (indexInArray, valueOfElement) { 
        $(this).siblings('span').html('');
    });
    // $('main input').prop('readonly', 'readonly');
    $.each($('main input:not(:disabled,[type=file])'), function (indexInArray, valueOfElement) { 
        $(this).siblings('span').html($(this).val());
    });
    // $('main textarea').prop('readonly', 'readonly');
    $.each($('main textarea:not(:disabled)'), function (indexInArray, valueOfElement) { 
        $(this).siblings('span').html($(this).val());
    });
    // $('main option:not(:selected)').hide();
    $.each($('main select:not(:disabled)'), function (indexInArray, valueOfElement) { 
        $(this).siblings('span').html($(this).val());
    });
    // $('main input:checkbox').click(function () {  return false});
    $('.createSession-page-1').hide();
    $('.createSession-page-2').hide();
    $('.createSession-page-3').hide();
    $('.createSession-page-4').hide();
    $('.createSession-page-4').show();
    $("#pageNumber").html(4);
    $('#createSessionPageLink button:nth-child('+3+')').removeClass('text-dark').addClass('text-primary');
    $('#createSessionPageLink button:nth-child('+4+')').removeClass('text-primary').addClass('text-dark');
}

function createSessionPageLink(pageNumber) {
    let thisPageNumber = parseInt($("#pageNumber").html()); 
    if (thisPageNumber == pageNumber) {
        return false;
    }
    if (pageNumber == 4){
        createSessionPreview();
        $('#createSessionPageLink button:nth-child('+thisPageNumber+')').removeClass('text-dark').addClass('text-primary');
        $('#createSessionPageLink button:nth-child('+pageNumber+')').removeClass('text-primary').addClass('text-dark');
        return true;
    }
    $('.createSession-page-1').hide();
    $('.createSession-page-2').hide();
    $('.createSession-page-3').hide();
    $('.createSession-page-4').hide();
    $('.createSession-page-'+pageNumber+':not(:disabled)').show();
    $('#createSessionPageLink button:nth-child('+thisPageNumber+')').removeClass('text-dark').addClass('text-primary');
    $('#createSessionPageLink button:nth-child('+pageNumber+')').removeClass('text-primary').addClass('text-dark');
    $("#pageNumber").html(pageNumber);

    //make session name read only
    // $('#createsessionName').prop('readonly','readonly');
}

function createSessionReset() {
    window.location.replace("/organization/createsession");
}

function addPeer() {
    $.get("/organization/addrow?sessionSection=PEERS", $("main form[action='/organization/createsession'][method='POST']").serialize(),function(responseTxt, statusTxt, xhr){
        if(statusTxt == "success")
            mainDiv=$('<div></div>').html(responseTxt);
            $('main').html($(mainDiv).find('main').html());
            $.getScript('/js/createSession.js');
            createSessionPageLink(3);
            updatePeerOrdinalValuesOptions();
        if(statusTxt == "error")
          alert("Error: " + xhr.status + ": " + xhr.statusText);
      });
}

function addVertical() {
    $.get("/organization/addrow?sessionSection=VERTICALS", $("main form[action='/organization/createsession'][method='POST']").serialize(),function(responseTxt, statusTxt, xhr){
        if(statusTxt == "success")
            mainDiv=$('<div></div>').html(responseTxt);
            $('main').html($(mainDiv).find('main').html());
            $.getScript('/js/createSession.js');
            createSessionPageLink(2);
            updatePeerOrdinalValuesOptions();
        if(statusTxt == "error")
          alert("Error: " + xhr.status + ": " + xhr.statusText);
      });
}

function addColumn(i) {
    $.get("/organization/addrow?sessionSection=COLUMNS&verticalIndex="+i, $("main form[action='/organization/createsession'][method='POST']").serialize(),function(responseTxt, statusTxt, xhr){
        if(statusTxt == "success")
            mainDiv=$('<div></div>').html(responseTxt);
            $('main').html($(mainDiv).find('main').html());
            $.getScript('/js/createSession.js');
            createSessionPageLink(2);
            updatePeerOrdinalValuesOptions();
        if(statusTxt == "error")
          alert("Error: " + xhr.status + ": " + xhr.statusText);
      });
}

function createsessionSave() {
    $.post("/organization/savesession", $("main form[action='/organization/createsession'][method='POST']").serialize(),
        function (data, textStatus, jqXHR) {
            if(data.success==='true'){
                createSessionFormData = $("main form[action='/organization/createsession'][method='POST']").serialize();
                $( "#saveDraft-dialog-alert" ).dialog("open");
                $.get("/organization/countdraftsessions",
                function (data, textStatus, jqXHR) {
                    $('#draftSessionCount').text(data);
                }
            );
            } else {
                $('#sessionNameError').append($('<p></p>').text(data.message));
            }
        }
    );
}

function createsessionSubmit() {
    window.onbeforeunload = function () {
    };
    var verticalTables = $('table.verticalForms')
        .toArray();
    var peerTable = $('table#peerForm');
    var validation =
        (verticalTables.filter(
            verticalTable => $(verticalTable).find('tr').length < 3
            )
            .length == 0
        )
        && peerTable.find('tr').length > 3;
    var columnNames = $("input.columnNames").toArray();
    var columnNamesValidation = 
        (
           columnNames.filter(columnName=>!$(columnName).val())
                .length == 0
        );
    var organizationNames = $("input.organizationNames").toArray();
    var organizationNamesValidation = 
        (
           organizationNames.filter(organizationName=>!$(organizationName).val())
                .length == 0
        );
    if (!validation) {
        alert("columns or peers cannot be empty")
        return;
    } 
    if (!columnNamesValidation) {
        alert("column name cannot be empty")
        return;
    }
    if (!organizationNamesValidation) {
        alert("organization name cannot be empty")
        return;
    }
    $("main form[action='/organization/createsession'][method='POST']").submit();
}
function deletePeer(index) {
    $( "#deletePeer-dialog-confirm" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
        "OK": function() {
            $.get("/organization/deleterow?sessionSection=PEERS&index="+index, $("main form[action='/organization/createsession'][method='POST']").serialize(),function(responseTxt, statusTxt, xhr){
                if(statusTxt == "success")
                    mainDiv=$('<div></div>').html(responseTxt);
                    $('main').html($(mainDiv).find('main').html());
                    $.getScript('/js/createSession.js');
                    createSessionPageLink(3);
                    updatePeerOrdinalValuesOptions();
                if(statusTxt == "error")
                    alert("Error: " + xhr.status + ": " + xhr.statusText);
                });
            $( this ).dialog( "close" );
        },
        Cancel: function() {
            $( this ).dialog( "close" );
        }
        }
    });
    $( "#deletePeer-dialog-confirm" ).dialog("open");
}

function deleteVertical(index) {
    $( "#deleteVertical-dialog-confirm" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
        "OK": function() {
            $.get("/organization/deleterow?sessionSection=VERTICALS&index="+index, $("main form[action='/organization/createsession'][method='POST']").serialize(),function(responseTxt, statusTxt, xhr){
                if(statusTxt == "success")
                    mainDiv=$('<div></div>').html(responseTxt);
                    $('main').html($(mainDiv).find('main').html());
                    $.getScript('/js/createSession.js');
                    createSessionPageLink(2);
                    updatePeerOrdinalValuesOptions();
                if(statusTxt == "error")
                    alert("Error: " + xhr.status + ": " + xhr.statusText);
                });
            $( this ).dialog( "close" );
        },
        Cancel: function() {
            $( this ).dialog( "close" );
        }
        }
    });
    $( "#deleteVertical-dialog-confirm" ).dialog("open");
}

function deleteColumn(i,j) {
    $( "#deleteColumn-dialog-confirm" ).dialog({
        resizable: false,
        height: "auto",
        width: 400,
        modal: true,
        buttons: {
        "OK": function() {
            $.get("/organization/deleterow?sessionSection=COLUMNS&index="+j+"&verticalIndex="+i, $("main form[action='/organization/createsession'][method='POST']").serialize(),function(responseTxt, statusTxt, xhr){
                if(statusTxt == "success")
                    mainDiv=$('<div></div>').html(responseTxt);
                    $('main').html($(mainDiv).find('main').html());
                    $.getScript('/js/createSession.js');
                    createSessionPageLink(2);
                    updatePeerOrdinalValuesOptions();
                if(statusTxt == "error")
                    alert("Error: " + xhr.status + ": " + xhr.statusText);
                });
            $( this ).dialog( "close" );
        },
        Cancel: function() {
            $( this ).dialog( "close" );
        }
        }
    });
    $( "#deleteColumn-dialog-confirm" ).dialog("open");
}

$(function () {
    $("#saveDraft-dialog-alert").dialog({
        autoOpen: false,
    })
    $("#deletePeer-dialog-confirm").dialog({
        autoOpen: false,
    })
    $("#deleteColumn-dialog-confirm").dialog({
        autoOpen: false,
    })
    $("#deleteVertical-dialog-confirm").dialog({
        autoOpen: false,
    })
})


function editSessionHelp() {
    $('main input').removeAttr('readonly');
    $('main textarea').removeAttr('readonly');
    $('.peerValues').prop('readonly','readonly');
    $('main option:not(:selected)').show();
    $('input:checkbox').off('click');
}

function editSession() {
    editSessionHelp();
    createSessionPageLink(1);
}

function editVertical() {
    editSessionHelp();
    createSessionPageLink(2);
}

function editPeer() {
    editSessionHelp();
    createSessionPageLink(3);
}

//Columns lower bounds and upper bounds
function updateLowerBoundAndUpperBoundInput(dataType) {
    if ($(dataType).val()=="integer") {
        $(dataType).parent().siblings().find('.lowerBounds').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.lowerBounds').siblings('button').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.upperBounds').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.signed').show().prop('disabled',false);
        $(dataType).parent().siblings().find('.numberOfBits').show().prop('disabled',false);
        $(dataType).parent().siblings().has('.signed').find('.divToShow').html("Yes if this column is signed");
        $(dataType).parent().siblings().has('.numberOfBits').find('.divToShow').html("Number of bits");
    } else if($(dataType).val()=="real") {
        $(dataType).parent().siblings().find('input.lowerBounds').show().prop('disabled',false).prop('type','number').prop('min',1).prop('placeholder','# integer digits');
        $(dataType).parent().siblings().find('textarea.lowerBounds').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.lowerBounds').siblings('button').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.upperBounds').show().prop('disabled',false).prop('type','number').prop('min',1).prop('placeholder','# decimal digits');
        $(dataType).parent().siblings().find('.signed').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.numberOfBits').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().has('.lowerBounds').find('.divToShow').html("Number of integer digits");
        $(dataType).parent().siblings().has('.upperBounds').find('.divToShow').html("Number of decimal digits");
    } else if($(dataType).val()=="categorical") {
        $(dataType).parent().siblings().find('textarea.lowerBounds').show().prop('disabled',false).prop('placeholder',', separated values');
        $(dataType).parent().siblings().find('input.lowerBounds').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.lowerBounds').siblings('button').show().prop('disabled',false);
        $(dataType).parent().siblings().find('.upperBounds').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.upperBounds').siblings('span').text('');
        $(dataType).parent().siblings().find('.signed').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.numberOfBits').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().has('.lowerBounds').find('.divToShow').html("Comma separated values");
    } else {
        $(dataType).parent().siblings().find('.lowerBounds').hide().prop('disabled', 'disabled');
        $(dataType).parent().siblings().find('.lowerBounds').siblings('button').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.upperBounds').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.signed').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.numberOfBits').hide().prop('disabled','disabled');
        $(dataType).parent().siblings().find('.lowerBounds').siblings('span').text('');
        $(dataType).parent().siblings().find('.upperBounds').siblings('span').text('');
    }
}

$(function () {  
    $.each($('.dataTypes'), function (indexInArray, valueOfElement) { 
        updateLowerBoundAndUpperBoundInput(this);
    });
    $('.dataTypes').change(function (e) { 
        updateLowerBoundAndUpperBoundInput(this);
    });
})

//set vertical name value
$(function (){
    $.each($('.verticalNames'), function (indexInArray, valueOfElement) {
        if ($(this).val()=="") {
            $(this).val($(this).siblings('span').text())
        }
    });
})

// Peer table functions
$(function(){
    $.each($('.peerTypes'), function (indexInArray, valueOfElement) { 
        if ($(this).val() === "ANALYST") {
            $(this).parent().siblings().find(".peerValues").show().prop('disabled',false).val("ALLOWEDQUERYCOUNT");
        } else if ($(this).val() === "DATAOWNER"){
            $(this).parent().siblings().find(".peerValues").show().prop('disabled',false).val("VERTICAL");
        } else {
            $(this).parent().siblings().find(".peerValues").prop('disabled','disabled').hide();
        }
    });
})

$(function() {
    $(".peerTypes").change(function () {  
        if ($(this).val() === "ANALYST") {
            $(this).parent().siblings().find(".peerValues").show().prop('disabled',false).val("ALLOWEDQUERYCOUNT");
        } else if ($(this).val() === "DATAOWNER"){
            $(this).parent().siblings().find(".peerValues").show().prop('disabled',false).val("VERTICAL");
        } else {
            $(this).parent().siblings().find(".peerValues").prop('disabled','disabled').hide();
        }
    });
})

//update peer ordinal values based on vertical numbers and peer types
function updatePeerOrdinalValuesOptions() {
    $.each($('.peerTypes'), function (indexInArray, valueOfElement) { 
        if ($(this).val()=="ANALYST") {
            $(this).parent().siblings().find('input.ordinalValues').prop('disabled',false).show();
            $(this).parent().siblings().find('select.ordinalValues').prop('disabled','disabled').hide();
        } else if ($(this).val() === "DATAOWNER"){
            $(this).parent().siblings().find('input.ordinalValues').prop('disabled',true).hide();
            $(this).parent().siblings().find('select.ordinalValues').prop('disabled',false).show();
            let verticalNum = $('div.verticalForms').length; 
            $.each($(this).parent().siblings().find('select.ordinalValues'), function (indexInArray, valueOfElement) {
                let oldOrdinalValue = $(this).siblings($('p')).text();
                for (let index = 1; index <=verticalNum; index++) {
                    $(this).append(new Option('Vertical '+index,index));
                }
                $(this).val(parseInt(oldOrdinalValue));
            });
        } else {
            $(this).parent().siblings().find('input.ordinalValues').prop('disabled','disabled').hide();
            $(this).parent().siblings().find('select.ordinalValues').prop('disabled','disabled').hide();
        } 
    });
}
window.onload = (event) => {
    updatePeerOrdinalValuesOptions();
    createSessionFormData = $("main form[action='/organization/createsession'][method='POST']").serialize();
};

$(function () {  
    $('.peerTypes').change(function () {
        if ($(this).val()=="ANALYST") {
            $(this).parent().siblings().find('input.ordinalValues').prop('disabled',false).show();
            $(this).parent().siblings().find('select.ordinalValues').prop('disabled','disabled').hide();
        } else if ($(this).val() === "DATAOWNER"){
            $(this).parent().siblings().find('input.ordinalValues').prop('disabled','disabled').hide();
            $(this).parent().siblings().find('select.ordinalValues').prop('disabled',false).show();
            let verticalNum = $('div.verticalForms').length; 
            $.each($(this).parent().siblings().find('select.ordinalValues'), function (indexInArray, valueOfElement) {
                $(this).empty();
                for (let index = 1; index <=verticalNum; index++) {
                    $(this).append(new Option('Vertical '+index,index));
                }
            });
        } else {
            $(this).parent().siblings().find('input.ordinalValues').prop('disabled','disabled').hide();
            $(this).parent().siblings().find('select.ordinalValues').prop('disabled','disabled').hide();
        } 
      });
})

//update ipAddress and ports based on organization names
$(function() {
    $(".organizationNames").change(function () {  
        $.ajax({
            type: "get",
            url: "/organization/getconnectioninfo?someOrgName="+$(this).val(),
            dataType: "json",
            context: this,
            success: function (data) {  
                $(this).parent().siblings().find(".ipAddresses").val(data.ipAddress);
                $(this).parent().siblings().find(".ports").val(data.port);
            }
        })
    })
})

//display text hovering
var setTimeoutConst;
$(function () {
    $(".divHovering").hover(function (event) {
        let divHovering = this;
        setTimeoutConst = setTimeout(function() {
            $(divHovering).siblings(".divToShow").css({ top: event.clientY+5, left: event.clientX+5 }).show();
        }, 500);
    }, function () {
        clearTimeout(setTimeoutConst);
        $(this).siblings(".divToShow").hide();
    });
})

//automatically update sessionName text automatically
$(function () {
    $('#createsessionName').change(function (event) {
        $(this).siblings($('span')).html($(this).val());
      })
})

// show alert only if form is changed
window.onbeforeunload = function () {
    if (createSessionFormData != $("main form[action='/organization/createsession'][method='POST']").serialize()) {
         return true;
    }
};

// upload csv files
function readFileAsString(input) {
    var files = input.files;
    if (files.length === 0) {
        console.log('No file is selected');
        return;
    }

    var reader = new FileReader();
    reader.onload = function(event) {
        $(input).siblings('textarea.lowerBounds').val(event.target.result);
    };
    reader.readAsText(files[0]);
}

function thisFileUpload(button) {
    $(button).siblings('[type=file]').click();
}

// import existing private studies
$(function () {
    $.ajax({
        type: "get",
        url: "/organization/allsessionnames",
        data: "data",
        dataType: "json",
        success: function (response) {
            $("#existingPrivateStudiesSelect").find('option').remove();
            if (response.length) {
                $.each(response, function (i, option) { 
                    $("#existingPrivateStudiesSelect")
                    .append($("<option>").attr("value",option).text(option));
                });
            } else {
                $("#existingPrivateStudiesSelect").parents ('table').remove();
            }
        }
    });
});

function importExistingPrivateStudiesButton() {
    $.get("/organization/createsession", $('#existingPrivateStudiesSelect').serialize(),
        function (responseTxt, statusTxt, xhr) {
            if(statusTxt == "success")
                sessionName = $('#createsessionName').val();
                sessionAbout = $('#createSessionAbout').val();
                mainDiv=$('<div></div>').html(responseTxt);
                $('main').html($(mainDiv).find('main').html());
                $.getScript('/js/createSession.js');
                updatePeerOrdinalValuesOptions();
                $('#createsessionName').val(sessionName);
                $('#createSessionAbout').val(sessionAbout);
                alert("Study import successful");
            if(statusTxt == "error")
                alert("Error: " + xhr.status + ": " + xhr.statusText);
        }
    )
}