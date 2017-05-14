<?php

/**
 * @file
 * Auxiliary functionality.
 */

/**
 * Get titles for available operations.
 *
 * @return string[]
 *   Operation titles.
 */
function pinmap_operation_titles() {
  return [
    'import' => t('Import from CSV'),
    'export' => t('Export to CSV'),
  ];
}

/**
 * Set predefined page title using operation name.
 *
 * @param string $operation
 *   Operation name.
 */
function pinmap_operation_set_title($operation) {
  $titles = pinmap_operation_titles();

  if (isset($titles[$operation])) {
    drupal_set_title($titles[$operation]);
  }
}

/**
 * Get content types and their field names.
 *
 * @return array[]
 *   Content type and field machine names, grouped by content type label.
 */
function pinmap_get_valid_fields() {
  $types = node_type_get_types();
  $result = [];

  foreach (field_info_instances('node') as $type => $instances) {
    foreach ($instances as $field => $instance) {
      // Valid field - is "addressfield".
      if ('addressfield' === $instance['widget']['module'] && isset($types[$type])) {
        // @example
        // @code
        // [
        //   'Map marker' => [
        //     'map_marker:field_address' => 'Address',
        //   ],
        // ]
        // @endcode
        $result[$types[$type]->name]["$type:$field"] = $instance['label'];
      }
    }
  }

  return $result;
}

/**
 * Returns an array with content type and field name.
 *
 * @param string $field
 *   Parse an option produced by "pinmap_get_valid_fields()".
 *
 * @return string[]
 *   Parsed option.
 *
 * @see pinmap_get_valid_fields()
 */
function pinmap_parse_destination($field) {
  return explode(':', $field, 2);
}

/**
 * Create node with address.
 *
 * @param string $content_type
 *   Content type machine name (bundle name of the "node" entity).
 * @param string $field_name
 *   Field machine name. Must be attached to specified bundle.
 * @param array $value
 *   Value for field.
 *
 * @throws \InvalidArgumentException
 *   When country was not specified.
 * @throws \RuntimeException
 *   When country does not exists.
 * @throws \Exception
 *   When DB transaction fails.
 *
 * @return \stdClass
 *   Created node.
 */
function pinmap_create_node($content_type, $field_name, array $value) {
  static $countries = [], $columns = [];

  if (!isset($value['country'])) {
    throw new \InvalidArgumentException(t('Country field is required!'));
  }

  if (empty($columns)) {
    module_load_include('install', 'addressfield');
    $columns = array_fill_keys(array_keys(addressfield_field_schema()['columns']), '');
  }

  if (empty($countries)) {
    // Transform "DE => Germany" to "germany => de".
    $countries = array_flip(pinmap_countries());
  }

  // Need to process the name of country. First of all it should be in
  // lowercase, to prevent unexpected sets of characters. Then, we know
  // that every word in country name starts from a capital letter. Then,
  // when name was processed, translate it and transform back to lowercase.
  // @codingStandardsIgnoreStart
  $key = strtolower(t(ucwords(strtolower($value['country']))));
  // @codingStandardsIgnoreEnd
  $node = new \stdClass();

  // Check country using full name.
  if (!isset($countries[$key])) {
    // If search by full name failed then try to find using country code.
    $key = array_search($key, $countries);

    // Unable continue without country.
    if (FALSE === $key) {
      throw new \RuntimeException(t('Country "@country" does not exists!', [
        '@country' => $value['country'],
      ]));
    }
  }

  $node->type = $content_type;
  $node->status = 1;
  $node->language = LANGUAGE_NONE;

  node_object_prepare($node);

  // @see pinmap_entity_presave()
  if (!empty($value['lat']) && !empty($value['lng'])) {
    $value[PINMAP_IGNORE_API_CALL_MARKER] = TRUE;
  }

  // Here allowed country code only (length is "2").
  // @see addressfield_field_schema()
  $value['country'] = strtoupper($countries[$key]);
  $value = array_merge($columns, $value);
  $value['data'] = serialize($value['data']);

  $node->{$field_name}[LANGUAGE_NONE][] = $value;

  node_save($node);

  return $node;
}

/**
 * Get the list of country codes and names.
 *
 * @param bool $to_lower
 *   Whether to transform all labels and country keys to lowercase.
 *
 * @return string[]
 *   List of country codes and associated country names.
 */
function pinmap_countries($to_lower = TRUE) {
  $list = _addressfield_country_options_list();

  if ($to_lower) {
    $list = array_change_key_case(array_map('strtolower', $list), CASE_LOWER);
  }

  return $list;
}

/**
 * Set batch operations.
 *
 * @param string $operation
 *   Operation name ("import" or "export").
 * @param array $arguments
 *   Operation arguments.
 */
function pinmap_batch_set($operation, array $arguments) {
  $file = drupal_get_path('module', 'pinmap') . "/includes/pinmap.$operation.batch.inc";

  if (file_exists($file)) {
    foreach ($arguments as $i => $argument) {
      $arguments[$i] = ["_pinmap_batch_{$operation}_address", $argument];
    }

    batch_set([
      'file' => $file,
      'title' => t('@type addresses', [
        '@type' => ucfirst($operation),
      ]),
      'finished' => '_pinmap_batch_finished',
      'operations' => $arguments,
    ]);

    if (function_exists('background_batch_process_batch')) {
      background_batch_process_batch();
      drupal_goto('admin/config/system/batch/overview');
    }
  }
}

/**
 * Columns names of the address field.
 *
 * @param bool $testing_mode
 *   If TRUE, then labels will be replaced with dummy values.
 *
 * @throws \Exception
 *   When developer is a addict and broke values for testing.
 *
 * @return string[]
 *   An associative array with column names and labels.
 */
function pinmap_address_columns($testing_mode = FALSE) {
  // IMPORTANT: an order of array items MUST NOT be changed.
  $values = [
    'organisation_name' => 'Company',
    'thoroughfare' => 'Address',
    'postal_code' => 'Post code',
    'locality' => 'City',
    'country' => 'Country',
    'lat' => 'Location latitude',
    'lng' => 'Location longitude',
    // Will be transformed to:
    // @code
    // [
    //   'data' => [
    //     'phone_number' => '<VALUE>',
    //   ],
    // ]
    // @endcode
    //
    // Multiple nesting levels available.
    'data:phone_number' => 'Phone',
    // @todo Provide an ability to use "information" field
    // as part of the "addressfield" widget.
    // 'data:information' => 'Information',
  ];

  if ($testing_mode) {
    // Always keep this values updated! When add/remove a field
    // to/from "$values" array DO NOT FORGET to update this one.
    $testing_values = [
      'organisation_name' => 'Otavi Apotheke',
      'thoroughfare' => 'Müllerstraße 64',
      'postal_code' => '13349',
      'locality' => 'Berlin',
      'country' => 'DE',
      'lat' => 52.55608,
      'lng' => 13.3428,
      'phone_number' => '+49 30 4525269',
    ];

    if (count($values) !== count($testing_values)) {
      throw new \Exception('Oops, your dummy values for testing are differs from defined columns!');
    }

    return $testing_values;
  }

  return $values;
}

/**
 * Get file name depending on operation.
 *
 * @param string $content_type
 *   Machine name of content type.
 * @param string $field_name
 *   Machine name of field.
 * @param string $operation
 *   One of the operations ("import" or "export").
 *
 * @return string
 *   Processed string.
 */
function pinmap_get_file_name($content_type, $field_name, $operation) {
  return sprintf(PINMAP_MARKERS_FILE, $content_type, $field_name, $operation);
}

/**
 * Get form element for choosing source field.
 *
 * @param string $label
 *   Field label.
 * @param bool $multiple
 *   Must be used only in content type plugin definition because there
 *   we should not use redirection and able to display location from
 *   multiple sources.
 *
 * @return array[]
 *   Form elements.
 */
function pinmap_form_source_field($label, $multiple = FALSE) {
  $fields = pinmap_get_valid_fields();
  $form = [];

  if (empty($fields)) {
    drupal_set_message(t('You have no correctly configured content types.'), 'warning');

    if ($multiple) {
      drupal_goto(PINMAP_ADMIN_PATH);
    }
  }
  else {
    $form['field'] = [
      '#type' => 'select',
      '#title' => $label,
      '#options' => $fields,
      '#required' => $multiple,
      '#multiple' => $multiple,
    ];
  }

  return $form;
}

/**
 * Elements for every multi step form.
 *
 * @param array $form
 *   Form element definitions.
 * @param array $values
 *   Saved values from previous steps.
 * @param string $button
 *   Text for submit button.
 *
 * @return array[]
 *   Updated form.
 */
function pinmap_multistep_base_form(array $form, array $values, $button) {
  $form['data'] = [
    '#type' => 'value',
    '#value' => $values,
  ];

  $form['actions'] = [
    '#type' => 'actions',
  ];

  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => $button,
  ];

  return $form;
}

/**
 * Extract operation name form a build information.
 *
 * @param array $form_state
 *   Drupal form state.
 *
 * @return string
 *   Operation name ("import" or "export");
 *
 * @see pinmap_menu()
 */
function pinmap_form_state_get_operation(array $form_state) {
  return reset($form_state['build_info']['args']);
}

/**
 * Attach an address field to a content type.
 *
 * @param string $content_type
 *   Content type.
 * @param string $field_name
 *   Field name.
 */
function pinmap_attach_address_field($content_type, $field_name) {
  $field = [
    'type' => PINMAP_EXTENDS_FIELD_TYPE,
    'field_name' => $field_name,
  ];

  $instance = [
    'type' => PINMAP_EXTENDS_FIELD_WIDGET,
    'label' => 'Address',
    'bundle' => $content_type,
    'field_name' => $field_name,
    'entity_type' => 'node',
    'widget' => [
      'settings' => [
        'format_handlers' => [
          'phone' => 'phone',
          'address' => 'address',
          'name-full' => FALSE,
          'organisation' => 'organisation',
          'name-oneline' => FALSE,
          'address-optional' => FALSE,
          'address-hide-street' => FALSE,
          'address-hide-country' => FALSE,
          'address-hide-postal-code' => FALSE,
        ],
        'phone_number_fields' => [
          'fax' => 0,
          'phone' => 'phone',
          'mobile' => 0,
          'extension' => 0,
        ],
      ],
    ],
  ];

  foreach (['field' => $field, 'instance' => $instance] as $type => $data) {
    try {
      call_user_func("field_create_{$type}", $data);
    }
    catch (\Exception $e) {
      watchdog(__FUNCTION__, $e->getMessage(), [], WATCHDOG_WARNING);
    }
  }
}

/**
 * Select values from field table.
 *
 * @param string $entity_type
 *   An entity type.
 * @param string $bundle
 *   Entity bundle.
 * @param string $field_name
 *   Field name of the entity.
 * @param string[] $columns
 *   Column names in field table.
 *
 * @return \SelectQuery
 *   Query object.
 *
 * @throws \InvalidArgumentException
 *   When field does not exists.
 */
function pinamp_field_query($entity_type, $bundle, $field_name, array $columns = []) {
  $field_info = field_info_field(PINMAP_DEFAULT_FIELD_NAME);

  if (empty($field_info)) {
    throw new \InvalidArgumentException(t('Field name @name is invalid.', ['@name' => $field_name]));
  }

  foreach ($columns as $i => $column) {
    if (isset($field_info['columns'][$column])) {
      $columns[$i] = $field_name . '_' . $column;
    }
  }

  return db_select('field_data_' . $field_name, $field_name)
    ->fields($field_name, $columns)
    ->condition('entity_type', $entity_type)
    ->condition('bundle', $bundle);
}

/**
 * Batch finish callback.
 *
 * @param bool $success
 *   Weather of batch execution.
 * @param array $results
 *   Values from batch callback ($context['results']).
 * @param array $operations
 *   Operations that remained unprocessed.
 * @param string $time
 *   Time that was spent for batch execution.
 *
 * @see pinmap_batch_set()
 *
 * @internal
 */
function _pinmap_batch_finished($success, array $results, array $operations, $time) {
  $status = 'status';
  $failed = count(array_filter($results));
  $succeed = count($results) - $failed;
  $message = [];

  // Status information about operations.
  $message[] = "Operation was performed for $time.";
  // Add an empty line.
  $message[] = '';
  // Tell about successfully imported addresses.
  $message[] = "Succeed: $succeed.";
  // Tell about items that were not imported.
  $message[] = "Failed: $failed.";

  // Warn user about failed operations.
  if ($failed > 0) {
    $status = 'warning';
  }

  if (!$success) {
    list($operation, $arguments) = reset($operations);

    // Add an empty line.
    $message[] = '';
    $message[] = "An error occurred while processing $operation with arguments: " . print_r($arguments, TRUE);
    $status = 'error';
  }

  drupal_set_message(implode('<br>', $message), $status);
}

/**
 * Restrict access to pages that depends on correctly-configured content type.
 *
 * @param string $arguments
 *   Access arguments.
 *
 * @return bool
 *   State.
 *
 * @see pinmap_menu()
 *
 * @internal
 */
function _pinmap_operation_accessible($arguments) {
  return !empty(pinmap_get_valid_fields()) && user_access($arguments);
}

/**
 * Clear cache of search results.
 *
 * @see pinmap_drush_cache_clear()
 *
 * @internal
 */
function _pinmap_cache_search_clear() {
  cache_clear_all('*', PINMAP_CACHE_SEARCH_BIN, TRUE);
}
