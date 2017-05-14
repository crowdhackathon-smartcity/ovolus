<?php

/**
 * @file
 * UI for exporting addresses.
 */

/**
 * {@inheritdoc}
 *
 * @see pinmap_menu()
 *
 * @internal
 */
function _pinmap_export_form(array $form, array &$form_state) {
  $values = isset($form_state['values']['data']) ? $form_state['values']['data'] : [];
  $button = t('Continue');

  $values['operation'] = pinmap_form_state_get_operation($form_state);
  pinmap_operation_set_title($values['operation']);

  if (empty($values['field'])) {
    $form += pinmap_form_source_field(t('Source'));
  }
  else {
    $values['file'] = pinmap_get_file_name($values['type'], $values['field'], $values['operation']);

    $form['confirm'] = [
      '#type' => 'item',
      '#title' => t('Are you sure you want to export @count nodes?', [
        '@count' => count($values['nodes']),
      ]),
    ];

    if (file_exists($values['file'])) {
      $form['confirm']['#description'] = t('Latest exported file available here: <a href="@url">@url</a>', [
        '@url' => file_create_url($values['file']),
      ]);
    }

    $button = t('Export');
  }

  return pinmap_multistep_base_form($form, $values, $button);
}

/**
 * {@inheritdoc}
 *
 * @internal
 */
function _pinmap_export_form_submit(array $form, array &$form_state) {
  // Form needed to be rebuilt every time, except the last step.
  $form_state['rebuild'] = TRUE;

  $values =& $form_state['values'];
  // The data field is always visible and needed to store all values during
  // steps walking.
  $data =& $values['data'];

  // First step: choosing source field.
  if (isset($form['field'])) {
    // Example string: "content_type:field_name".
    list($data['type'], $data['field']) = pinmap_parse_destination($values['field']);
    $data['nodes'] = db_select('node', 'n')
      ->fields('n', ['nid'])
      ->condition('type', $data['type'])
      ->execute()
      ->fetchCol();
  }

  // Last step: export accepted.
  if (isset($form['confirm'])) {
    $form_state['rebuild'] = FALSE;
    $header = pinmap_address_columns();

    foreach ($data['nodes'] as $i => $nid) {
      $data['nodes'][$i] = [$nid, $data['field'], $data['file'], $header];
    }

    file_put_contents($data['file'], implode(',', $header));
    pinmap_batch_set($data['operation'], $data['nodes']);
  }
}
