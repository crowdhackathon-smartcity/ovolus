<?php

/**
 * @file
 * Main UI for module.
 */

use Drupal\pinmap\Controller\Search;

/**
 * {@inheritdoc}
 *
 * @see pinmap_menu()
 *
 * @internal
 */
function _pinmap_main_form(array $form) {
  $providers = [];
  $options = [];

  foreach (pinmap_providers() as $name => $class) {
    $options[$name] = $name;

    $providers[$class::variable('api_key')] = [
      '#type' => 'textfield',
      '#title' => t('@provider API key', ['@provider' => $name]),
      '#description' => t("With API key you'll be able to perform more requests to !provider API. Read more about an API: !api_docs.", [
        '!provider' => $name,
        '!api_docs' => l($class::SERVICE_URL, $class::SERVICE_URL, ['attributes' => ['target' => '_blank']]),
      ]),
      '#default_value' => $class::getApiKey(),
      '#states' => [
        'visible' => [
          ':input[name*=pinmap_provider]' => ['value' => $name],
        ],
      ],
    ];
  }

  // @see pinmap_default_content_type_disabled()
  $form['pinmap_default_content_type_disabled'] = [
    '#type' => 'checkbox',
    '#title' => t('Disable default content type (%type)', [
      '%type' => PINMAP_DEFAULT_CONTENT_TYPE,
    ]),
    '#description' => t('Correctly-configured content type with an address field. Created for usage and demonstration.'),
  ];

  // @see pinmap_is_search_cache_enabled()
  $form['pinmap_is_search_cache_enabled'] = [
    '#type' => 'checkbox',
    '#title' => t('Enable caching of search results'),
  ];

  // @see pinmap_is_search_debug_enabled()
  $form['pinmap_is_search_debug_enabled'] = [
    '#type' => 'checkbox',
    '#title' => t('Enable debug messages for wrong search queries'),
    '#description' => t('Use this option in development only. All wrongly built queries will be logged by Watchdog.'),
  ];

  $form['pinmap_provider'] = [
    '#type' => 'select',
    '#title' => t('Geocoding provider'),
    '#options' => $options,
    '#description' => t('API calls to selected provider will be performed during saving the value of address field.'),
  ];

  $form += $providers;

  $form['#validate'][] = __FUNCTION__ . '_validate';
  // Reset content types cache to react on disabling default content type.
  $form['#submit'][] = 'node_type_cache_reset';

  // Set default values.
  foreach (element_children($form) as $child) {
    if (!array_key_exists('#default_value', $form[$child]) && function_exists($child)) {
      $form[$child]['#default_value'] = $child();
    }
  }

  return system_settings_form($form);
}

/**
 * {@inheritdoc}
 *
 * @see _pinmap_main_form()
 *
 * @internal
 */
function _pinmap_main_form_validate(array &$form, array &$form_state) {
  // Yeah, babe! I was born there :)
  $provider = pinmap_provider('Ordzhonikidze', $form_state['values']['pinmap_provider']);
  $api_key_field_name = $provider::variable('api_key');

  try {
    $provider->requestApiKeyChange($form_state['values'][$api_key_field_name]);
  }
  catch (\RuntimeException $e) {
    form_error($form[$api_key_field_name], $e->getMessage());
    // Disallow to change provider until API key won't be correct or empty.
    $form['pinmap_provider']['#attributes']['disabled'] = TRUE;
  }
  catch (\Exception $e) {
    // Here could an error about incorrect coordinates or address.
  }
}

/**
 * {@inheritdoc}
 *
 * @see pinmap_menu()
 *
 * @internal
 */
function _pinmap_search() {
  try {
    $search = new Search();
    $search->setQuery(drupal_get_query_parameters());

    print drupal_json_encode(array_values($search->getResults()));
  }
  catch (\Exception $e) {
    if (pinmap_is_search_debug_enabled()) {
      watchdog_exception(ltrim(__FUNCTION__, '_'), $e);
    }
  }
}
