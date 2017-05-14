<?php

/**
 * @file
 * Drush integration.
 */

/**
 * Implements hook_drush_cache_clear().
 *
 * @see _pinmap_cache_search_clear()
 */
function pinmap_drush_cache_clear(array &$types) {
  $types['pinmap-search'] = '_pinmap_cache_search_clear';
}
