<?php

/**
 * @file
 * Collect all CTools plugins of "Access" type.
 */

use CTools\PluginContexts;

/**
 * Implements <MODULE>_<FILENAME>_ctools_access().
 */
function ctools_api_ctools_api_ctools_access(array $info) {
  return ctools_api_type_definition_get($info['type']) + [
    'title' => t('@module Access', [
      '@module' => CTOOLS_API_MODULE_TITLE,
    ]),
  ];
}

/**
 * {@inheritdoc}
 *
 * @see \CTools\PluginConstructor::callTypeConstructor()
 * @see ctools_access_ajax_edit_item_submit()
 * @see panels_edit_configure_access_test_form_submit()
 * @see ctools_api_plugin_base_configuration_form_submit()
 */
function ctools_api_access(array &$information, array $interfaces) {
  foreach (['summary', 'callback', 'settings form'] as $property) {
    $information[$property] = __FUNCTION__ . '_base_' . str_replace(' ', '_', $property);
  }
}

/**
 * {@inheritdoc}
 *
 * @see ctools_access_summary()
 */
function ctools_api_access_base_summary($conf, $context, array $plugin, $callback = 'summary') {
  return $plugin['object']::$callback((array) $conf, new PluginContexts($context));
}

/**
 * {@inheritdoc}
 *
 * @see ctools_access()
 */
function ctools_api_access_base_callback($conf, $context, array $plugin) {
  return ctools_api_access_base_summary($conf, $context, $plugin, 'accessible');
}

/**
 * {@inheritdoc}
 *
 * @see ctools_access_ajax_edit_item()
 */
function ctools_api_access_base_settings_form(array $form, array &$form_state, $conf) {
  $items = [];

  $conf = (array) $conf;
  $conf['context'] = isset($form_state['test']['context']) ? $form_state['test']['context'] : [];

  $form_state['conf'] = $conf;
  // The form need to be rebuilt every time, to keep panels display updated.
  // WARNING! Configuration will not be saved if this construction will be
  // removed.
  $form_state['rebuild'] = TRUE;

  // To have a possibility to use AJAX calls in configuration form of
  // access plugin.
  ctools_form_include($form_state, 'plugins');
  ctools_api_form_include_file($form_state, __FILE__);
  ctools_api_plugin_base_configuration_form($items, $form_state);

  $form['settings'] += $items;

  return $form;
}
