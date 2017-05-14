<?php

/**
 * @file
 * Pin Map API.
 */

/**
 * Alter search query.
 *
 * @param array $query
 *   An array of search arguments.
 */
function hook_pinmap_search_query_alter(array &$query) {
  if (isset($query['custom-param'])) {
    $query['fields'][] = 'my_content_type:my_field';
  }
}

/**
 * Alter search results.
 *
 * @param array $results
 *   Found entries.
 * @param array $query
 *   Query information.
 *
 * @see _pinmap_search()
 * @see \Drupal\pinmap\Controller\Search::fetchResults()
 */
function hook_pinmap_search_results_alter(array &$results, array $query) {

}

/**
 * Provide CSS/JS for customizing the map.
 *
 * @param string $variant
 *   Theme variant, chosen for displaying.
 *
 * @return array[]
 *   An associative array with two allowed keys: "css" and "js". Every value of
 *   key is an indexed array with relative, to module folder, paths to assets
 *   to be included.
 *
 * @see \Drupal\pinmap\CTools\ContentTypes\GoogleMap::HOOK_ASSETS
 * @see \Drupal\pinmap\CTools\ContentTypes\GoogleMap::addAssets()
 */
function hook_pinmap_pane_assets($variant) {
  $assets = [];

  $assets['css'][] = 'relative/path/to.css';
  $assets['js'][] = 'relative/path/to.js';
  $assets['js'][] = "relative/path/to.$variant.js";

  return $assets;
}

/**
 * Provide searchers definitions.
 *
 * @return \Drupal\pinmap\Searcher\SearcherInterface[]
 *   An associative array where key is an unique name of searcher and value
 *   - fully qualified path to class which implements searcher interface.
 *
 * @see pinmap_searchers()
 */
function hook_pinmap_searchers() {
  $searchers = [];

  $searchers['name'] = \Drupal\pinmap\Searcher\Name::class;

  return $searchers;
}

/**
 * Allow other modules to modify searchers definitions.
 *
 * @param \Drupal\pinmap\Searcher\SearcherInterface[] $searchers
 *   List of defined searchers.
 *
 * @see hook_pinmap_searchers()
 */
function hook_pinmap_searchers_alter(array &$searchers) {
  $searchers['name'] = \Drupal\my_module\Searcher\Name::class;
}
