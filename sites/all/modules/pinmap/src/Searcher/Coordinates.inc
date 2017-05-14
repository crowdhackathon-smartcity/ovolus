<?php

namespace Drupal\pinmap\Searcher;

/**
 * Seek for addresses by latitude and longitude of location.
 */
final class Coordinates implements SearcherInterface {

  /**
   * {@inheritdoc}
   */
  public static function requirements() {
    return [
      'is_numeric' => ['lat', 'lng', 'radius', 'limit'],
    ];
  }

  /**
   * {@inheritdoc}
   *
   * @link https://ru.scribd.com/presentation/2569355/Geo-Distance-Search-with-MySQL
   */
  public static function query(array $query) {
    return "
      SELECT
        *,
        6371 * ASIN(SQRT(
          POWER(SIN(({$query['lat']} - abs([field]_lat)) * pi() / 180 / 2), 2) +
          COS({$query['lat']} * pi() / 180) *
          COS(abs([field]_lat) * pi() / 180) *
          POWER(SIN(({$query['lng']} - [field]_lng) * pi() / 180 / 2), 2)
        )) as distance
      FROM
        [table]
      WHERE
        bundle = :bundle
      AND
        [field]_lat IS NOT NULL
      AND
        [field]_lng IS NOT NULL
      HAVING
        distance < {$query['radius']}
      ORDER BY
        distance
      LIMIT
        0, {$query['limit']}
    ";
  }

}
