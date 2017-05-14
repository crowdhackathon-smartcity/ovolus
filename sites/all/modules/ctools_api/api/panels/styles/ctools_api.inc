<?php

/**
 * @file
 * Collect all Panels plugins of "Styles" type.
 */

use CTools\PluginContexts;

/**
 * Implements <MODULE>_<FILENAME>_panels_styles().
 */
function ctools_api_ctools_api_panels_styles(array $info) {
  return ctools_api_type_definition_get($info['type']) + [
    'title' => t('@module Style', [
      '@module' => CTOOLS_API_MODULE_TITLE,
    ]),
  ];
}

/**
 * {@inheritdoc}
 *
 * @see \CTools\PluginConstructor::callTypeConstructor()
 */
function ctools_api_styles(array &$information, array $interfaces) {
  foreach (['pane', 'region'] as $renderer) {
    if (isset($interfaces['CTools\Plugins\Styles\Styles' . ucfirst($renderer) . 'Interface'])) {
      $prefix = 'pane' === $renderer ? "$renderer " : '';

      $information['render ' . $renderer] = __FUNCTION__ . "_base_render_$renderer";
      $information[$prefix . 'settings form'] = __FUNCTION__ . "_base_settings_form";
      break;
    }
  }

  $information += [
    'description' => '',
  ];

  // Prevent creation additional theme suggestions.
  // @see panels_theme()
  unset($information['hook theme']);
}

/**
 * {@inheritdoc}
 *
 * @see ctools_api_styles()
 */
function theme_ctools_api_styles_base_render_region(array $variables) {
  $plugin = $variables['style'];
  $conf =& $variables['settings'];

  $plugin['object']::preprocess(
    $variables['display'],
    $conf,
    $variables,
    ctools_api_styles_display_contexts($variables['display'], $plugin, $conf)
  );

  return ctools_api_apply_theme($plugin['object'], $conf, $plugin, $variables);
}

/**
 * {@inheritdoc}
 *
 * @see ctools_api_styles()
 */
function theme_ctools_api_styles_base_render_pane(array $variables) {
  $content =& $variables['content'];
  $plugin = $variables['style'];
  $conf =& $variables['settings'];
  $parts = explode('\\', $variables['pane']->subtype);

  $content->title_overriden = !empty($content->title_heading);
  // Collect the CSS settings of panel into array that can be rendered by
  // "drupal_attributes()" function.
  $content->attributes = [
    'class' => [],
  ];

  foreach (['class', 'id'] as $prop) {
    $object_prop = "css_$prop";

    if (!empty($content->$object_prop)) {
      $content->attributes[$prop] = explode(' ', $content->$object_prop);
      unset($content->$object_prop);
    }
  }

  // Add transformed vendor and object names to pane classes.
  $content->attributes['class'] = array_merge($content->attributes['class'], array_map('ctools_api_to_dashes', array_unique([
    reset($parts),
    end($parts),
  ])));

  if ($content->title_overriden) {
    $content->title = '';
  }

  $plugin['object']::preprocess(
    $content,
    $conf,
    $variables,
    ctools_api_styles_display_contexts($variables['display'], $plugin, $conf)
  );

  return ctools_api_apply_theme($plugin['object'], $conf, $plugin, $variables);
}

/**
 * {@inheritdoc}
 *
 * @see panels_edit_style_settings_form()
 */
function ctools_api_styles_base_settings_form($settings, $display, $pid, $type, array &$form_state) {
  $items = [];

  ctools_api_form_include_file($form_state, __FILE__);
  ctools_api_plugin_base_configuration_form($items, $form_state);

  return $items;
}

/**
 * Select configured contexts.
 *
 * @param \panels_display $display
 *   Panels display object.
 * @param array $plugin
 *   Plugin definition.
 * @param array $conf
 *   Plugin settings.
 *
 * @return PluginContexts
 *   An instance of plugin contexts object.
 */
function ctools_api_styles_display_contexts(\panels_display $display, array $plugin, array $conf) {
  // Ensure that "context" key will be always presented to not see PHP notices.
  $conf += ['context' => []];

  return new PluginContexts(
    ctools_context_select(ctools_api_display_contexts($display), $plugin['required context'], $conf['context'])
  );
}
