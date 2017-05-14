<?php

/**
 * @file
 * Batch operation for exporting address from DB.
 */

/**
 * Batch operation callback.
 *
 * @see _pinmap_export_form()
 */
function _pinmap_batch_export_address($nid, $field, $file, array $header, array &$context) {
  $node = node_load($nid);
  $status = isset($node->$field) && !empty($node->{$field}[LANGUAGE_NONE]);

  if ($status) {
    foreach ($node->{$field}[LANGUAGE_NONE] as $value) {
      $value['data'] = unserialize($value['data']);
      $row = [];

      foreach ($header as $column => $label) {
        $parents = explode(':', $column);
        $column = end($parents);

        $row[$column] = drupal_array_get_nested_value($value, $parents);

        // Handle old structure when data were stored in "data['coords']".
        if (empty($row[$column]) && in_array($column, ['lat', 'lng'])) {
          $row[$column] = drupal_array_get_nested_value($value, ['data', 'coords', $column]);
        }
      }

      file_put_contents($file, "\n" . implode(',', $row), FILE_APPEND);
    }
  }

  $context['results'][] = !$status;
}
