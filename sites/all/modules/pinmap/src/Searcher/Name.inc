<?php

namespace Drupal\pinmap\Searcher;

/**
 * Seek for addresses by approximate name of organization.
 */
final class Name implements SearcherInterface {

  /**
   * {@inheritdoc}
   */
  public static function requirements() {
    return [
      'is_string' => ['name'],
      'is_numeric' => ['limit'],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function query(array $query) {
    return "
      SELECT
        *
      FROM
        [table]
      WHERE
        bundle = :bundle
      AND
        [field]_lat IS NOT NULL
      AND
        [field]_lng IS NOT NULL
      AND
        [field]_organisation_name LIKE '%{$query['name']}%'
      LIMIT
        0, {$query['limit']}
    ";
  }

}
