(function ($) {
  'use strict';

  Drupal.tableDrag.prototype.initColumns = function () {
    for (var group in this.tableSettings) {
      // Find the first field in this group.
      for (var d in this.tableSettings[group]) {
        var field = $('.' + this.tableSettings[group][d].target + ':first', this.table);
        if (field.length && this.tableSettings[group][d].hidden) {
          var hidden = this.tableSettings[group][d].hidden;
          var cell = field.closest('td');
          break;
        }
      }

      // Mark the column containing this field so it can be hidden.
      if (hidden && cell[0]) {
        // Match immediate children of the parent element to allow nesting.
        var columnIndex = $('> td', cell.parent()).index(cell.get(0));
        $('> thead > tr, > tbody > tr, > tr', this.table).each(function () {
          // Get the columnIndex and adjust for any colspans in this row.
          var index = columnIndex;
          var cells = $(this).children();
          cells.each(function (n) {
            if (n < index && this.colSpan > 1) {
              index -= this.colSpan - 1;
            }
          });
          cell = cells.eq(index);
          cell.addClass('tabledrag-' + (cell.attr('colSpan') > 1 ? 'has-colspan' : 'hide'));
        });
      }
    }

    // Now hide cells and reduce colspans unless cookie indicates previous choice.
    // Set a cookie if it is not already present.
    if ($.cookie('Drupal.tableDrag.showWeight') === null) {
      $.cookie('Drupal.tableDrag.showWeight', 0, {
        path: Drupal.settings.basePath,
        // The cookie expires in one year.
        expires: 365
      });
      this.hideColumns();
    }
    // Check cookie value and show/hide weight columns accordingly.
    else {
      if ($.cookie('Drupal.tableDrag.showWeight') == 1) {
        this.showColumns(false);
      }
      else {
        this.hideColumns();
      }
    }
  };

  Drupal.tableDrag.prototype.hideColumns = function () {
    // Hide weight/parent cells and headers.
    $('.tabledrag-hide', 'table.tabledrag-processed').css('display', 'none');
    // Show TableDrag handles.
    $('.tabledrag-handle', 'table.tabledrag-processed').css('display', '');
    // Reduce the colspan of any effected multi-span columns.
    $('.tabledrag-has-colspan', this.table).each(function () {
      this.colSpan -= 1;
    });
    // Change link text.
    $('.tabledrag-toggle-weight').text(Drupal.t('Show row weights'));
    // Change cookie.
    $.cookie('Drupal.tableDrag.showWeight', 0, {
      path: Drupal.settings.basePath,
      // The cookie expires in one year.
      expires: 365
    });
    // Trigger an event to allow other scripts to react to this display change.
    $('table.tabledrag-processed').trigger('columnschange', 'hide');
  };

  Drupal.tableDrag.prototype.showColumns = function (updateColSpans) {
    // Show weight/parent cells and headers.
    $('.tabledrag-hide', 'table.tabledrag-processed').css('display', '');
    // Hide TableDrag handles.
    $('.tabledrag-handle', 'table.tabledrag-processed').css('display', 'none');
    // Increase the colspan for any columns where it was previously reduced.
    if (undefined === updateColSpans ? true : updateColSpans) {
      $('.tabledrag-has-colspan', this.table).each(function () {
        this.colSpan += 1;
      });
    }
    // Change link text.
    $('.tabledrag-toggle-weight').text(Drupal.t('Hide row weights'));
    // Change cookie.
    $.cookie('Drupal.tableDrag.showWeight', 1, {
      path: Drupal.settings.basePath,
      // The cookie expires in one year.
      expires: 365
    });
    // Trigger an event to allow other scripts to react to this display change.
    $('table.tabledrag-processed').trigger('columnschange', 'show');
  };
})(jQuery);
