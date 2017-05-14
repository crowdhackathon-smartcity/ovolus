<?php

namespace CTools\Plugins\Access;

use CTools\PluginContexts;

/**
 * Class AccessPlugin.
 *
 * @package CTools\Plugins\Access
 */
abstract class AccessPlugin implements AccessInterface {

  /**
   * {@inheritdoc}
   */
  public static function info() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function contexts() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function dependencies() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormValidate(array &$form, array &$form_state, array &$values, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormSubmit(array &$form, array &$form_state, array &$conf, PluginContexts $contexts) {

  }

  /**
   * {@inheritdoc}
   */
  public static function configurationFormElementCallback(array &$form, array &$form_state, array &$conf, PluginContexts $contexts, array &$commands, array $element) {

  }

}
