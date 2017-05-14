/**
 * @file
 * Client-side implementation of the map.
 */

(function ($, easing) {
  'use strict';

  Drupal.pinmap = {};
  Drupal.pinmap.query = {};
  Drupal.pinmap.query.types = {
    LATLNG: 'coordinates',
    ADDRESS: 'name'
  };

  var $document = $(document);
  var geocoder = new google.maps.Geocoder();
  var settings = Drupal.settings.pinmap.CTools.ContentTypes.GoogleMap;

  $('.pin-map-area').each(function (i) {
    var markers = [];
    var options = settings[i];
    var $container = $(this);
    var $searchForm = $container.find('form[role=search]');
    var $searchInput = $searchForm.find('input');
    var $searchButton = $searchForm.find('button');
    var $message = $('<div class="pinmap-message" hidden>');
    var $preloader = $('<div class="pinmap-preloader" hidden>');
    var $mapContainer = $container.find('.map');
    var componentRestrictions = options.search.country ? {country: options.search.country} : null;
    var autocomplete = new google.maps.places.Autocomplete($searchInput[0], {
      types: ['geocode'],
      componentRestrictions: componentRestrictions
    });
    var map = new google.maps.Map($mapContainer[0], {
      // Disable switching between road map, landscape and other types.
      mapTypeControl: false,
      // Disable zooming on scrolling mouse wheel.
      scrollwheel: false,
      // Use custom styles from pane settings.
      styles: JSON.parse(options.map.styles)
    });
    var cleanMarkers = function () {
      markers = markers.filter(function (marker) {
        return marker.setMap(null);
      });
    };
    var geocode = function (query, callback) {
      geocoder.geocode(query, function (results, status) {
        if (google.maps.GeocoderStatus.OK === status) {
          callback(results[0]);
        }
      });
    };
    var createInfoWindow = function (address) {
      var infoWindow = new google.maps.InfoWindow();
      var html = [];

      if (address.organisation_name) {
        html.push('<strong>' + address.organisation_name + '</strong>');
      }

      html.push(address.thoroughfare + ', ' + address.postal_code + ' ' + address.locality + ', ' + address.country);

      if (address.data.phone_number) {
        html.push(Drupal.t('Phone: @phone', {'@phone': address.data.phone_number}));
      }

      infoWindow.setContent(html.join('<br>'));

      return infoWindow;
    };
    var createMarker = function (infoWindow, latLng) {
      var marker = new google.maps.Marker();

      // If one of predefined animations configured - set it.
      if (google.maps.Animation.hasOwnProperty(options.map.animation)) {
        marker.setAnimation(google.maps.Animation[options.map.animation]);
      }

      infoWindow.opened = false;

      infoWindow.addListener('closeclick', function () {
        infoWindow.hide();
      });

      infoWindow.closeAll = function () {
        // Hide all previously opened popups.
        markers.map(function (item) {
          if (item !== marker) {
            item.infoWindow.hide();
          }
        });
      };

      infoWindow.show = function () {
        if (!infoWindow.opened) {
          infoWindow.open(map, marker);
          infoWindow.opened = true;
          google.maps.event.trigger(infoWindow, 'show');
        }
      };

      infoWindow.hide = function () {
        if (infoWindow.opened) {
          infoWindow.close(map, marker);
          infoWindow.opened = false;
          google.maps.event.trigger(infoWindow, 'hide');
        }
      };

      infoWindow.toggle = function () {
        infoWindow[infoWindow.opened ? 'hide' : 'show']();
      };

      marker.addListener('click', function () {
        infoWindow.closeAll();
        infoWindow.toggle();
      });

      marker.infoWindow = infoWindow;
      marker.setPosition(latLng);
      marker.setMap(map);

      markers.push(marker);

      return latLng;
    };
    var setMapCenter = function (query) {
      var queryType = Drupal.pinmap.query.types.LATLNG;

      if (query.hasOwnProperty('geometry')) {
        query = query.geometry.location.toJSON();
      }
      else if (query.hasOwnProperty('name')) {
        queryType = Drupal.pinmap.query.types.ADDRESS;
      }
      else if (!query.lat || !query.lng) {
        // Wrong argument.
        return;
      }

      var bounds = new google.maps.LatLngBounds();
      var center = query;

      query.fields = options.content.field;
      // In kilometers.
      query.radius = options.search.radius;
      query.limit = options.search.limit;
      query.type = queryType;

      $preloader.fadeIn(easing.preloader);

      // Allow to modify the search query.
      google.maps.event.trigger(map, 'pinmap_search_query', query);

      $.get(Drupal.settings.basePath + 'pinmap/search', query, function (addresses) {
        // Clean previous message.
        $message.text('').hide();
        // Remove previous search result.
        cleanMarkers();

        if (null === addresses) {
          $message.text(Drupal.t('Search unpredictably failed. Probably query is broken. Enable search queries debugging on the module configuration page to see more.'));
        }
        else {
          // Results by name were not found. Try to geocode the query.
          if (Drupal.pinmap.query.types.ADDRESS === queryType && 0 === addresses.length) {
            return geocode({address: query.name, componentRestrictions: componentRestrictions}, setMapCenter);
          }

          // Create the markers.
          addresses.map(function (marker, i) {
            var infoWindow = createInfoWindow(marker);

            bounds.extend(createMarker(infoWindow, {
              lat: parseFloat(marker.lat),
              lng: parseFloat(marker.lng)
            }));

            // Attach current marker to the result.
            addresses[i].marker = markers[i];
          });

          if (addresses.length > 1) {
            map.fitBounds(bounds);
            center = {};
          }
          else if (1 === addresses.length) {
            center = {
              lat: parseFloat(addresses[0].lat),
              lng: parseFloat(addresses[0].lng)
            };
          }
          else {
            $message.text(Drupal.t('Nothing were found'));
            $message.hideEasing = easing.message;
          }

          if (center.lat && center.lng) {
            map.setCenter(center);
            map.setZoom(14);
          }

          // Inform subscribers about search results and allow to alter the
          // message.
          $document.trigger('pinmap_center_changed', {
            addresses: addresses,
            $container: $container,
            $message: $message,
            query: query
          });
        }

        showMessage();
      }, 'json');
    };
    var showMessage = function () {
      $preloader.fadeOut(easing.preloader);

      if ($message.is(':hidden') && '' !== $message.text()) {
        $message.fadeIn(easing.message).delay(3000);

        if ($message.hideEasing > 0) {
          $message.fadeOut($message.hideEasing);
        }
      }
    };

    if (componentRestrictions) {
      geocode({address: options.search.country}, function (place) {
        if (place.geometry.bounds) {
          map.fitBounds(place.geometry.bounds);
        }
        else if (place.geometry.location) {
          map.setCenter(place.geometry.location);
        }

        map.setZoom(5);
      });
    }
    else {
      // This is a geographical center of Earth.
      // @see https://en.wikipedia.org/wiki/Geographical_centre_of_Earth
      map.setCenter({lat: 39, lng: 34});
      map.setZoom(2);
    }

    if (options.search.geolocation) {
      $('<div class="pinmap-location-button">').each(function () {
        var restoreForm = function () {
          $element.removeClass('active');
          $searchInput.attr('readonly', false);
          $searchButton.attr('disabled', false);
        };

        var $element = $(this).bind('click', function (event) {
          event.preventDefault();

          $element.addClass('active');
          $preloader.fadeIn(easing.preloader);
          $searchInput.attr('readonly', true);
          $searchButton.attr('disabled', true);

          navigator.geolocation.getCurrentPosition(function (position) {
            // Reverse geocoding.
            geocode({location: {
              lat: position.coords.latitude,
              lng: position.coords.longitude
            }}, function (place) {
              if (place.hasOwnProperty('formatted_address')) {
                $searchInput.val(place.formatted_address);
              }

              setMapCenter(place);
              restoreForm();
            });
          }, function (error) {
            $message.text(error.message);

            showMessage();
            restoreForm();
          });
        });

        this.title = options.search.geolocation_title;

        map.controls[google.maps.ControlPosition.RIGHT_BOTTOM].push(this);
      });
    }

    // Hide search form if this is configured on a backend.
    if (options.search.disabled) {
      $searchForm.remove();
    }
    else {
      var searchHandler = function (event) {
        event.preventDefault();

        var value = $searchInput.val();

        // Send request only if field contains more than 3 characters.
        if (value.length > 3) {
          var query = {};

          query[Drupal.pinmap.query.types.ADDRESS] = value;

          setMapCenter(query);
        }
      };

      $searchInput.bind('keydown', function (event) {
        // Prevent double-sending. Google's handler does this for browser.
        if (13 === event.which) {
          event.preventDefault();
        }
      });

      // Stop browser's event propagation. Only programmatic use.
      $searchButton.bind('click', searchHandler);
      // Provide an ability to make an inaccurate search.
      $searchForm.bind('submit', searchHandler);
    }

    map.controls[google.maps.ControlPosition.CENTER].push($message[0]);
    map.controls[google.maps.ControlPosition.CENTER].push($preloader[0]);

    // Set initial height of the map (configured on a backend).
    $mapContainer.css('height', options.map.height);

    // Allow to hide message clicking on it.
    $message.bind('click', function () {
      $message.fadeOut(easing.message);
    });

    autocomplete.addListener('place_changed', function () {
      setMapCenter(autocomplete.getPlace());
    });

    google.maps.event.addDomListener(window, 'resize', function (event) {
      var center = map.getCenter();

      google.maps.event.trigger(map, event.type, markers);
      map.setCenter(center);
    });

    $document.trigger('pinmap_initialized', {
      map: map,
      options: options,
      $container: $container
    });
  });
})(jQuery, {preloader: 300, message: 500});
