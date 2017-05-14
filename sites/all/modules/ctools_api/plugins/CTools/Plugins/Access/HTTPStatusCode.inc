<?php

namespace CTools\Plugins\Access;

use CTools\PluginContexts;

/**
 * Class HTTPStatusCode.
 *
 * @package CTools\Plugins\Access
 */
class HTTPStatusCode extends AccessPlugin {

  /**
   * {@inheritdoc}
   */
  public static function name() {
    return t('HTTP status code');
  }

  /**
   * {@inheritdoc}
   */
  public static function info() {
    return [
      'description' => t('Restrict an access by HTTP status code(s).'),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function summary(array $conf, PluginContexts $contexts) {
    return t('Content available when server responds with the following HTTP codes: @codes', [
      '@codes' => implode(', ', $conf['available_codes']),
    ]);
  }

  /**
   * {@inheritdoc}
   */
  public static function accessible(array $conf, PluginContexts $contexts) {
    // The drupal_get_http_header() return something like: "404 Not Found".
    return in_array((int) drupal_get_http_header('Status'), $conf['available_codes']);
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationForm(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    $form['available_codes'] = [
      '#type' => 'checkboxes',
      '#title' => t('Choose HTTP status code for which accesses available'),
      // HTTP codes, handled by Drupal.
      '#options' => drupal_map_assoc([403, 404, 500, 503]),
      '#required' => TRUE,
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {
    $conf['available_codes'] = array_filter($conf['available_codes']);
  }

}
