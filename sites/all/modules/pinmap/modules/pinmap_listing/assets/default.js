/**
 * @file
 * Forms listing of found results beneath the map.
 */

(function ($) {
  'use strict';

  /**
   * @type {jQuery}
   */
  var $listing;
  /**
   * @type {jQuery}
   */
  var $body = $('html, body');
  /**
   * @type {{path: string, scale: number, fillColor: string, fillOpacity: number}}
   */
  var icon = {
    path: 'M27.648 -41.399q0 -3.816 -2.7 -6.516t-6.516 -2.7 -6.516 2.7 -2.7 6.516 2.7 6.516 6.516 2.7 6.516 -2.7 2.7 -6.516zm9.216 0q0 3.924 -1.188 6.444l-13.104 27.864q-0.576 1.188 -1.71 1.872t-2.43 0.684 -2.43 -0.684 -1.674 -1.872l-13.14 -27.864q-1.188 -2.52 -1.188 -6.444 0 -7.632 5.4 -13.032t13.032 -5.4 13.032 5.4 5.4 13.032z',
    scale: 0.6,
    fillColor: 'green',
    fillOpacity: 1
  };
  /**
   * @param {Object} address
   *
   * @return {String}
   */
  var createMarkup = function (address) {
    var html = '';

    html += '<h5>' + address.organisation_name + '</h5>';
    html += '<address>';
    html += '<p>' + address.thoroughfare + '</p>';
    html += '<p>' + address.postal_code + ' ' + address.locality + '</p>';
    html += '<p>' + address.country + '</p>';
    html += '</address>';

    if (address.data.phone_number) {
      html += '<p>' + Drupal.t('Phone: @phone', {'@phone': address.data.phone_number}) + '</p>';
    }

    return '<li>' + html + '</li>';
  };
  /**
   * @param {Event} event
   *   Event object.
   * @param {Object} data
   *   Event payload.
   * @param {google.maps.Map} data.map
   *   An instance of the map.
   * @param {Object} data.options
   *   Set of options for the map and services.
   * @param {jQuery} data.$container
   *   Map container.
   */
  var initialized = function (event, data) {
    /**
     * @param {google.maps.Marker[]} markers
     */
    var resize = function (markers) {
      // Do something with markers on window resizing.
    };

    /**
     * @param {Object} query
     * @param {Object} query.fields
     *   An associative array with "content_type:field_name" as key and value.
     * @param {Number} query.lat
     *   Will be exist only for coordinates query.
     * @param {Number} query.lng
     *   Will be exist only for coordinates query.
     * @param {Number} query.limit
     *   Search results limitation.
     * @param {Number} query.radius
     *   Search radius.
     * @param {String} query.type
     *   One of query types. {@see Drupal.pinmap.query.types}
     * @param {String} query.name
     *   Will be exist when no "lat" and "lng". Means named query.
     */
    var pinmap_search_query = function(query) {
      // Add custom parameters to the search query to rely on them on
      // the backend. Modifying existing params can lead to unpredictable
      // behaviour and it is not recommended to be doing.
    };

    data.map.addListener('resize', resize);
    data.map.addListener('pinmap_search_query', pinmap_search_query);
  };
  /**
   * @param {Event} event
   *   Event object.
   * @param {Object} data
   *   Event payload.
   * @param {jQuery} data.$container
   *   Container with a map.
   * @param {jQuery} data.$message
   *   Use ".text()" method for changing the message. In case of emptiness message will not be displayed to the user.
   * @param {Number} data.$message.hideEasing
   *   Set to "0" to not hide it after expiring the delay.
   * @param {Object[]} data.addresses
   *   List of found addresses.
   * @param {String} data.addresses[].organisation_name
   * @param {String} data.addresses[].thoroughfare
   * @param {String} data.addresses[].postal_code
   * @param {String} data.addresses[].locality
   * @param {Object} data.addresses[].data
   * @param {Object} data.addresses[].data.phone_number
   * @param {google.maps.Marker} data.addresses[].marker
   *   Marker representing the address. {@link https://developers.google.com/maps/documentation/javascript/reference#Marker}
   * @param {google.maps.InfoWindow} data.addresses[].marker.infoWindow
   *   Popup attached to the marker. {@link https://developers.google.com/maps/documentation/javascript/reference#InfoWindow}
   * @param {Boolean} data.addresses[].marker.infoWindow.opened
   *   Indicates whether popup is opened.
   * @param {Function} data.addresses[].marker.infoWindow.closeAll
   *   Close all popups, except current.
   * @param {Function} data.addresses[].marker.infoWindow.show
   *   Method for displaying the popup. Fires the "show" event.
   * @param {Function} data.addresses[].marker.infoWindow.hide
   *   Method for hiding the popup. Fires the "hide" event.
   * @param {Function} data.addresses[].marker.infoWindow.toggle
   *   Method for displaying/hiding the popup depending on its current state.
   * @param {Object} data.query
   *   Data which were used for search query.
   * @param {String[]} data.query.fields
   *   List of "content_type:field_name" to search for results in.
   * @param {Number} data.query.radius
   *   Search radius in kilometers.
   * @param {Number} data.query.limit
   *   Number to limit the results.
   * @param {String} data.query.type
   *   The type of query. {@see Drupal.pinmap.query.types}
   */
  var center_changed = function (event, data) {
    // Do not hide the message (only by clicking on it).
    if (!data.addresses.length) {
      data.$message.text(Drupal.t('No addresses were found'));
      data.$message.hideEasing = 0;
    }

    // Initialize container with items which were found.
    if (undefined === $listing) {
      $listing = $('<ul class="clearfix pinmap-listing" />');
      data.$container.append($listing);
    }

    // Ensure container are clear before filling it up.
    $listing.html('');

    data.addresses.map(function (address) {
      var $item = $(createMarkup(address));
      var highlight = {
        in: function () {
          if (!$item.blocked) {
            $item.addClass('active');

            icon.fillColor = 'red';
            address.marker.setIcon(icon);
          }
        },
        out: function () {
          if (!$item.blocked) {
            $item.removeClass('active');

            icon.fillColor = 'green';
            address.marker.setIcon(icon);
          }
        }
      };

      $item.blocked = false;
      $item.bind('mouseenter', highlight.in);
      $item.bind('mouseleave', highlight.out);

      address.marker.setIcon(icon);
      address.marker.addListener('mouseover', highlight.in);
      address.marker.addListener('mouseout', highlight.out);

      address.marker.infoWindow.addListener('show', function () {
        $item.blocked = true;
        highlight.in();
      });

      address.marker.infoWindow.addListener('hide', function () {
        $item.blocked = false;
        highlight.out();
      });

      $item.bind('click', function () {
        $body.animate({scrollTop: data.$container.offset().top}, 500);

        address.marker.infoWindow.closeAll();
        address.marker.infoWindow.toggle();
      });

      $listing.append($item);
    });
  };

  $(document)
    .bind('pinmap_initialized', initialized)
    .bind('pinmap_center_changed', center_changed);
})(jQuery);
