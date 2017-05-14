<?php

namespace Drupal\pinmap\Controller;

use Drupal\pinmap\Searcher\SearcherInterface;

/**
 * Search controller.
 */
final class Search {

  /**
   * Search query.
   *
   * @var array
   */
  private $query = [];
  /**
   * Searcher, which will be determined by search query.
   *
   * @var SearcherInterface
   */
  private $searcher;
  /**
   * List of defined searchers.
   *
   * @var SearcherInterface[]
   */
  private $searchers = [];
  /**
   * Whether cache is enabled for search queries.
   *
   * @var bool
   */
  private $cacheEnabled = FALSE;

  /**
   * Search constructor.
   */
  public function __construct() {
    $this->searchers = pinmap_searchers();
    $this->cacheEnabled = pinmap_is_search_cache_enabled();
  }

  /**
   * Validates and sets the query for performing the search.
   *
   * @param array $query
   *   An array of search parameters to validate.
   */
  public function setQuery(array $query) {
    if (empty($query['type'])) {
      throw new \InvalidArgumentException('Request is missing search type');
    }

    $original_query = $query;
    $searcher = $this->getSearcher($query['type']);
    $params = $searcher::requirements();

    // Validate the query.
    self::validateQueryParams($query, $params);
    // Let modules to modify the query depending on passed params.
    drupal_alter('pinmap_search_query', $query);

    // Query modified. Needs re-validation.
    if ($query !== $original_query) {
      self::validateQueryParams($query, $params);
    }

    // This value is no longer needed.
    unset($query['type']);

    $this->query = $query;
    $this->searcher = $searcher;
  }

  /**
   * Returns results of search (from cache if configured).
   *
   * @return array
   *   Addresses, which were found by search query.
   */
  public function getResults() {
    // Query was not set by calling appropriate method.
    if (empty($this->query)) {
      throw new \RuntimeException('You must specify search query first');
    }

    // MD5 sum will differs if something will be changed in query.
    $cache_id = md5(serialize($this->query));
    $cache = $this->cacheEnabled ? cache_get($cache_id, PINMAP_CACHE_SEARCH_BIN) : FALSE;

    // Cache is enabled and results were found in there.
    if (FALSE !== $cache && !empty($cache->data)) {
      return $cache->data;
    }

    $results = $this->fetchResults();

    if ($this->cacheEnabled) {
      cache_set($cache_id, $results, PINMAP_CACHE_SEARCH_BIN, CACHE_TEMPORARY);
    }

    return $results;
  }

  /**
   * Returns an array of arrays with content type and field name to query.
   *
   * @return array[]
   *   Query targets.
   *
   * @todo Only "node" entity available for now. Should this be extended?
   */
  private function getQueryTargets() {
    $field_instances = field_info_instances('node');
    $fields = [];

    foreach ($this->query['fields'] as $i => $item) {
      list($content_type, $field_name) = pinmap_parse_destination($item);

      if (empty($field_instances[$content_type])) {
        throw new \InvalidArgumentException(sprintf('Content type "%s" does not exists', $content_type));
      }

      if (empty($field_instances[$content_type][$field_name])) {
        throw new \InvalidArgumentException(sprintf('Field "%s" of "%s" content type does not exist', $field_name, $content_type));
      }

      $fields[] = [$content_type, $field_name];
    }

    unset($this->query['fields']);

    return $fields;
  }

  /**
   * Returns list of addresses which were found by search query.
   *
   * @return \stdClass[]
   *   List of objects which meet search criteria.
   */
  private function fetchResults() {
    $countries = pinmap_countries(FALSE);
    $results = [];

    foreach ($this->getQueryTargets() as list($content_type, $field_name)) {
      // Prepare the query.
      $query = strtr(call_user_func([$this->searcher, 'query'], $this->query), [
        '[table]' => "field_data_{$field_name}",
        '[field]' => $field_name,
      ]);

      foreach (db_query($query, [':bundle' => $content_type]) as $item) {
        foreach ((array) $item as $property => $value) {
          // Remove original key.
          unset($item->{$property});
          // And replace by non-prefixed.
          $item->{str_replace("{$field_name}_", '', $property)} = $value;
        }

        $item->data = empty($item->data) ? [] : unserialize($item->data);

        // Decode country code to full name.
        if (!empty($countries[$item->country])) {
          $item->country = $countries[$item->country];
        }

        $results[$item->entity_id] = $item;
      }
    }

    drupal_alter('pinmap_search_results', $results, $this->query);

    return $results;
  }

  /**
   * Returns searcher for building the query.
   *
   * @param string $type
   *   One of available search types.
   *
   * @return SearcherInterface
   *   FQN of searcher class.
   */
  private function getSearcher($type) {
    if (empty($this->searchers[$type])) {
      throw new \InvalidArgumentException(sprintf('Search type "%s" is not available', $type));
    }

    return $this->searchers[$type];
  }

  /**
   * Static query validator.
   *
   * @param array $query
   *   Search query to validate.
   * @param array $params
   *   Search requirements to seek for in query.
   */
  private static function validateQueryParams(array $query, array $params) {
    $params['is_array'][] = 'fields';
    $params['is_array'] = array_unique($params['is_array']);

    foreach ($params as $validator => $options) {
      foreach ($options as $option) {
        if (empty($query[$option])) {
          throw new \InvalidArgumentException(sprintf('Request is missing "%s" required option', $option));
        }

        if (!$validator($query[$option])) {
          throw new \InvalidArgumentException(sprintf('Request option "%s" cannot pass validation by "%s"', $option, $validator));
        }
      }
    }
  }

}
