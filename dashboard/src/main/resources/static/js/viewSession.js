function sortTable(n,thElement) {
    var table, rows, switching, i, x, y, shouldSwitch, dir, switchcount = 0;
    table = thElement.parentElement.parentElement;
    switching = true;
    // Set the sorting direction to ascending:
    dir = "asc";
    /* Make a loop that will continue until
    no switching has been done: */
    while (switching) {
      // Start by saying: no switching is done:
      switching = false;
      rows = table.rows;
      /* Loop through all table rows (except the
      first, which contains table headers): */
      for (i = 1; i < (rows.length - 1); i++) {
        // Start by saying there should be no switching:
        shouldSwitch = false;
        /* Get the two elements you want to compare,
        one from current row and one from the next: */
        x = rows[i].getElementsByTagName("TD")[n];
        y = rows[i + 1].getElementsByTagName("TD")[n];
        /* Check if the two rows should switch place,
        based on the direction, asc or desc: */
        if (dir == "asc") {
          if (x.innerHTML.toLowerCase() > y.innerHTML.toLowerCase()) {
            // If so, mark as a switch and break the loop:
            shouldSwitch = true;
            break;
          }
        } else if (dir == "desc") {
          if (x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {
            // If so, mark as a switch and break the loop:
            shouldSwitch = true;
            break;
          }
        }
      }
      if (shouldSwitch) {
        /* If a switch has been marked, make the switch
        and mark that a switch has been done: */
        rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);
        switching = true;
        // Each time a switch is done, increase this count by 1:
        switchcount ++;
      } else {
        /* If no switching has been done AND the direction is "asc",
        set the direction to "desc" and run the while loop again. */
        if (switchcount == 0 && dir == "asc") {
          dir = "desc";
          switching = true;
        }
      }
    }
  }

$(function () {
  if ($('table').length != 0) {
    var width = [];
    $.each($('table:first th'), function (indexInArray, valueOfElement) {
      width.push($(this).find('div').width());
    });
    $.each($('table:not(:first)'), function () {
      $.each($(this).find('th'), function (indexInArray, valueOfElement) {
        $(this).find('div').css('width', width[indexInArray].toString())
      });
    });
  }
});

$(function(){
  $.each($('table'), function (indexInArray, valueOfElement) { 
    $(this).find('tr').slice(3).addClass('collapse');
    if ($(this).find('tr').length < 4) {
      $(this).parent().siblings('div').hide();
    } else {
      $(this).append('<tr><td>&#8942;</td><td></td><td></td><td></td><td></td></tr>');
    }
  });
})

function showMoreContent(button) {
  if (button.innerHTML == 'show more') {
    $(button).parent().siblings('div.card').find('tr:last').remove();
    $(button).parent().siblings('div.card').find('.collapse').addClass('show');
    
    button.innerHTML = 'show less';
  } else if (button.innerHTML == 'show less'){
    $(button).parent().siblings('div.card').find('table').append('<tr><td>&#8942;</td><td></td><td></td><td></td><td></td></tr>');
    $(button).parent().siblings('div.card').find('.collapse').removeClass('show');
    button.innerHTML = 'show more';
  }
}