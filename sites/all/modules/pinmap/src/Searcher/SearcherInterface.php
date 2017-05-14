<?php

namespace Drupal\pinmap\Searcher;

/**
 * Searcher definition.
 */
interface SearcherInterface {

  /**
   * Returns requirements for parameters of search query and their validators.
   *
   * IMPORTANT: to explicitly specified requirements of searcher will be added
   * an additional one, based on which search mechanism has built: an array of
   * fields. It must be an indexed array where values are strings containing
   * the content type and name of field attached to it, divided by colon.
   *
   * @return array[]
   *   Key is name of PHP function which takes single argument for input -
   *   value of specified query parameter. Value - an array of parameters
   *   which must exists in query.
   *
   * @example
   * Query: /pinmap/search?fields%5B0%5D=content_type%3Afield_name&data1%5B0%5D=value&data1%5B1%5D=value2
   *
   * @code
   * [
   *   'is_array' => ['data1'],
   * ]
   * @endcode
   *
   * @see Search::setQuery()
   * @see Search::getQueryTargets()
   */
  public static function requirements();

  /**
   * Returns SQL query to database for performing search.
   *
   * @param array $query
   *   List of arguments passed as a query. If you correctly defined the list
   *   of requirements then here you are safely to use any of those parameters.
   *   Undefined values also can be passed, but no guarantees that they will
   *   be in correct format.
   *
   * @return string
   *   Unusual SQL query. Since queries are performed to Drupal fields then we
   *   must explicitly specify the bundle of entity and prefix for each column
   *   in the table of field. Use the following placeholders in forming the
   *   queries:
   *     - "[table]" - name of table to query;
   *     - "[field]" - name of field, which always prefixes column name;
   *     - ":bundle" - bundle of entity to provide correct search results.
   *
   * @example
   * Presented below queries might be completely the same. Do not forget to
   * use placeholders every time.
   *
   * @code
   * SELECT * FROM [table] WHERE bundle = :bundle AND [field]_country = 'DE';
   * SELECT * FROM field_data_field_address WHERE bundle = 'user_address' AND field_address_country = 'DE';
   * @endcode
   *
   * @see Search::fetchResults()
   */
  public static function query(array $query);

}
