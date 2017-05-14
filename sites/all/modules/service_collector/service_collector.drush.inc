<?php

/**
 * @file
 * Drush integration.
 */

/**
 * Implements hook_drush_cache_clear().
 *
 * @see service_collector_cache_clear()
 */
function service_collector_drush_cache_clear(array &$types) {
  $types['service-collector'] = 'service_collector_cache_clear';
}
