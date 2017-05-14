<?php

namespace Drupal\pinmap\Geocoder\Providers;

use Drupal\pinmap\Geocoder\Provider;

/**
 * Class Google.
 */
class Google extends Provider {

  /**
   * {@inheritdoc}
   */
  const QUERY_URL = 'https://maps.google.com/maps/api/geocode/json';
  /**
   * {@inheritdoc}
   */
  const SERVICE_URL = 'https://developers.google.com/maps/documentation/geocoding/intro';

  /**
   * {@inheritdoc}
   */
  protected function buildQuery($address) {
    return [
      'address' => $address,
    ];
  }

  /**
   * {@inheritdoc}
   */
  protected function getCoordinates() {
    if (empty($this->result->results)) {
      return [];
    }

    $data = reset($this->result->results)->geometry->location;

    return [
      'lat' => $data->lat,
      'lng' => $data->lng,
    ];
  }

}
