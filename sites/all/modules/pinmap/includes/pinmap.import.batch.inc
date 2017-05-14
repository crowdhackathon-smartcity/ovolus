<?php

/**
 * @file
 * Batch operation for importing address from CSV.
 */

/**
 * Batch operation callback.
 *
 * @see _pinmap_import_form()
 */
function _pinmap_batch_import_address(array $conformity, array $line, $content_type, $field_name, array &$context) {
  $data = [];

  foreach ($conformity as $destination => $source) {
    $item = $line[$source];

    if (0 === strpos($item, '<~') && strlen($item) - 2 === strrpos($item, '~>')) {
      $item = ltrim($item, '<~');
      $item = rtrim($item, '~>');
      $decoded = [];

      foreach (explode('~><~', $item) as $pair) {
        list($key, $value) = explode('=', $pair, 2);
        $decoded[$key] = $value;
      }

      $item = $decoded;
    }

    drupal_array_set_nested_value($data, explode(':', $destination), $item);
  }

  try {
    pinmap_create_node($content_type, $field_name, $data);
    $context['results'][] = FALSE;
  }
  catch (\Exception $e) {
    $context['results'][] = $e->getMessage();
  }
}
