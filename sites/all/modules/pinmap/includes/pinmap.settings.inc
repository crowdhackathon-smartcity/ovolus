<?php

/**
 * @file
 * Pin Map settings.
 */

/**
 * Get geocoding providers.
 *
 * @return Drupal\pinmap\Geocoder\Provider[]
 *   Geocoding providers.
 */
function pinmap_providers() {
  $providers = [];

  foreach (['Google', 'Yandex', 'MapQuest'] as $provider) {
    $providers[$provider] = 'Drupal\pinmap\Geocoder\Providers\\' . $provider;
  }

  return $providers;
}

/**
 * Get configured provider for geocoding.
 *
 * @param string $address
 *   Address to decode.
 * @param string $provider
 *   Name of the provider.
 *
 * @return string|Drupal\pinmap\Geocoder\Provider
 *   Geocoding provider name or instance of an object.
 */
function pinmap_provider($address = '', $provider = '') {
  $providers = pinmap_providers();
  $provider = $provider ?: variable_get(__FUNCTION__, 'Google');

  if (isset($providers[$provider])) {
    return empty($address) ? $provider : new $providers[$provider]($address);
  }

  return '';
}

/**
 * Whether to use a standard content type.
 *
 * @return bool
 *   State.
 */
function pinmap_default_content_type_disabled() {
  return (bool) variable_get(__FUNCTION__, FALSE);
}

/**
 * Whether cache for search results is enabled.
 *
 * @return bool
 *   State.
 */
function pinmap_is_search_cache_enabled() {
  return (bool) variable_get(__FUNCTION__, FALSE);
}

/**
 * Whether search debug mode is enabled.
 *
 * @return bool
 *   State.
 */
function pinmap_is_search_debug_enabled() {
  return (bool) variable_get(__FUNCTION__, FALSE);
}
